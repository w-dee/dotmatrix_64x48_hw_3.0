#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <FS.h>


#include SSID_H

#include "matrix_drive.h"
#include "buttons.h"
#include "ir_control.h"
#include "bme280.h"
#include "fonts/font_bff.h"
#include "ui.h"
#include "settings.h"
#include "web_server.h"

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

#define WIRE_SDA 0
#define WIRE_SCL 5

BME280 bme280;



extern uint32_t _SPIFFS_start;

void setup(void){

  Serial.begin(115200);
  Serial.print("\r\n\r\nWelcome\r\n");
  WiFi.mode(WIFI_OFF);
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);

  ir_init();
  led_init();
  Wire.begin(WIRE_SDA, WIRE_SCL);


  bme280.begin();
  bme280.setMode(BME280_MODE_NORMAL, BME280_TSB_1000MS, BME280_OSRS_x1, BME280_OSRS_x1, BME280_OSRS_x1, BME280_FILTER_OFF);


	  if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	  }

	web_server_setup();

	Serial.printf("Flash size : %d %08x\r\n", ESP.getFlashChipSize(), (uint32_t)_SPIFFS_start);

	Serial.printf("%08x\r\n", *reinterpret_cast<uint32_t*>(BFF_FONT_FILE_START_ADDRESS + 0x40200000));

	FSInfo info;

	SPIFFS.begin();
	SPIFFS.info(info);
	Serial.printf("Main SPIFFS\r\n");
	Serial.printf("totalBytes    : %d\r\n", info.totalBytes);
	Serial.printf("usedBytes     : %d\r\n", info.usedBytes);
	Serial.printf("blockSize     : %d\r\n", info.blockSize);
	Serial.printf("pageSize      : %d\r\n", info.pageSize);
	Serial.printf("maxOpenFiles  : %d\r\n", info.maxOpenFiles);
	Serial.printf("maxPathLength : %d\r\n", info.maxPathLength);

	SETTINGS_SPIFFS.begin();
	SETTINGS_SPIFFS.info(info);
	Serial.printf("Settings SPIFFS\r\n");
	Serial.printf("totalBytes    : %d\r\n", info.totalBytes);
	Serial.printf("usedBytes     : %d\r\n", info.usedBytes);
	Serial.printf("blockSize     : %d\r\n", info.blockSize);
	Serial.printf("pageSize      : %d\r\n", info.pageSize);
	Serial.printf("maxOpenFiles  : %d\r\n", info.maxOpenFiles);
	Serial.printf("maxPathLength : %d\r\n", info.maxPathLength);

	Serial.printf("\r\n""PortGetFreeHeapSize: %d\r\n", xPortGetFreeHeapSize());

	Serial.printf("begin font init\r\n");
	font_bff.begin(BFF_FONT_FILE_START_ADDRESS);
	Serial.printf("end font init\r\n");

	ui_setup();


//	settings_write(F("test"), F("test_content"));
	String r;
	settings_read(F("test"), r);
	Serial.printf("read: \"%s\"\r\n", r.c_str());

}





void test_led_sel_row();
extern "C" {
uint8 wifi_get_channel(void);
}

static int init_state = 0;
void loop() 
{

	static int last_chann = -1;
	int now_chann = wifi_get_channel();
	if(last_chann != now_chann)
	{
		last_chann = now_chann;
		Serial.printf_P(PSTR("WiFi channel changed: %d\r\n"), now_chann);
	}

	test_led_sel_row();
	button_update();

	ui_process();

	if(init_state == 0 && millis() > 4000)
	{
		init_state = 1;
		Serial.println("connecting ...\r\n"); 
		WiFi.mode(WIFI_OFF);
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);
	}


	if(init_state == 1)
	{
//		Serial.printf("%d\r\n", (int)WiFi.status());
		if(WiFi.status() == WL_CONNECTED)
		{
			init_state = 2;

		  Serial.println("");
		  Serial.print("N Connected to ");
		  Serial.println(ssid);
		  Serial.print("IP address: ");
		  Serial.println(WiFi.localIP());


		}
	}


	if(init_state == 2)
	{
		web_server_handle_client();
	}
	{
		static ir_status_t last_ir_status = (ir_status_t)-1;
		ir_status_t new_ir_status = ir_get_status();
		if(new_ir_status != last_ir_status)
		{
			last_ir_status = new_ir_status;
			Serial.printf("IR state : %d\r\n", (int)new_ir_status);
		}
	}
}


