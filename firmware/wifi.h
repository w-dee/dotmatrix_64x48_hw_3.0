#ifndef WIFI_H__
#define WIFI_H__
#include <ESP8266WiFi.h>
void wifi_setup();
void wifi_check();
void wifi_wps();


void wifi_start();
void wifi_write_settings();

struct ip_addr_settings_t
{
	String ip_addr;
	String ip_gateway;
	String ip_mask;
	String dns1;
	String dns2;

	ip_addr_settings_t();
	void clear();
};

const String & wifi_get_ap_name();
const String & wifi_get_ap_pass();
const ip_addr_settings_t & wifi_get_ip_addr_settings();

void wifi_set_ap_info(const String &_ap_name, const String &_ap_pass);

void wifi_set_ap_info(const String &_ap_name, const String &_ap_pass,
	const ip_addr_settings_t & ip);

void wifi_manual_ip_info(const ip_addr_settings_t & ip);

#endif

