#include "hal/gpio_event_controller.hpp"

#include "hal/hal_enums.hpp"
#include "nrf_gpio.h"

#include <nrf_gpiote.h>

#include <cstdint>

namespace edge::aidan {

// TODO: expose pullup, polarity, etc
void GPIOEventController::set_gpio_callback(
    uint32_t pin, InputResistor pin_resistance, GPIOEventCallback callback
)
{
    callbacks[pin] = callback;
    auto resistance = static_cast<nrf_gpio_pin_pull_t>(pin_resistance);

    // TODO: start high/lwo based on resistance type
    nrf_gpio_cfg_sense_input(pin, resistance, NRF_GPIO_PIN_SENSE_HIGH);
}

void GPIOEventController::clear_gpio_callback(uint32_t pin)
{
    callbacks[pin] = nullptr;
    nrf_gpio_cfg_sense_input(pin, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_NOSENSE);
}

GPIOEventController::GPIOEventController()
{
    NRF_GPIO->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT
                           << GPIO_DETECTMODE_DETECTMODE_Pos;
    NVIC_EnableIRQ(GPIOTE_IRQn);
    NVIC_SetPriority(GPIOTE_IRQn, 0x2);
    nrf_gpiote_int_enable(NRF_GPIOTE_INT_PORT_MASK);
}

GPIOEventController::~GPIOEventController()
{
    nrf_gpiote_int_disable(NRF_GPIOTE_INT_PORT_MASK);
}

void GPIOEventController::handle_gpiote_port_event() const
{
    auto latch = NRF_GPIO->LATCH;

    for (uint32_t pin = 0; pin < GPIO_PINS; pin++) {
        if (!(latch & (1UL << pin))) {
            continue;
        }

        uint32_t pin_state = nrf_gpio_pin_read(pin);
        auto state =
            (pin_state == 0) ? NRF_GPIO_PIN_SENSE_LOW : NRF_GPIO_PIN_SENSE_HIGH;
        auto opposite_state =
            (pin_state != 0) ? NRF_GPIO_PIN_SENSE_LOW : NRF_GPIO_PIN_SENSE_HIGH;
        nrf_gpio_cfg_sense_input(pin, nrf_gpio_pin_pull_get(pin), opposite_state);

        auto callback = GPIOEventController::get().callbacks[pin];
        if (callback != nullptr) {
            callback(state, pin);
        }
    }
    NRF_GPIO->LATCH = latch;
}

extern "C" {
void GPIOTE_IRQHandler()
{
    if (nrf_gpiote_event_is_set(NRF_GPIOTE_EVENTS_PORT)) {
		GPIOEventController::get().handle_gpiote_port_event();
        nrf_gpiote_event_clear(NRF_GPIOTE_EVENTS_PORT);
    }
}
}

} // namespace edge::aidan
