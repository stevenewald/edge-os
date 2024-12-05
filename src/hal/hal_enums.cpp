#include "hal/hal_enums.hpp"

#include "util.hpp"

namespace edge::aidan {
nrf_gpio_pin_pull_t to_nrf_gpio_pin_pull(PinPullMode mode)
{
    switch (mode) {
        case PinPullMode::PUR:
            return NRF_GPIO_PIN_PULLUP;
        case PinPullMode::PDR:
            return NRF_GPIO_PIN_PULLDOWN;
        case PinPullMode::NONE:
            return NRF_GPIO_PIN_NOPULL;
        default:
            panic("Invalid pull mode");
    }
}

nrf_gpio_pin_sense_t to_nrf_gpio_pin_sense(PinSense sense)
{
    switch (sense) {
        case PinSense::LOW:
            return NRF_GPIO_PIN_SENSE_LOW;
        case PinSense::HIGH:
            return NRF_GPIO_PIN_SENSE_HIGH;
        default:
            panic("Invalid pin sense");
    }
}
} // namespace edge::aidan
