#pragma once

#include "hal/hal_enums.hpp"

namespace edge::aidan {

bool read_gpio_pin(uint32_t pin_number);

void set_gpio_pin(uint32_t pin_number);

void clear_gpio_pin(uint32_t pin_number);

void write_gpio_pin(uint32_t pin_number, uint32_t value);

void toggle_gpio_pin(uint32_t pin_number);

void set_gpio_pin_output(uint32_t pin_number);

void set_gpio_pin_input(uint32_t pin_number, InputResistor input_resistor);

} // namespace edge::aidan
