#include "drivers/virtual_timer_controller.hpp"

#include "nrf52833.h"
#include "scheduler/pending_process_callbacks.hpp"

namespace edge::drivers {

extern "C" {
void TIMER3_IRQHandler(void)
{
    VirtualTimerController& vtc = VirtualTimerController::get();
    vtc.TIMER->EVENTS_COMPARE[2] = 0;
    vtc.trigger_ready_timers();
}
}

VirtualTimerController::VirtualTimerController()
{
    TIMER->BITMODE = 0x3;
    TIMER->MODE = 0x0;
    TIMER->PRESCALER = 0x4;
    TIMER->TASKS_START = 1;
    TIMER->INTENSET = 0x40000;
    NVIC_EnableIRQ(TIMER3_IRQn);
}

bool VirtualTimerController::has_ready_timer() const
{
    return !timers_.empty() && timers_.top().timer_value <= read_timer();
};

void VirtualTimerController::trigger_ready_timers()
{
    while (has_ready_timer()) {
        timer t{timers_.top()};
        timers_.pop();
        PendingProcessCallbacks::get().add_ready_callback(
            0, reinterpret_cast<void (*)(int, int)>(t.callback)
        );
    }
}

VirtualTimerController& VirtualTimerController::get()
{
    static VirtualTimerController controller;
    return controller;
}

uint32_t VirtualTimerController::read_timer() const
{
    TIMER->TASKS_CAPTURE[1] = 1;
    return TIMER->CC[1];
}

uint32_t VirtualTimerController::timer_start(uint32_t microseconds, void* cb)
{
    uint32_t curr_time = read_timer();
    timers_.emplace(0, cb, curr_time + microseconds);

    TIMER->CC[2] = timers_.top().timer_value;
    return 0;
}

uint32_t VirtualTimerController::virtual_timer_start(uint32_t microseconds, void* cb)
{
    return timer_start(microseconds, cb);
}

void VirtualTimerController::virtual_timer_cancel(uint32_t timer_id) {}

} // namespace edge::drivers
