#pragma once

#include "nrf52833.h"
#include "nrf_timer.h"

extern "C" {
void TIMER4_IRQHandler(void);
}

namespace edge {

// Kernel reserves timer 4
class KernelTimerController {
    enum CallbackType : uint8_t { GET_TIME = 0, CLOCK_OVERFLOW = 1 };

    inline static NRF_TIMER_Type* const TIMER = NRF_TIMER4;
    static constexpr nrf_timer_frequency_t TIMER_FREQUENCY = NRF_TIMER_FREQ_16MHz;
    static constexpr auto OVERFLOW_CYCLES = UINT_MAX;

    uint32_t clock_wraps_;

public:
    static KernelTimerController&
    get_instance()
    {
        static KernelTimerController controller;
        return controller;
    }

    uint32_t
    get_time()
    {
        nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_CAPTURE0);
        return nrf_timer_cc_read(TIMER, NRF_TIMER_CC_CHANNEL0);
    }

private:
    KernelTimerController() { initialize_timer_(); }

    static CallbackType
    event_triggered_()
    {
        if (nrf_timer_event_check(TIMER, NRF_TIMER_EVENT_COMPARE0)) [[unlikely]] {
            return GET_TIME;
        }
        else if (nrf_timer_event_check(TIMER, NRF_TIMER_EVENT_COMPARE1)) {
            return CLOCK_OVERFLOW;
        }
        else {
            printf("FATAL: event_triggered called but no event detected");
            NVIC_SystemReset();
        }
    }

    static void
    clear_event_(CallbackType type)
    {
        switch (type) {
            case GET_TIME:
                nrf_timer_event_clear(TIMER, NRF_TIMER_EVENT_COMPARE0);
                break;
            case CLOCK_OVERFLOW:
                nrf_timer_event_clear(TIMER, NRF_TIMER_EVENT_COMPARE1);
                break;
        }
    }

    static void
    initialize_timer_()
    {
        // High frequency timer
        nrf_timer_frequency_set(TIMER, TIMER_FREQUENCY);
        nrf_timer_mode_set(TIMER, NRF_TIMER_MODE_TIMER);
        nrf_timer_bit_width_set(TIMER, NRF_TIMER_BIT_WIDTH_32);
        nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_START);

        NVIC_EnableIRQ(TIMER4_IRQn);
        nrf_timer_cc_write(TIMER, NRF_TIMER_CC_CHANNEL1, OVERFLOW_CYCLES);
        nrf_timer_int_enable(TIMER, NRF_TIMER_INT_COMPARE1_MASK);
    }

    void
    mark_clock_wrapped()
    {
        clock_wraps_++;
    }

    friend void ::TIMER4_IRQHandler(void);
};
} // namespace edge
