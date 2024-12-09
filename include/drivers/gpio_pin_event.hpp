#pragma once
#include "drivers/driver_enums.hpp"
#include "hal/gpio_event_controller.hpp"
#include "hal/hal_enums.hpp"

namespace edge::drivers {
class GPIOPinEvent {
    uint8_t pin;

public:
    GPIOPinEvent(
        uint8_t pin, GPIOConfiguration resistance,
        aidan::GPIOEventController::GPIOEventCallback callback
    ) :
        pin(pin)
    {
        // TODO: add resistance
        aidan::GPIOEventController::get().set_gpio_callback(
            pin, aidan::PinPullMode::NONE, callback
        );
    }

    GPIOPinEvent(const GPIOPinEvent&) = delete;
    GPIOPinEvent(GPIOPinEvent&&) = delete;
    GPIOPinEvent& operator=(const GPIOPinEvent&) = delete;
    GPIOPinEvent& operator=(GPIOPinEvent&&) = delete;

    ~GPIOPinEvent() { aidan::GPIOEventController::get().clear_gpio_callback(pin); }
};
} // namespace edge::drivers
