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
} // namespace edge::aidan
