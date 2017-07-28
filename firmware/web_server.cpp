#include <assert.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <FS.h>
#include "matrix_drive.h"
#include "buttons.h"
#include "settings.h"

extern FS SPIFFS; // main FS

static ESP8266WebServer server(80);
#define serverIndex  F("<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>")

enum ota_status_t { ota_header, ota_content, ota_success, ota_fail };
static ota_status_t ota_status;
static StreamString LastOTAError;

static String password; //!< password in plain text
static const char * user_name = "admin"; //!< default user name

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
		led_set_interval_mode(LIM_PWM_OFF); // stop harmful LED PWM clock which will interfere with WiFi.
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

static bool send_common_header()
{
	if(!server.authenticate(user_name, password.c_str()))
	{
		server.requestAuthentication();
		return false;
	}
	server.sendHeader(F("Connection"), F("close"));
	return true;
}

static bool loadFromFS(String path){
	String dataType = F("text/plain");
	if(path.endsWith("/")) path += F("index.html");

	if(path.endsWith(".htm") || path.endsWith(".html")) dataType = F("text/html");
	else if(path.endsWith(".css")) dataType = F("text/css");
	else if(path.endsWith(".js"))  dataType = F("application/javascript");
	else if(path.endsWith(".png")) dataType = F("image/png");
	else if(path.endsWith(".gif")) dataType = F("image/gif");
	else if(path.endsWith(".jpg")) dataType = F("image/jpeg");
	else if(path.endsWith(".ico")) dataType = F("image/x-icon");
	else if(path.endsWith(".xml")) dataType = F("text/xml");
	else if(path.endsWith(".pdf")) dataType = F("application/pdf");
	else if(path.endsWith(".zip")) dataType = F("application/zip");

	path = String(F("/w")) + path; // all contents must be under "w" directory

	File dataFile = SPIFFS.open(path.c_str(), "r");

	if (!dataFile)
		return false;

	server.streamFile(dataFile, dataType);

	dataFile.close();
	return true;
}


static void handleNotFound()
{
	if(!send_common_header()) return;
	if(loadFromFS(server.uri())) return;
	String message = F("Not Found\n\n");
	message += String(F("URI: "));
	message += server.uri();
	message += String(F("\nMethod: "));
	message += (server.method() == HTTP_GET)?F("GET"):F("POST");
	message += String(F("\nArguments: "));
	message += server.args();
	message += String(F("\n"));
	for (uint8_t i=0; i<server.args(); i++){
		message += String(F(" NAME:"))+server.argName(i) + F("\n VALUE:") + server.arg(i) + F("\n");
	}
	server.send(404, F("text/plain"), message);

}

static void send_json_ok()
{
	server.send(200, F("application/json"), F("{\"result\":\"ok\"}"));
}

static int last_import_error = 0;

void web_server_setup()
{
	password = F("admin"); // initial password

	server.on(F("/update"), HTTP_POST, [](){
			Serial.println(F("\r\nOTA done.\r\n"));
			if(!send_common_header()) return;
			server.send(200, F("text/plain"),
				(ota_status == ota_fail || Update.hasError())?("FAIL:"+LastOTAError).c_str():"OK");
			server.close();
			timer0_detachInterrupt();
			delay(2000);
			ESP.restart();
		}, []() { web_server_ota_upload_handler(); });

	server.on(F("/keys/U"), HTTP_GET, []() {
		if(!send_common_header()) return; button_push(BUTTON_UP);     send_json_ok(); });
	server.on(F("/keys/D"), HTTP_GET, []() {
		if(!send_common_header()) return; button_push(BUTTON_DOWN);   send_json_ok(); });
	server.on(F("/keys/L"), HTTP_GET, []() {
		if(!send_common_header()) return; button_push(BUTTON_LEFT);   send_json_ok(); });
	server.on(F("/keys/R"), HTTP_GET, []() {
		if(!send_common_header()) return; button_push(BUTTON_RIGHT);  send_json_ok(); });
	server.on(F("/keys/O"), HTTP_GET, []() {
		if(!send_common_header()) return; button_push(BUTTON_OK);     send_json_ok(); });
	server.on(F("/keys/C"), HTTP_GET, []() {
		if(!send_common_header()) return; button_push(BUTTON_CANCEL); send_json_ok(); });
	server.onNotFound(handleNotFound);

	server.on(F("/settings/export"), HTTP_GET, [](){
			String filename(F("export.tar"));
			if(!send_common_header()) return;
			server.sendHeader(F("Content-Disposition"),
				F("attachment; filename=\"mazo3_settings.tar\""));
			if(!settings_export(filename, String())) return;
			File dataFile = SPIFFS.open(filename.c_str(), "r");
			if (!dataFile) return;
			server.streamFile(dataFile, F("application/tar"));
			dataFile.close();
		});

	server.on(F("/settings/import"), HTTP_POST, [](){
			Serial.println(F("\r\Import done.\r\n"));
			if(!send_common_header()) return;
			server.send(200, F("text/plain"), last_import_error ? F("Import failed. System will now reboot.") : F("Import done. System will now reboot."));
			server.close();
			timer0_detachInterrupt();
			delay(2000);
			ESP.restart();
		}, []() {
			String filename(F("import.tar"));
			HTTPUpload& upload = server.upload();
			if(upload.status == UPLOAD_FILE_START){
				;
			} else if(upload.status == UPLOAD_FILE_WRITE){
				// upload file in progress
				const uint8_t *p = upload.buf;
				size_t size = upload.currentSize;
				File dataFile = SPIFFS.open(filename.c_str(), "a");
				if(dataFile.size() > MAX_SETTINGS_TAR_SIZE)
				{
					last_import_error = 1;
					return; // max size exceeded
				}
				dataFile.write(p, size);
			} else if(upload.status == UPLOAD_FILE_END){
				if(!settings_import(filename))
				{
					last_import_error = 2;
					return; // import error
				}
			}
		});

	server.begin();
	Serial.println(F("HTTP server started"));

}

void web_server_handle_client()
{
	server.handleClient();
}
