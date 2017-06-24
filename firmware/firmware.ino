#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <FS.h>

#include "../../../../.config/ssid.h"

#include "matrix_drive.h"
#include "buttons.h"
#include "ir_control.h"
#include "bme280.h"
#include "fonts/font_bff.h"



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







#define WIRE_SDA 0
#define WIRE_SCL 5

BME280 bme280;

ESP8266WebServer server(80);

const char* serverIndex = "5<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";


extern uint32_t _SPIFFS_start;

void setup(void){

  Serial.begin(115200);
  Serial.print("\r\n\r\nWelcome\r\n");
  WiFi.mode(WIFI_OFF);


  ir_init();
  led_init();
  Wire.begin(WIRE_SDA, WIRE_SCL);


  bme280.begin();
  bme280.setMode(BME280_MODE_NORMAL, BME280_TSB_1000MS, BME280_OSRS_x1, BME280_OSRS_x1, BME280_OSRS_x1, BME280_FILTER_OFF);


		  if (MDNS.begin("esp8266")) {
			Serial.println("MDNS responder started");
		  }



			server.on("/", HTTP_GET, [](){
			  server.sendHeader("Connection", "close");
			  server.send(200, "text/html", serverIndex);
			});
			server.on("/update", HTTP_POST, [](){
			  server.sendHeader("Connection", "close");
			  server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");

			timer0_detachInterrupt();
			  ESP.restart();


			},[](){
			  HTTPUpload& upload = server.upload();
			  if(upload.status == UPLOAD_FILE_START){
				Serial.setDebugOutput(true);
				WiFiUDP::stopAll();
				Serial.printf("Update: %s\n", upload.filename.c_str());
				uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
				if(!Update.begin(maxSketchSpace)){//start with max available size
				  Update.printError(Serial);
				}
			  } else if(upload.status == UPLOAD_FILE_WRITE){
				if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
				  Update.printError(Serial);
				}
			  } else if(upload.status == UPLOAD_FILE_END){
				if(Update.end(true)){ //true to set the size to the current progress
				  Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
				} else {
				  Update.printError(Serial);
				}
				Serial.setDebugOutput(false);
			  }
			  yield();
			});


	  server.begin();
	  Serial.println("HTTP server started");

	Serial.printf("Flash size : %d %08x\r\n", ESP.getFlashChipSize(), (uint32_t)_SPIFFS_start);

	Serial.printf("%08x\r\n", *reinterpret_cast<uint32_t*>(1024*1024 + 0x40200000));

		FSInfo info;
		SPIFFS.begin();
		SPIFFS.info(info);
		Serial.printf("totalBytes    : %d\r\n", info.totalBytes);
		Serial.printf("usedBytes     : %d\r\n", info.usedBytes);
		Serial.printf("blockSize     : %d\r\n", info.blockSize);
		Serial.printf("pageSize      : %d\r\n", info.pageSize);
		Serial.printf("maxOpenFiles  : %d\r\n", info.maxOpenFiles);
		Serial.printf("maxPathLength : %d\r\n", info.maxPathLength);


		Serial.printf("begin font init\r\n");
		font_bff.begin(BFF_FONT_FILE_START_ADDRESS);
		Serial.printf("end font init\r\n");

}





void test_led_sel_row();

static int init_state = 0;
void loop() 
{
	test_led_sel_row();
	button_update();


	if(init_state == 0 && millis() > 16000)
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
		  Serial.print("Connected to ");
		  Serial.println(ssid);
		  Serial.print("IP address: ");
		  Serial.println(WiFi.localIP());


		}
	}


	if(init_state == 2)
	{
		server.handleClient();
	}

	if(buttons[BUTTON_OK])
	{
		Serial.println("BUTTON_OK");
		buttons[BUTTON_OK] = 0;
		ir_record();
	}

	if(buttons[BUTTON_CANCEL])
	{
		Serial.println("BUTTON_CANCEL");
		buttons[BUTTON_CANCEL] = 0;
		ir_replay();
	}

	if(buttons[BUTTON_UP])
	{
		Serial.println("BUTTON_UP");
		buttons[BUTTON_UP] = 0;
	  double temperature, humidity, pressure;
	  uint8_t measuring, im_update;
	  char s[64];
	  bme280.getData(&temperature, &humidity, &pressure);
	int temp = temperature * 10;
	int hum = humidity;
	int pre = pressure;
	 
	  sprintf(s, "Temperature: %d.%d C, Humidity: %d %%, Pressure: %d hPa\r\n",
		      temp / 10, temp%10, hum, pre);
	  Serial.print(s);

	}

	if(buttons[BUTTON_DOWN])
	{
		Serial.println("BUTTON_DOWN");
		buttons[BUTTON_DOWN] = 0;
		int n = analogRead(0);
		Serial.printf("ambient : %d\r\n", n);
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


