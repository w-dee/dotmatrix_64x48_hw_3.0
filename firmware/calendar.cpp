#include <Arduino.h>
#include "calendar.h"
#include "pendulum.h"

extern "C" {
#include "sntp.h"
}

// TODO: The default SNTP implementation lacks sub-second
// precision, Year 2038 problem workaround, ... etc.
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


