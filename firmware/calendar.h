#ifndef CALENDAR_H
#define CALENDAR_H

#include "settings.h"

/* I don't know why this struct is not exported from sntp.c ... 
   This must be the same struct as sntp.c's one. */
extern "C" {
	typedef struct tag_calendar_tm
	{
	  int	tm_sec;
	  int	tm_min;
	  int	tm_hour;
	  int	tm_mday;
	  int	tm_mon;
	  int	tm_year;
	  int	tm_wday;
	  int	tm_yday;
	  int	tm_isdst;
	}  calendar_tm;
}


void calendar_init();
void calendar_get_time(calendar_tm & tm);

static constexpr size_t MAX_NTP_SERVERS = 3;
string_vector calendar_get_ntp_server();
void calendar_set_ntp_server(const string_vector & servers);

int calendar_get_timezone(); // returns +-hhmm representation of the timezone; eg. 900 for UTC+9:00
void calendar_set_timezone(int); 


#endif

