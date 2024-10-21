#pragma once

#include "nrf52833.h"
#include "nrf_timer.h"

extern "C" {
void TIMER4_IRQHandler(void);
}

namespace edge {
/**
 * Non static methods have guarantee of pre-initialization
 *
 * ========== RESERVATIONS ==========
 * TIMER[0][0] IS RESERVED FOR get_time()
 * TIMER[0][1] IS RESERVED FOR OVERFLOW DETECTION
 * ==================================
 */
class TimerController {
    static constexpr auto TIMER_PRESCALAR = 0;
    static constexpr auto OVERFLOW_CYCLES = UINT_MAX;
    uint32_t clock_wraps_;

public:
    static TimerController&
    get_instance()
    {
        static TimerController controller;
        return controller;
    }

    uint32_t
    get_time()
    {
        NRF_TIMER4->TASKS_CAPTURE[0] = 1;
        return NRF_TIMER4->CC[0];
    }

private:
    TimerController() { initialize_timers_(); }

    static void
    initialize_timers_()
    {
        // High frequency timer
        NRF_TIMER4->PRESCALER = TIMER_PRESCALAR;
        NRF_TIMER4->MODE = 0;
        NRF_TIMER4->BITMODE = 3;
        NRF_TIMER4->TASKS_START = 1;

        NVIC_EnableIRQ(TIMER4_IRQn);
        NRF_TIMER4->CC[1] = OVERFLOW_CYCLES;
        NRF_TIMER4->INTENSET = NRF_TIMER_INT_COMPARE1_MASK;
    }

    void
    mark_clock_wrapped()
    {
        clock_wraps_++;
    }

    friend void ::TIMER4_IRQHandler(void);
};
} // namespace edge
