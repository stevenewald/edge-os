#pragma once

#include "nrf_gpio.h"

#include <stdbool.h>
#include <stdint.h>

namespace edge::aidan {

enum INPUT_RESISTOR { PUR, PDR, NONE };

inline bool read_gpio_pin(uint32_t pin_number)
{
    return nrf_gpio_pin_read(pin_number);
}

inline void set_gpio_pin(uint32_t pin_number)
{
    nrf_gpio_pin_set(pin_number);
}

inline void clear_gpio_pin(uint32_t pin_number)
{
    nrf_gpio_pin_clear(pin_number);
}

inline void toggle_gpio_pin(uint32_t pin_number)
{
    nrf_gpio_pin_toggle(pin_number);
}

inline void set_gpio_pin_output(uint32_t pin_number)
{
    nrf_gpio_cfg_output(pin_number);
}

inline void set_gpio_pin_input(uint32_t pin_number, INPUT_RESISTOR input_resistor)
{
    switch (input_resistor) {
        case INPUT_RESISTOR::NONE:
            nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_NOPULL);
            break;
        case INPUT_RESISTOR::PUR:
            nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_PULLUP);
            break;
        case INPUT_RESISTOR::PDR:
            nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_PULLDOWN);
            break;
    }
}

} // namespace edge::aidan
