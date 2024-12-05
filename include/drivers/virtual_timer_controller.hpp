#pragma once

#include "nrf_timer.h"
#include "stdint.h"

extern "C" {
void TIMER3_IRQHandler(void);
}

namespace edge::drivers {

// Reserves Timer 3
class VirtualTimerController {
    static constexpr nrf_timer_frequency_t TIMER_FREQUENCY = NRF_TIMER_FREQ_16MHz;

public:
    uint32_t read_timer();

    uint32_t virtual_timer_start(uint32_t microseconds, void* callback);

    void virtual_timer_cancel(uint32_t timer_id);

    static VirtualTimerController& get();

    ~VirtualTimerController() = default;

    inline static NRF_TIMER_Type* const TIMER = NRF_TIMER3;

    uint32_t timer_start(uint32_t microseconds, void* callback);

    void update_ll();

    friend void ::TIMER3_IRQHandler(void);

private:
    VirtualTimerController();
};

} // namespace edge::drivers
