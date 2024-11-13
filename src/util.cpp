#include "util.hpp"

#include "nrf_delay.h"

#include <nrf52833.h>
#include <stdio.h>

namespace edge {
void panic(const char* reason)
{
    printf("KERNEL PANIC: %s\n", reason);
    nrf_delay_ms(2000);
    NVIC_SystemReset();
}
} // namespace edge
