#include "drivers/driver_controller.hpp"

#include "drivers/led_display.hpp"
#include "drivers/timer.hpp"

#include <stdio.h>

namespace edge::drivers {
// Runs on context switch
void do_async_work()
{
    led_display.display_pixels_once();
}

etl::optional<int> handle_command(DriverType type, int arg1, int arg2, int arg3)
{
    switch (type) {
        case DriverType::LED_DISPLAY:
            led_display.set_led(arg1, arg2, arg3);
            break;
        case DriverType::GET_TIME:
            return timer4_controller.get_time_us();
        case DriverType::TERMINAL_OUTPUT:
            printf((char*)arg1);
            break;
    }
    return etl::nullopt;
}
} // namespace edge::drivers
