#include "nrf_delay.h"
#include "userlib/syscalls.hpp"

#include <cstdint>

void spinlock_task(void)
{
    while (1) {
        nrf_delay_ms(1000);
        edge::userlib::debug_print("SPINLOCK PROGRAM: Still Spinlocked.\n");
    }
}
