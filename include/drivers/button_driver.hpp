#pragma once

#include "config.hpp"
#include "driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpio_pin_event.hpp"
#include "microbit_v2.h"
#include "scheduler/user_callback_storage.hpp"

namespace edge::drivers {
class ButtonController {
    GPIOPin button_a{BTN_A, GPIOConfiguration::IN_PUR};
    GPIOPin button_b{BTN_B, GPIOConfiguration::IN_PUR};

    void handle_button_press(ButtonType type, ButtonState state);
    void handle_gpio_interrupt(nrf_gpio_pin_sense_t sense, int pin);

    GPIOPinEvent event_a;
    GPIOPinEvent event_b;

    UserCallbackStorage a_subscriptions;
    UserCallbackStorage b_subscriptions;

    ButtonController();
    ~ButtonController() = default;

public:
    ButtonController(const ButtonController&) = delete;
    ButtonController(ButtonController&&) = delete;
    ButtonController& operator=(const ButtonController&) = delete;
    ButtonController& operator=(ButtonController&&) = delete;

    static ButtonController& get();

    bool get_button_pressed(ButtonType button_type);

    void subscribe_button_press(
        ButtonType type, ProcessCallbackPtr callback, ProcessId process_id
    );
};
} // namespace edge::drivers
