#include "util.hpp"

#include <nrf52833.h>
#include <stdio.h>

namespace edge {
void panic(const char* reason)
{
    printf("KERNEL PANIC: %s\n", reason);
    NVIC_SystemReset();
}
} // namespace edge
