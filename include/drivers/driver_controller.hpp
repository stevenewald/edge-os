#pragma once

#include "drivers/driver_types.hpp"
#include "drivers/led_display.hpp"

#include <stdio.h>

namespace edge::drivers {
class DriverController {
    drivers::LedDisplay display;

public:
    void do_work() { display.display_pixels_once(); }

    void handle_command(DriverType type, int arg1, int arg2, int arg3);
};

extern DriverController driver_controller;
} // namespace edge::drivers
