#include "drivers/driver_enums.hpp"
#include "hal/gpio_wrapper.hpp"
#include "userlib/syscalls.hpp"

void captouch_task(void)
{
    while (1) {
        if (edge::userlib::get_captouch_pressed()) {
            /* edge::userlib::debug_print("pressed\n"); */
        }
        else {
            /* edge::userlib::debug_print("not pressed"); */
        }
    }
}
