#pragma once

#include "drivers/button_type.hpp"
#include "drivers/gpio_pin.hpp"
#include "microbit_v2.h"

namespace edge::drivers {
class ButtonController {
    GPIOPin button_a{BTN_A, IN_PUR};
    GPIOPin button_b{BTN_B, IN_PUR};

public:
    bool get_button_pressed(ButtonType button_type)
    {
        if (button_type == ButtonType::A) {
            return button_a.read();
        }
        else {
            return button_b.read();
        }
    }
};

extern ButtonController button_controller;
} // namespace edge::drivers
