#include "timer.hpp"

#include "nrf_delay.h"

extern "C" {
void
TIMER4_IRQHandler(void)
{
    printf("Interrupt!\n");
    using edge::KernelTimerController;

    switch (KernelTimerController::event_triggered_()) {
        case KernelTimerController::GET_TIME:
            printf("FATAL: GET_TIME register incorrectly triggered callback\n");
            KernelTimerController::clear_event_(KernelTimerController::GET_TIME);
            break;
        case KernelTimerController::CLOCK_OVERFLOW:
            printf(
                "Clock overflow detected. Count: %lu\n",
                KernelTimerController::get_instance().clock_wraps_
            );
            KernelTimerController::get_instance().mark_clock_wrapped();
            KernelTimerController::clear_event_(KernelTimerController::CLOCK_OVERFLOW);
            break;
    }
}
}
