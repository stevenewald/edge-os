#pragma once

#include "app_timer.h"

namespace edge::drivers {

class AppTimerController
{
public:
    static AppTimerController& get();

// private:

	void (*callback_)(int,int);
	app_timer_id_t penis;
	void subscribe(void (*callback)(int,int));
	static void fuckit();

    AppTimerController();

    static void LogHandler(void* p_context);

};


} // namespace edge::drivers
