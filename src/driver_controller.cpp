#include "drivers/driver_controller.hpp"

#include "drivers/button_type.hpp"
#include "drivers/buttons.hpp"
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
        case DriverType::BUTTONS:
            return button_controller.get_button_pressed(static_cast<ButtonType>(arg1));
    }
    return etl::nullopt;
}
} // namespace edge::drivers
