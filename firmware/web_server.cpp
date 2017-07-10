#include <assert.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include "matrix_drive.h"

static ESP8266WebServer server(80);
static const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"; // TODO: make this on flash

enum ota_status_t { ota_header, ota_content, ota_success, ota_fail };
static ota_status_t ota_status;
static StreamString LastOTAError;

/*
	The OTA handler receives combined ROM
	(which includes main text, main filesystem, font data) in one file.

	ROM image package structure: (one block is 2048 byte length)
	MAIN_HEADER  (1 block)
	MAIN_CONTENT (N blocks)
	FS_HEADER (1 block)
	FS_CONTENT (N blocks)
*/


static void web_server_ota_upload_handler()
{
	static uint32_t remaining_length;


	#pragma pack(push, 1)
	struct header_t
	{
		char magic; // must be 0xea
		char dum1[6]; // " PART "
		char partition; // either 'M' (main) or 'F' (fs)
		char dum2[24]; // " BLOCK HEADER           "
		uint32_t length; // content length
		uint32_t block_count; // block count of content
		char dum3[2048 - 1-6-1-24-4-4-32];
		char md5[32]; // md5 sum of the content

		String get_md5() const
		{
			// make zero-terminated string and return
			char buf[33];
			memcpy(buf, md5, 32);
			buf[32] = 0;
			return String(buf);
		}
	};
	#pragma pack(pop)


	HTTPUpload& upload = server.upload();
	if(upload.status == UPLOAD_FILE_START){
		led_stop_pwm_clock(); // stop harmful LED PWM clock which will interfere with WiFi.
		Serial.setDebugOutput(true);
		WiFiUDP::stopAll();
		Serial.printf_P(PSTR("Update: %s\n"), upload.filename.c_str());
		ota_status = ota_header;
	} else if(upload.status == UPLOAD_FILE_WRITE){
		static_assert(HTTP_UPLOAD_BUFLEN == 2048, "HTTP_UPLOAD_BUFLEN must be 2048");
		Serial.printf(".");
		// we assume that the upload content comes by unit of each 2048 byte block.
		const header_t *pheader = reinterpret_cast<const header_t*>
			(upload.buf);

		// some sanity check and dump
		if(ota_status == ota_header)
		{
			Serial.print(F("\r\n"));
			if(pheader->magic == 0x50 &&
				pheader->dum1[0] == 0x4b &&
				pheader->dum1[1] == 0x03 &&
				pheader->dum1[2] == 0x04)
			{
				LastOTAError.print(F("Invalid header. The file seems to be a ZIP file. Please unzip it first.\r\n"));
				Serial.println(LastOTAError.c_str());
				ota_status = ota_fail; // invalid header
				return;
			}
				
			if(pheader->magic != 0xea ||
				pheader->dum1[0] != ' ' ||
				pheader->dum1[1] != 'P' ||
				pheader->dum1[2] != 'A' ||
				pheader->dum1[3] != 'R' ||
				pheader->dum1[4] != 'T' ||
				pheader->dum1[5] != ' ')
			{
				LastOTAError.print(F("Invalid header. The image file may not be a proper firmware.\r\n"));
				Serial.println(LastOTAError.c_str());
				ota_status = ota_fail; // invalid header
				return;
			}
			Serial.printf_P(PSTR("pheader->partition: 0x%x\r\n"), pheader->partition);
			Serial.printf_P(PSTR("pheader->length: %u\r\n"), pheader->length);
			Serial.printf_P(PSTR("pheader->block_count: %u\r\n"), pheader->block_count);


			if(pheader->partition == 'M')
			{
				// main text
				if(!Update.begin(pheader->length))
				{
					Update.printError(LastOTAError);
					Serial.println(LastOTAError.c_str());
					ota_status = ota_fail;
					return;
				}
			}
			else if(pheader->partition == 'F')
			{
				// filesystem and font
				if(!Update.begin(pheader->length, U_SPIFFS))
				{
					Update.printError(LastOTAError);
					Serial.println(LastOTAError.c_str());
					ota_status = ota_fail;
					return;
				}
			}
			else
			{
				LastOTAError.printf_P(PSTR("Invalid header signature: 0x%x\r\n"), pheader->partition);
				Serial.println(LastOTAError.c_str());
				ota_status = ota_fail; // invalid header sig
				return;
			}

			Update.setMD5(pheader->get_md5().c_str());
			remaining_length = pheader->length;

			ota_status = ota_content;
		}
		else if(ota_status == ota_content)
		{
			// write content
			uint32_t one_size =
				remaining_length < upload.currentSize ? remaining_length : upload.currentSize;
			if(Update.write(upload.buf, one_size) != one_size)
			{
				Update.printError(LastOTAError);
				Serial.println(LastOTAError.c_str());
				ota_status = ota_fail;
			}
			remaining_length -= one_size;
			if(remaining_length == 0)
			{
				Serial.println(F("\r\nPartition all received"));
				// finish ota partition
				if(Update.end(true)){ //true to set the size to the current progress
					Serial.printf_P(PSTR("\r\nUpdate Success: %u\r\n"), upload.totalSize);
				} else {
					Update.printError(LastOTAError);
					Serial.println(LastOTAError.c_str());
					ota_status = ota_fail;
					return;
				}

				// step to next status
				ota_status = ota_header;
			}
		}

	} else if(upload.status == UPLOAD_FILE_END){
		Serial.setDebugOutput(false);
	}
	yield();
}

void web_server_setup()
{

	server.on("/", HTTP_GET, [](){
	  server.sendHeader("Connection", "close");
	  server.send(200, "text/html", serverIndex);
	});
	server.on("/update", HTTP_POST, [](){
			Serial.println(F("\r\nOTA done.\r\n"));
			server.sendHeader("Connection", "close");
			server.send(200, "text/plain", (ota_status == ota_fail || Update.hasError())?("FAIL:"+LastOTAError).c_str():"OK");
			server.close();
			timer0_detachInterrupt();
			delay(2000);
			ESP.restart();
		}, []() { web_server_ota_upload_handler(); });


  server.begin();
  Serial.println(F("HTTP server started"));

}

void web_server_handle_client()
{
	server.handleClient();
}
