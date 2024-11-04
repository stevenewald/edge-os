#include "hil/gpio_wrapper.hpp"

#include "nrf_gpio.h"

#include <stdbool.h>
#include <stdint.h>

namespace edge::aidan {

bool read_gpio_pin(uint32_t pin_number)
{
    return nrf_gpio_pin_read(pin_number);
}

void set_gpio_pin(uint32_t pin_number)
{
    nrf_gpio_pin_set(pin_number);
}

void clear_gpio_pin(uint32_t pin_number)
{
    nrf_gpio_pin_clear(pin_number);
}

void write_gpio_pin(uint32_t pin_number, uint32_t value)
{
    nrf_gpio_pin_write(pin_number, value);
}

void toggle_gpio_pin(uint32_t pin_number)
{
    nrf_gpio_pin_toggle(pin_number);
}

void set_gpio_pin_output(uint32_t pin_number)
{
    nrf_gpio_cfg_output(pin_number);
}

void set_gpio_pin_input(uint32_t pin_number, InputResistor input_resistor)
{
    switch (input_resistor) {
        case InputResistor::NONE:
            nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_NOPULL);
            break;
        case InputResistor::PUR:
            nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_PULLUP);
            break;
        case InputResistor::PDR:
            nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_PULLDOWN);
            break;
    }
}

} // namespace edge::aidan
