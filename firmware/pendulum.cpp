#include <Arduino.h>

#include "pendulum.h"
extern "C" {
#include "eagle_soc.h"
#include "osapi.h"
#include "ets_sys.h"
}

pendulum_t::pendulum_t(pendulum_t::callback_t _callback, uint32_t interval_ms) :
	callback(_callback)
{
	os_timer_setfn(&timer, &callback_fn, reinterpret_cast<void*>(this));
	os_timer_arm(&timer, interval_ms, 1/*repeat*/);
}

pendulum_t::~pendulum_t()
{
	os_timer_disarm(&timer);
}

void pendulum_t::callback_fn(void *arg)
{
	pendulum_t *p = reinterpret_cast<pendulum_t*> (arg);
	p->callback();
}

