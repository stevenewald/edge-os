#include "drivers/app_timer_controller.hpp"
#include "scheduler/pending_process_callbacks.hpp"

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
	penis = timer_id;
    printf("Starting app timer.\n");
}

void AppTimerController::fuckit() {

	PendingProcessCallbacks::get().add_ready_callback(0, AppTimerController::get().callback_);
}

void AppTimerController::subscribe(void (*callback)(int,int)) {
	callback_ = callback;
    app_timer_create(&penis, APP_TIMER_MODE_REPEATED, reinterpret_cast<void(*)(void*)>(fuckit));
    app_timer_start(penis, APP_TIMER_TICKS(2), NULL);
}

void AppTimerController::LogHandler(void* p_context)
{
    printf("App Timer hit.\n");
}


} // namespace edge::drivers
