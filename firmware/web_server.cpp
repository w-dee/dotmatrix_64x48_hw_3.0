#include <assert.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <FS.h>
#include "buildinfo.h"

#include "matrix_drive.h"
#include "buttons.h"
#include "settings.h"
#include "calendar.h"
#include "font_bff.h"
#include "ui.h"


extern FS SPIFFS; // main FS
static bool in_recovery = false; // web UI in recovery mode

static ESP8266WebServer server(80);
#define Server_Recovery_Index \
	F("<html><body><h1>Recovery Mode</h1><div>The system is in recovery mode because the filesystem mount has failed or font initialization has failed. Please upload the proper firmware to recover the filesystem and font.</div><div><form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form></div></body></html>")

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
	if(!in_recovery)
	{
		if(!server.authenticate(user_name, password.c_str()))
		{
			server.requestAuthentication();
			return false;
		}
	}
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
	else if(path.endsWith(".txt")) dataType = F("text/plain");

	path = String(F("/w")) + path; // all contents must be under "w" directory
		// TODO: path reverse-traversal check

	if(SPIFFS.exists(path + F(".gz")))
      path += String(F(".gz")); // handle gz

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
	Serial.print(message);
	server.send(404, F("text/plain"), message);

}

static void send_json_ok()
{
	server.send(200, F("application/json"), F("{\"result\":\"ok\"}"));
}

//! write a json-escaped string into the stream
static void string_json(const String &s, Stream & st)
{
	st.print((char)'"'); // starting "
	const char *p = s.c_str();
	while(*p)
	{
		char c = *p;

		if(c < 0x20)
			st.printf_P(PSTR("\\u%04d"), (int)c); // control characters
		else if(c == '\\')
			st.print(F("\\\\"));
		else if(c == '"')
			st.print(F("\\\""));
		else
			st.print(c); // other characters

		++p;
	}
	st.print((char)'"'); // ending "
}

static void web_server_export_json_for_ui(bool js)
{
	StreamString st;
	if(js) st.print(F("window.settings="));
	st.print(F("{\"result\":\"ok\",\"values\":{\n"));

	string_vector v;

	v = calendar_get_ntp_server();
	st.print(F("\"cal_ntp_servers\":[\n"));
	string_json(v[0], st); st.print((char)',');
	string_json(v[1], st); st.print((char)',');
	string_json(v[2], st); st.print((char)']');

	st.print(F(",\n"));
	st.print(F("\"cal_timezone\":"));
	st.printf_P(PSTR("%d"), calendar_get_timezone());

	st.print(F(",\n"));
	st.print(F("\"admin_pass\":"));
	string_json(password, st);

	st.print(F(",\n"));
	st.print(F("\"ui_marquee\":"));
	string_json(ui_get_marquee(), st);

	st.print(F(",\n"));
	st.print(F("\"version_info\":{"));

	st.print(F("\"date\":"));
	string_json(_BuildInfo.date, st);
	st.print(F(",\n"));

	st.print(F("\"time\":"));
	string_json(_BuildInfo.time, st);
	st.print(F(",\n"));

	st.print(F("\"src_version\":"));
	string_json(_BuildInfo.src_version, st);
	st.print(F(",\n"));

	st.print(F("\"env_version\":"));
	string_json(_BuildInfo.env_version, st);

	st.print(F("}\n"));


	st.print(F("}}\n"));
	if(js) st.print((char)';');

	if(js)
		server.send(200, F("application/javascript"), st);
	else
		server.send(200, F("application/json"), st);
}

static void web_server_handle_admin_pass()
{
	if(!send_common_header()) return;
	password = server.arg(F("admin_pass"));
	settings_write(F("web_server_admin_pass"), password);

	send_json_ok();
}


static void web_server_handle_settings_calendar()
{
	if(!send_common_header()) return;
	string_vector ntp_servers{
		server.arg(F("ntp1")),
		server.arg(F("ntp2")),
		server.arg(F("ntp3")) };
	int tz = server.arg(F("tz")).toInt();
	calendar_set_ntp_server(ntp_servers);
	calendar_set_timezone(tz);

	send_json_ok();
}
static void web_server_handle_ui_marquee()
{
	if(!send_common_header()) return;
	String m = server.arg(F("ui_marquee"));
	ui_set_marquee(m);

	send_json_ok();
}

static int last_import_error = 0;

void web_server_setup()
{
	// read settings
	settings_write(F("web_server_admin_pass"), F("admin"), SETTINGS_NO_OVERWRITE);
	settings_read(F("web_server_admin_pass"), password);

	// check the filesystem and font is sane
	if(!SPIFFS.exists(F("/w/index.html.gz"))
		|| !font_bff.get_available()) // this must exist for proper working
	{
		// insane filesystem;
		in_recovery = true;
		server.on(F("/"), HTTP_GET, []() {
			server.send(200, 
			F("text/html"), Server_Recovery_Index); });
	}
	else
	{
		// handle filesystem content
		server.onNotFound(handleNotFound);
	}

	// setup handlers
	server.on(F("/update"), HTTP_POST, [](){
			Serial.println(F("\r\nOTA done.\r\n"));
			if(!send_common_header()) return;
			server.sendHeader(F("Connection"), F("close"));
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

	server.on(F("/settings/settings.json"), HTTP_GET, []() {
			if(!send_common_header()) return;
			web_server_export_json_for_ui(false);
		});
	server.on(F("/settings/settings.js"), HTTP_GET, []() {
			if(!send_common_header()) return;
			web_server_export_json_for_ui(true);
		});

	server.on(F("/settings/admin_pass"), HTTP_POST,
		&web_server_handle_admin_pass);

	server.on(F("/settings/calendar"), HTTP_POST,
		&web_server_handle_settings_calendar);

	server.on(F("/settings/ui_marquee"), HTTP_POST,
		&web_server_handle_ui_marquee);

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
			server.send(200, F("text/plain"),
				last_import_error == 1?
					F("Import failed. Too large file.") :
				last_import_error == 2?
					F("Import failed. System will now reboot.") :
					F("Import done. System will now reboot."));
			server.close();
			timer0_detachInterrupt();
			delay(2000);
			ESP.restart();
		}, []() {
			String filename(F("import.tar"));
			HTTPUpload& upload = server.upload();
			if(upload.status == UPLOAD_FILE_START){
				SPIFFS.remove(filename.c_str());
				last_import_error = 0;
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
				dataFile.close();
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
