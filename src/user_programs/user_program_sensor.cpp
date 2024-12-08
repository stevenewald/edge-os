#include "drivers/driver_enums.hpp"
#include "userlib/syscalls.hpp"

#include <cstdio>

static void button_callback(edge::drivers::ButtonType type, edge::drivers::ButtonState state)
{
    edge::userlib::debug_print("button A pressed.\n");
}


void sensor_task(void)
{
    edge::userlib::get_button_pressed(edge::drivers::ButtonType::A, button_callback);
    while (1) {
        edge::userlib::yield();
    }
}
