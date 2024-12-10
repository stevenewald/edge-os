#include "drivers/driver_enums.hpp"
#include "hal/gpio_wrapper.hpp"
#include "nrf_delay.h"
#include "userlib/syscalls.hpp"

void callback(edge::drivers::ButtonState a)
{
    if (a == edge::drivers::ButtonState::UP) {
        edge::userlib::debug_print("CAPTOUCH PROGRAM: Finger lifted.\n");
    }
    else {
        edge::userlib::debug_print("CAPTOUCH PROGRAM: Finger pressed.\n");
    }
}

void captouch_task(void)
{
    edge::userlib::subscribe_captouch_pressed(callback);
    while (1) {
        if (edge::userlib::get_captouch_pressed()) {
            // edge::userlib::debug_print("pressed\n");
        }
        else {
            // edge::userlib::debug_print("not pressed\n");
        }
        edge::userlib::yield();
    }
}
