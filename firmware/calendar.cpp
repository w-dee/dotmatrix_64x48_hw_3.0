#include <Arduino.h>
#include "calendar.h"
#include "pendulum.h"

extern "C" {
#include "sntp.h"
}

// TODO: The default SNTP implementation lacks sub-second
// precision, Year 2038 problem workaround,
// sub-hour timezone settings like UTC+12:45, daylight time ... etc.
// I'll write another SNTP implementation soon.

void calendar_init()
{
	sntp_init();
	sntp_setservername(0, "ntp1.jst.mfeed.ad.jp");
	sntp_setservername(1, "ntp2.jst.mfeed.ad.jp");
	sntp_setservername(2, "ntp3.jst.mfeed.ad.jp");
}


static void calendar_tick()
{
	const char * p = sntp_get_real_time(sntp_get_current_timestamp());
	Serial.printf_P(PSTR("time: %s\r\n"), p);
}

static pendulum_t pendulum(&calendar_tick, 1000);

void calendar_get_time(calendar_tm & tm)
{
	time_t t = sntp_get_current_timestamp();
//	sntp_localtime_r(&t, &tm);
}

static string_vector ntp_servers;

string_vector calendar_get_ntp_server() { return ntp_servers; }

void calendar_set_ntp_server(const string_vector & servers)
{
	if(servers.size() > MAX_NTP_SERVERS) return;

	ntp_servers = servers;
}


