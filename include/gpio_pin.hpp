#pragma once

#include "gpio_wrapper.hpp"
#include "stdint.h"

namespace edge {
namespace drivers {

enum GPIOConfiguration { OUT, IN_NORES, IN_PDR, IN_PUR };

class GPIOPin {
public:
    GPIOPin(uint32_t const pin_number, GPIOConfiguration const pin_config) :
        pin_number(pin_number), pin_config(pin_config)
    {
        switch (pin_config) {
            case GPIOConfiguration::OUT:
                aidan::set_gpio_pin_output(pin_number);
                break;
            case GPIOConfiguration::IN_NORES:
                aidan::set_gpio_pin_input(pin_number, aidan::INPUT_RESISTOR::NONE);
                break;
            case GPIOConfiguration::IN_PDR:
                aidan::set_gpio_pin_input(pin_number, aidan::INPUT_RESISTOR::PDR);
                break;
            case GPIOConfiguration::IN_PUR:
                aidan::set_gpio_pin_input(pin_number, aidan::INPUT_RESISTOR::PUR);
                break;
        }
    }

    void toggle() { aidan::toggle_gpio_pin(pin_number); }

    void clear() { aidan::clear_gpio_pin(pin_number); }

    void set() { aidan::set_gpio_pin(pin_number); }

    bool read() { return aidan::read_gpio_pin(pin_number); }

    uint32_t get_pin_number() { return pin_number; }

    GPIOConfiguration get_pin_config() { return pin_config; }

private:
    uint32_t const pin_number;
    GPIOConfiguration const pin_config;
};

} // namespace drivers
} // namespace edge
