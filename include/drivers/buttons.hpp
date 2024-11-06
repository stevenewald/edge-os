#pragma once

#include "config.hpp"
#include "driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpio_pin_event.hpp"
#include "microbit_v2.h"

namespace edge::drivers {
// Without runtime std::function, we can't directly point the gpio callback to
// handle_callback instead, we need a function to call it instead. I'm sure there is a
// better way to do this, though
void handle_gpiote_callback(nrf_gpio_pin_sense_t sense, int channel);

class ButtonController {
public:
    static constexpr uint8_t MAX_READY_CALLBACKS = 10;

private:
    GPIOPin button_a{BTN_A, GPIOConfiguration::IN_PUR};
    GPIOPin button_b{BTN_B, GPIOConfiguration::IN_PUR};

    GPIOPinEvent event_a{
        BTN_A, GPIOConfiguration::IN_PUR, &edge::drivers::handle_gpiote_callback
    };
    GPIOPinEvent event_b{
        BTN_B, GPIOConfiguration::IN_PUR, &edge::drivers::handle_gpiote_callback
    };

    using SubscriptionArray = etl::array<SubscribeCallbackPtr, MAX_PROCESSES>;
    SubscriptionArray a_subscriptions;
    SubscriptionArray b_subscriptions;
    etl::array<etl::vector<subscribe_callback, MAX_READY_CALLBACKS>, MAX_PROCESSES>
        ready_callbacks;

public:
    ButtonController() = default;
    ButtonController& operator=(const ButtonController&) = delete;
    ButtonController& operator=(ButtonController&&) = delete;
    ButtonController(const ButtonController&) = delete;
    ButtonController(ButtonController&&) = delete;
    ~ButtonController() = default;

    bool get_button_pressed(ButtonType button_type);

    void subscribe_button_press(
        ButtonType type, SubscribeCallbackPtr callback, uint8_t process_id
    );

    etl::optional<subscribe_callback> get_ready_callback(uint8_t process_id);

private:
    void handle_callback(ButtonType type, ButtonState state);
    friend void handle_gpiote_callback(nrf_gpio_pin_sense_t sense, int pin);
};

extern ButtonController button_controller;
} // namespace edge::drivers
