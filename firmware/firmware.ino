#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "../../../../.config/ssid.h"

#include "matrix_drive.h"
#include "buttons.h"
#include "ir_control.h"


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


