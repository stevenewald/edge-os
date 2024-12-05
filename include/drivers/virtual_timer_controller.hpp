#pragma once

#include "nrf_timer.h"
#include "stdint.h"

namespace edge::drivers {
extern "C" {
void TIMER3_IRQHandler(void);
}

struct timer {
    uint32_t id;

    void* callback;

    uint32_t timer_value;

    bool operator<(const timer& other) const { return timer_value < other.timer_value; }
};

// Reserves Timer 3
class VirtualTimerController {
    static constexpr nrf_timer_frequency_t TIMER_FREQUENCY = NRF_TIMER_FREQ_16MHz;
    inline static NRF_TIMER_Type* const TIMER = NRF_TIMER3;
    etl::priority_queue<timer, 32> timers_;

    void trigger_ready_timers();
    bool has_ready_timer() const;

public:
    uint32_t read_timer() const;

    uint32_t virtual_timer_start(uint32_t microseconds, void* callback);

    void virtual_timer_cancel(uint32_t timer_id);

    static VirtualTimerController& get();

    uint32_t timer_start(uint32_t microseconds, void* callback);

    VirtualTimerController(VirtualTimerController&) = delete;
    VirtualTimerController(VirtualTimerController&&) = delete;
    VirtualTimerController& operator=(VirtualTimerController&) = delete;
    VirtualTimerController& operator=(VirtualTimerController&&) = delete;

    friend void TIMER3_IRQHandler(void);

private:
    ~VirtualTimerController() = default;
    VirtualTimerController();
};

} // namespace edge::drivers
