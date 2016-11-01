#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

#include "../../../../.config/ssid.h"

#include "matrix_drive.h"
#include "buttons.h"
#include "ir_control.h"
#include "bme280.h"

#define WIRE_SDA 0
#define WIRE_SCL 5

BME280 bme280;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  Serial.begin(115200);
  Serial.print("\r\n\r\nWelcome\r\n");
  ir_init();
  led_init();
  Wire.begin(WIRE_SDA, WIRE_SCL);


  bme280.begin();
  bme280.setMode(BME280_MODE_NORMAL, BME280_TSB_1000MS, BME280_OSRS_x1, BME280_OSRS_x1, BME280_OSRS_x1, BME280_FILTER_OFF);



  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}





void test_led_sel_row();

void loop() 
{
	test_led_sel_row();
	button_update();
	server.handleClient();

	if(buttons[BUTTON_OK])
	{
		buttons[BUTTON_OK] = 0;
		ir_record();
	}

	if(buttons[BUTTON_CANCEL])
	{
		buttons[BUTTON_CANCEL] = 0;
		ir_replay();
	}

	if(buttons[BUTTON_UP])
	{
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


