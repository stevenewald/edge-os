#include "drivers/timer.hpp"

extern "C" {
void TIMER4_IRQHandler(void)
{
    printf("Interrupt!\n");
    using edge::timer4_controller;

    switch (timer4_controller.event_triggered_()) {
        case timer4_controller.GET_TIME:
            printf("FATAL: GET_TIME register incorrectly triggered callback\n");
            timer4_controller.clear_event_(timer4_controller.GET_TIME);
            break;
        case timer4_controller.CLOCK_OVERFLOW:
            printf(
                "Clock overflow detected. Count: %lu\n", timer4_controller.clock_wraps_
            );
            timer4_controller.mark_clock_wrapped();
            timer4_controller.clear_event_(timer4_controller.CLOCK_OVERFLOW);
            break;
    }
}
}

namespace edge {
Timer4Controller timer4_controller;
}
