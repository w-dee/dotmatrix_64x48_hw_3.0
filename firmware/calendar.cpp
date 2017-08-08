#include <Arduino.h>
#include "calendar.h"
#include "pendulum.h"

extern "C" {
#include "sntp.h"

struct tag_calendar_tm * ICACHE_FLASH_ATTR
sntp_localtime_r(const time_t * tim_p ,
		struct tag_calendar_tm *res);
}

// TODO: The default SNTP implementation lacks sub-second
// precision, Year 2038 problem workaround,
// sub-hour timezone settings like UTC+12:45, daylight time ... etc.
// I'll write another SNTP implementation soon.

static string_vector ntp_servers;
static string_vector shadow_ntp_servers;
	// it seems that lwip's ntp server name is just a pointer, so keeping
	// memory content is responsivility of the caller.
static int timezone; // current timezone
/**
 * set sntp server and tz from ntp_servers
 */
static void calendar_set_ntp_servers_from_vector()
{
	shadow_ntp_servers = ntp_servers; // this will do whole deep copy
	for(size_t i = 0; i < ntp_servers.size(); ++i)
	{
		// I wonder why lwip's sntp_setservername receives
		// <char *>, not <const char *>.
		sntp_setservername(i,
			const_cast<char *>(shadow_ntp_servers[i].c_str()));
	}

	sntp_set_timezone(timezone / 100);

}

void calendar_init()
{
	sntp_init();

	settings_write_vector(F("cal_ntp_servers"), 
		string_vector {
			F("ntp1.jst.mfeed.ad.jp"),
			F("ntp2.jst.mfeed.ad.jp"),
			F("ntp3.jst.mfeed.ad.jp") },
				SETTINGS_NO_OVERWRITE );
	settings_read_vector(F("cal_ntp_servers"), ntp_servers);

	settings_write(F("cal_timezone"), F("900"), SETTINGS_NO_OVERWRITE);

	String s;
	settings_read(F("cal_timezone"), s);
	timezone = s.toInt();

	calendar_set_ntp_servers_from_vector();
}

#if 0
static void calendar_tick()
{
	const char * p = sntp_get_real_time(sntp_get_current_timestamp());
	Serial.printf_P(PSTR("time: %s\r\n"), p);
}

static pendulum_t pendulum(&calendar_tick, 1000);
#endif

void calendar_get_time(calendar_tm & tm)
{
	time_t t = sntp_get_current_timestamp();
	sntp_localtime_r(&t, &tm);
}


string_vector calendar_get_ntp_server() { return ntp_servers; }

void calendar_set_ntp_server(const string_vector & servers)
{
	if(servers.size() > MAX_NTP_SERVERS) return;

	ntp_servers = servers;
	settings_write_vector(F("cal_ntp_servers"), ntp_servers);
	calendar_set_ntp_servers_from_vector();
}

int calendar_get_timezone()
{
	return timezone;
}
void calendar_set_timezone(int tz)
{
	timezone = tz;
	char buf[10];
	sprintf_P(buf, PSTR("%d"), tz);
	settings_write(F("cal_timezone"), buf);

	calendar_set_ntp_servers_from_vector();
}


