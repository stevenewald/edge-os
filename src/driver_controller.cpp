#include "drivers/driver_controller.hpp"

namespace edge::drivers {
void DriverController::handle_command(DriverType type, int arg1, int arg2, int arg3)
{
    if (type == DriverType::LED_DISPLAY) {
        display.set_led(arg1, arg2, arg3);
    }
    else {
        printf("NOT SUPPORTED\n");
    }
}

DriverController driver_controller;
} // namespace edge::drivers
