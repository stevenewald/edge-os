#include "timer.hpp"

extern "C" {
void
TIMER4_IRQHandler(void)
{
    edge::TimerController::get_instance().mark_clock_wrapped();
    NRF_TIMER4->EVENTS_COMPARE[1] = 0;
}
}
