#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <FS.h>
#include "buildinfo.h"


#include "matrix_drive.h"
#include "buttons.h"
#include "ir_control.h"
#include "fonts/font_bff.h"
#include "ui.h"
#include "settings.h"
#include "web_server.h"
#include "wifi.h"
#include "calendar.h"
#include "panic.h"
#include "sensors.h"
#include "console.h"

#include "spiffs_api.h"
extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;
extern "C" uint32_t _SPIFFS_page;
extern "C" uint32_t _SPIFFS_block;

#define SPIFFS_PHYS_ADDR ((uint32_t) (&_SPIFFS_start) - 0x40200000)
#define SPIFFS_PHYS_SIZE MY_SPIFFS_SIZE
#define SPIFFS_PHYS_PAGE ((uint32_t) &_SPIFFS_page)
#define SPIFFS_PHYS_BLOCK ((uint32_t) &_SPIFFS_block)

#ifndef SPIFFS_MAX_OPEN_FILES
#define SPIFFS_MAX_OPEN_FILES 5
#endif

FS SPIFFS = FS(FSImplPtr(new SPIFFSImpl(
                             SPIFFS_PHYS_ADDR,
                             SPIFFS_PHYS_SIZE,
                             SPIFFS_PHYS_PAGE,
                             SPIFFS_PHYS_BLOCK,
                             SPIFFS_MAX_OPEN_FILES)));


FS SETTINGS_SPIFFS = FS(FSImplPtr(new SPIFFSImpl(
							SETTINGS_SPIFFS_START,
							SETTINGS_SPIFFS_SIZE,
                            SPIFFS_PHYS_PAGE,
                            SPIFFS_PHYS_BLOCK,
                            1)));



extern "C" {  size_t xPortGetFreeHeapSize(); }


extern uint32_t _SPIFFS_start;

#define WIRE_SDA 0
#define WIRE_SCL 5

void setup(void){

	Serial.begin(115200);
	Serial.print("\r\n\r\nWelcome\r\n");
	Serial.printf_P(PSTR("Built at %s %s\r\n"), _BuildInfo.date, _BuildInfo.time);
	Serial.printf_P(PSTR("Source version : %s\r\n"), _BuildInfo.src_version);
	Serial.printf_P(PSTR("Arduino core version : %s\r\n"), _BuildInfo.env_version);


	Serial.printf_P(PSTR("Pre-initialization of LED matrix driver...\r\n"));
	led_pre_init();


	FSInfo info;

	Serial.printf_P(PSTR("Filesystem initialization...\r\n"));

	if(!SPIFFS.begin())
	{
		Serial.printf_P(PSTR("Main filesystem mount failed ... Now formatting the fs.\r\n"));
		Serial.printf_P(PSTR("Should better enter the recovery mode ...\r\n"));
		SPIFFS.format();
		if(!SPIFFS.begin())
			do_panic(6, F("Main SPIFFS format failed"));
	}
	SPIFFS.info(info);
	Serial.printf("Main SPIFFS\r\n");
	Serial.printf("totalBytes    : %d\r\n", info.totalBytes);
	Serial.printf("usedBytes     : %d\r\n", info.usedBytes);
	Serial.printf("blockSize     : %d\r\n", info.blockSize);
	Serial.printf("pageSize      : %d\r\n", info.pageSize);
	Serial.printf("maxOpenFiles  : %d\r\n", info.maxOpenFiles);
	Serial.printf("maxPathLength : %d\r\n", info.maxPathLength);

	if(!SETTINGS_SPIFFS.begin())
	{
		Serial.printf_P(PSTR("Settings filesystem mount failed ...\r\n"));
		Serial.printf_P(PSTR("Filesystem is now initialized.\r\n"));
		SETTINGS_SPIFFS.format();
		if(!SETTINGS_SPIFFS.begin())
			do_panic(5, F("Settings SPIFFS format failed"));
	}
	SETTINGS_SPIFFS.info(info);
	Serial.printf("Settings SPIFFS\r\n");
	Serial.printf("totalBytes    : %d\r\n", info.totalBytes);
	Serial.printf("usedBytes     : %d\r\n", info.usedBytes);
	Serial.printf("blockSize     : %d\r\n", info.blockSize);
	Serial.printf("pageSize      : %d\r\n", info.pageSize);
	Serial.printf("maxOpenFiles  : %d\r\n", info.maxOpenFiles);
	Serial.printf("maxPathLength : %d\r\n", info.maxPathLength);

	Serial.printf_P(PSTR("Infrared initialization...\r\n"));
	ir_init();

	Serial.printf_P(PSTR("Waiting for serial console request ...\r\n"));
	console_init();

	Serial.printf_P(PSTR("LED matrix driver initialization...\r\n"));
	led_init();

	Serial.printf_P(PSTR("WiFi initialization...\r\n"));
	wifi_setup();

	Serial.printf_P(PSTR("Calendar initialization...\r\n"));
	calendar_init();

 	Serial.printf_P(PSTR("Sensors initialization...\r\n"));
	Wire.begin(WIRE_SDA, WIRE_SCL);
	sensors_init();

 	Serial.printf_P(PSTR("Font initialization...\r\n"));
	font_bff.begin(BFF_FONT_FILE_START_ADDRESS);
/*
 	Serial.printf_P(PSTR("MDNS initialization...\r\n"));
	  if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	  }
*/
 	Serial.printf_P(PSTR("Web server initialization...\r\n"));
	web_server_setup(); // this must be after font initialization

	Serial.printf("Flash size : %d %08x\r\n", ESP.getFlashChipSize(), (uint32_t)_SPIFFS_start);

 	Serial.printf_P(PSTR("UI initialization...\r\n"));
	ui_setup();

	Serial.printf("\r\n""PortGetFreeHeapSize: %d\r\n", xPortGetFreeHeapSize());
}





void test_led_sel_row();

static int init_state = 0;
void loop() 
{
	wifi_check();
	test_led_sel_row();
	button_update();
	sensors_check();
	ui_process();
	web_server_handle_client();

	{
		static ir_status_t last_ir_status = (ir_status_t)-1;
		ir_status_t new_ir_status = ir_get_status();
		if(new_ir_status != last_ir_status)
		{
			last_ir_status = new_ir_status;
			Serial.printf("IR state : %d\r\n", (int)new_ir_status);
		}
	}

	console_process();
}


