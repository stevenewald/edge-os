#include "userlib/syscalls.hpp"

#include <cstdio>

void ending_task(void)
{
    using namespace edge::userlib;
    volatile int i = 0;
    bool enabled = true;
    while (i < 5'000'000) {
        if (i % 1'000'000 == 0) {
            edge::userlib::set_led(0, 0, enabled);
            enabled = !enabled;
        }
        i = i + 1;
    }
    debug_print("Ending Task: Done Running. 0,0 should stop changing.\n");
}
