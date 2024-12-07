#include "userlib/syscalls.hpp"

#include <cstdint>

void spinlock_task(void)
{
    volatile uint64_t i = 0;
    while (1) {
        if (i == 100'000'000) {
            edge::userlib::debug_print("Spinlock Task : Still Spinlocked.\n");
            i = 0;
        }
        i = i + 1;
    }
}
