#include "../include/userlib/syscalls.hpp"
#include <stdio.h>

int ipc_part1(void)
{
    using namespace edge::userlib;

    static bool flipped = false;
    static void (*ipc_callback)(int) = [](int value) { flipped = value; };

    subscribe_ipc("LED_DISPLAY", ipc_callback);

    while (1) {
        if (flipped) {
            set_led(4, 0, true);
            yield();
            set_led(4, 0, false);
        }
        else {
            set_led(0, 4, true);
            yield();
            set_led(0, 4, false);
        }
    }
	return 0;
}

