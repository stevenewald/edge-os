#pragma once

#include "app_timer.h"

namespace edge::drivers {

class AppTimerController
{
public:
    static AppTimerController& get();

private:

    AppTimerController();

    static void LogHandler(void* p_context);

};


} // namespace edge::drivers
