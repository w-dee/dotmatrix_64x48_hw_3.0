#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "matrix_drive.h"
#include "settings.h"
#include "wifi.h"


static String line;
static bool enabled = false;
void console_init()
{
	for(int i = 0; i < 4; i++)
	{
		Serial.printf_P(PSTR("Press enter to enable console ...\r\n"));
		delay(500);
		while(Serial.available() > 0)
		{
			int ch = Serial.read();
			if(ch == '\r' || ch == '\n')
			{
				Serial.printf_P(PSTR("\r\n\r\n=== Console enabled ===\r\n\r\n\r\n"));
				enabled = true;
				led_disable_i2s_output(); // this enables serial input
				while(Serial.available() > 0) Serial.read(); // discard any garbage data
				return;
			}
		}
	}

	// otherwise, I2S will take over serial0's RX
}

static void tokenize(const String &line, string_vector &vec)
{
	vec.clear();
	if(line.length() == 0) return;
	const char *p = line.c_str();
	String one;

	while(*p)
	{
		while(*p && *p <= 0x20) ++p;
		if(!*p) break;

		while(*p && *p > 0x20)
			one += (char) *p, ++p;
		vec.push_back(one);
		one = String();

		if(!*p) break;
	}

	if(one.length() > 0) vec.push_back(one);
}

static void console_usage()
{
	Serial.print(F(
			"\r\nUsage:\r\n"
			"ap <ssid>              - Set WiFi AP name\r\n"
			"psk <psk>              - Set WiFi PSK(i.e. password)\r\n"
			"wps                    - Virtually push WPS \"Push Button\"\r\n"
			"reboot                 - Restart the system\r\n"
			"\r\n"
			"During this console mode, LED matrix will not propery work.\r\n"
			"To go to normal mode, type \"reboot\".\r\n\r\n"
	));
}

static void console_command(const String & line)
{
	string_vector vec;
	tokenize(line, vec);

	if(vec.size() == 0)
	{
		console_usage();
		return;
	}

	if(vec[0] == String(F("ap")))
	{
		if(vec.size() != 2) goto parameter_count_error;
		wifi_set_ap_info(vec[1], wifi_get_ap_pass());
		return;
	}
	else if(vec[0] == String(F("psk")))
	{
		if(vec.size() != 2) goto parameter_count_error;
		wifi_set_ap_info(wifi_get_ap_name(), vec[1]);
		return;
	}
	else if(vec[0] == String(F("wps")))
	{
		if(vec.size() != 1) goto parameter_count_error;
		wifi_wps();
		return;
	}
	else if(vec[0] == String(F("reboot")))
	{
		if(vec.size() != 1) goto parameter_count_error;
		timer0_detachInterrupt();
		ESP.restart();
		delay(200);
		return;
	}
	else
	{
		Serial.printf_P(PSTR("\"%s\" is not a valid command.\r\n"), vec[0].c_str());
		console_usage();
		return;
	}


parameter_count_error:
	Serial.printf_P(PSTR("Invalid parameter count for \"%s\".\r\n"), vec[0].c_str());
	console_usage();
}

void console_process()
{
	if(!enabled) return;

	static bool first = true;
	if(first)
	{
		first = false;
		console_usage();
		Serial.print((char)'>');
		Serial.print((char)' ');
	}

	if(Serial.available() > 0)
	{
		int ch = Serial.read();
		switch(ch)
		{
		case 0x08: // bs
		case 127: // del
			if(line.length() > 0)
			{
				line = line.substring(0, line.length() - 1);
				Serial.print((char)'\x08');
				Serial.print((char)' ');
				Serial.print((char)'\x08');
			}
			break;

		case '\r':
			Serial.print((char)'\r');
			Serial.print((char)'\n');
			console_command(line);
			line = String();
			Serial.print((char)'>');
			Serial.print((char)' ');
			break;

		case '\n':
			break;// discard

		default:
			if(ch >= 0x20)
			{
				line += (char)ch;
				Serial.print((char)ch);
			}
			else
			{
				Serial.printf("[%02x]", ch);
			}
		}
	}
}

