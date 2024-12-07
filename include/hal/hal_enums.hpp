#pragma once

#include <nrf_gpio.h>

namespace edge::aidan {
enum class PinPullMode {
    PUR = NRF_GPIO_PIN_PULLUP,
    PDR = NRF_GPIO_PIN_PULLDOWN,
    NONE = NRF_GPIO_PIN_NOPULL
};

enum class PinSense { HIGH, LOW };

nrf_gpio_pin_pull_t to_nrf_gpio_pin_pull(PinPullMode mode);
nrf_gpio_pin_sense_t to_nrf_gpio_pin_sense(PinSense sense);

} // namespace edge::aidan
