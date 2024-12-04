#include "drivers/app_timer_controller.hpp"

namespace edge::drivers
{

AppTimerController& AppTimerController::get()
{
    static AppTimerController controller;
    return controller;
}

AppTimerController::AppTimerController()
{
    app_timer_init();
    APP_TIMER_DEF(timer_id);
    app_timer_create(&timer_id, APP_TIMER_MODE_REPEATED, LogHandler);
    app_timer_start(timer_id, APP_TIMER_TICKS(5000), NULL);
    printf("Starting app timer.\n");
}

void AppTimerController::LogHandler(void* p_context)
{
    printf("App Timer hit.\n");
}


} // namespace edge::drivers
