#pragma once

#include "driver_enums.hpp"
#include "hal/gpio_wrapper.hpp"
#include "hal/hal_enums.hpp"
#include "stdint.h"

namespace edge {
namespace drivers {

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
                aidan::set_gpio_pin_input(pin_number, aidan::PinPullMode::NONE);
                break;
            case GPIOConfiguration::IN_PDR:
                aidan::set_gpio_pin_input(pin_number, aidan::PinPullMode::PDR);
                break;
            case GPIOConfiguration::IN_PUR:
                aidan::set_gpio_pin_input(pin_number, aidan::PinPullMode::PUR);
                break;
        }
    }

    void toggle() { aidan::toggle_gpio_pin(pin_number); }

    void clear() { aidan::clear_gpio_pin(pin_number); }

    void set() { aidan::set_gpio_pin(pin_number); }

    void write(uint32_t value) { aidan::write_gpio_pin(pin_number, value); }

    bool read() { return aidan::read_gpio_pin(pin_number); }

    uint32_t get_pin_number() { return pin_number; }

    GPIOConfiguration get_pin_config() { return pin_config; }

private:
    uint32_t const pin_number;
    GPIOConfiguration const pin_config;
};

} // namespace drivers
} // namespace edge
