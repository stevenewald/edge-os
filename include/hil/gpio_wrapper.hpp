#pragma once

namespace edge::aidan {

enum INPUT_RESISTOR { PUR, PDR, NONE };

bool read_gpio_pin(uint32_t pin_number);

void set_gpio_pin(uint32_t pin_number);

void clear_gpio_pin(uint32_t pin_number);

void write_gpio_pin(uint32_t pin_number, uint32_t value);

void toggle_gpio_pin(uint32_t pin_number);

void set_gpio_pin_output(uint32_t pin_number);

void set_gpio_pin_input(uint32_t pin_number, INPUT_RESISTOR input_resistor);

} // namespace edge::aidan
