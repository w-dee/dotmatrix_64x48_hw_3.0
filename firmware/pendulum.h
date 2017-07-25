#ifndef PENDULUM_H_
#define PENDULUM_H_

#include <functional>

//! Class to call specified function by the specified interval, std::function way.
class pendulum_t
{
public:
	typedef std::function<void ()> callback_t;

protected:
	struct _ETSTIMER_ timer;
	callback_t callback;

public:
	pendulum_t(callback_t _callback, uint32_t interval_ms);
	~pendulum_t();

private:
	static void callback_fn(void *arg);
};




#endif
