#pragma once

#include <nrf_gpio.h>
#include <nrf_gpiote.h>

namespace edge::aidan {
enum class InputResistor {
    PUR = NRF_GPIO_PIN_PULLUP,
    PDR = NRF_GPIO_PIN_PULLDOWN,
    NONE = NRF_GPIO_PIN_NOPULL
};

} // namespace edge::aidan
