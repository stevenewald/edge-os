#pragma once

#include "nrf_timer.h"
#include "stdint.h"
#include "util.hpp"

namespace edge::drivers {
extern "C" {
void TIMER3_IRQHandler(void);
}

struct timer {
    uint32_t id;

    ProcessCallbackPtr callback;

    uint32_t timer_value;
    ProcessId process_id;
    uint32_t duration;
    bool periodic;
};

inline bool operator<(const timer& first, const timer& second)
{
    return first.timer_value < second.timer_value;
}

// Reserves Timer 3
class VirtualTimerController {
    static constexpr nrf_timer_frequency_t TIMER_FREQUENCY = NRF_TIMER_FREQ_16MHz;
    inline static NRF_TIMER_Type* const TIMER = NRF_TIMER3;
    static constexpr size_t MAX_TIMERS = 512;
    etl::set<timer, MAX_TIMERS> timers_;
    uint32_t virtual_timer_start(const timer& timer);

public:
    uint32_t virtual_timer_start(
        uint32_t microseconds, ProcessCallbackPtr callback, ProcessId timer_creator,
        bool periodic
    );
    void virtual_timer_cancel(uint32_t timer_id);

    uint32_t read_timer() const;

    static VirtualTimerController& get();

    VirtualTimerController(VirtualTimerController&) = delete;
    VirtualTimerController(VirtualTimerController&&) = delete;
    VirtualTimerController& operator=(VirtualTimerController&) = delete;
    VirtualTimerController& operator=(VirtualTimerController&&) = delete;

    friend void TIMER3_IRQHandler(void);

private:
    void trigger_ready_timers();

    etl::optional<timer> get_ready_timer();

    void enqueue_next_timer() const;

    uint32_t timer_start(
        uint32_t microseconds, ProcessCallbackPtr callback, ProcessId timer_creator
    );

    VirtualTimerController();
    ~VirtualTimerController() = default;
};

} // namespace edge::drivers
