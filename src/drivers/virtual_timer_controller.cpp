#include "drivers/virtual_timer_controller.hpp"

#include "nrf52833.h"
#include "scheduler/pending_process_callbacks.hpp"
#include "util.hpp"

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

etl::optional<timer> VirtualTimerController::get_ready_timer()
{
    auto begin = timers_.begin();
    if (!timers_.empty() && begin->timer_value <= read_timer()) {
        auto ret = etl::make_optional<timer>(*begin);
        timers_.erase(begin);
        return ret;
    }
    return etl::nullopt;
};

void VirtualTimerController::enqueue_next_timer() const
{
    if (!timers_.empty())
        TIMER->CC[2] = timers_.begin()->timer_value;
}

void VirtualTimerController::trigger_ready_timers()
{
    etl::optional<timer> ready_timer_opt = get_ready_timer();
    while (ready_timer_opt.has_value()) {
        timer& ready_timer = ready_timer_opt.value();
        if (etl::holds_alternative<KernelCallbackPtr>(ready_timer.callback)) {
            etl::get<KernelCallbackPtr>(ready_timer.callback)();
        }
        else {
            PendingProcessCallbacks::get().add_ready_callback(
                ready_timer.process_id,
                etl::get<ProcessCallbackPtr>(ready_timer.callback), ready_timer.id
            );
        }
        if (ready_timer.periodic) {
            ready_timer.timer_value += ready_timer.duration;
            virtual_timer_start(ready_timer);
        }
        ready_timer_opt = get_ready_timer();
    }
    enqueue_next_timer();
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

uint32_t VirtualTimerController::virtual_timer_start(const timer& timer)
{
    timers_.insert(timer);
    enqueue_next_timer();
    return timer.id;
}

uint32_t VirtualTimerController::virtual_timer_start(
    uint32_t microseconds, etl::variant<KernelCallbackPtr, ProcessCallbackPtr> cb,
    ProcessId timer_creator, bool periodic
)
{
    static uint32_t timer_offset = 0;
    ++timer_offset;

    uint32_t curr_time = read_timer();
    uint32_t timer_id = curr_time + timer_offset;

    timer new_timer{timer_id,      cb,           curr_time + microseconds,
                    timer_creator, microseconds, periodic};

    return virtual_timer_start(new_timer);
}

void VirtualTimerController::virtual_timer_cancel(uint32_t timer_id)
{
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if (it->id == timer_id) {
            timers_.erase(it);
            return;
        }
    }
    enqueue_next_timer();
}

} // namespace edge::drivers
