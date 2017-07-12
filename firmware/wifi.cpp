#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "matrix_drive.h"
#include SSID_H


static int8_t wifi_emi_reduction_mode = -1; // mode number or -1 = automatic


/*

	
*/

void wifi_setup()
{
	// first, disconnect wifi
	WiFi.mode(WIFI_OFF);
	WiFi.setPhyMode(WIFI_PHY_MODE_11N);
	WiFi.setAutoReconnect(true);

	// try to connect
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
}

extern "C" {
uint8 wifi_get_channel(void);
}




void wifi_check()
{
	// check wifi status


	static int last_chann = -1;
	int now_chann = wifi_get_channel();
	if(last_chann != now_chann)
	{
		last_chann = now_chann;

		if(wifi_emi_reduction_mode == -1)
			led_set_interval_mode_from_channel(now_chann);
		else
			led_set_interval_mode(wifi_emi_reduction_mode);

		Serial.printf_P(PSTR("WiFi channel changed: %d\r\n"), now_chann);
	}


	{
		static uint32_t next = millis() + 1000;
		if(millis() >= next)
		{

			Serial.printf("status:%d ip:%s\r\n", (int)WiFi.status(), String(WiFi.localIP()).c_str());
			Serial.printf("ssid:%s psk:%s\r\n", WiFi.SSID().c_str(), WiFi.psk().c_str());
			next = millis() + 1000;
		}
	}


}


/**
 * Begins WPS configuration.
 * This function does not return until WPS config ends (either successfully or not)
 */
void wifi_wps()
{
	led_stop_pwm_clock(); // stop PWM clock to reduce EMI during WPS
	WiFi.beginWPSConfig();
	led_start_pwm_clock();
}


