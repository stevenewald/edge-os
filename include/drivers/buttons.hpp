#pragma once

#include "config.hpp"
#include "driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpiote_pin.hpp"
#include "microbit_v2.h"

namespace edge::drivers {
// Without runtime std::function, we can't directly point the gpio callback to
// handle_callback instead, we need a function to call it instead. I'm sure there is a
// better way to do this, though
void handle_gpiote_callback(int channel);

class ButtonController {
public:
    static constexpr uint8_t MAX_READY_CALLBACKS = 10;

private:
    GPIOPin button_a{BTN_A, GPIOConfiguration::IN_PUR};
    GPIOPin button_b{BTN_B, GPIOConfiguration::IN_PUR};

    GPIOTEPin int_button_a{
        0, BTN_A, aidan::GPIOTEEventPolarity::HI_TO_LO,
        &edge::drivers::handle_gpiote_callback
    };
    GPIOTEPin int_button_b{
        1, BTN_B, aidan::GPIOTEEventPolarity::HI_TO_LO,
        &edge::drivers::handle_gpiote_callback
    };

    using SubscriptionArray = etl::array<ButtonCallbackPtr, MAX_PROCESSES>;
    etl::array<SubscriptionArray, 2> subscriptions;
    etl::array<
        etl::vector<button_subscribe_callback, MAX_READY_CALLBACKS>, MAX_PROCESSES>
        ready_callbacks;

public:
    bool get_button_pressed(ButtonType button_type);

    void subscribe_button_press(
        ButtonType type, ButtonCallbackPtr callback, uint8_t process_id
    );

    etl::optional<button_subscribe_callback> get_ready_callback(uint8_t process_id);

private:
    void handle_callback(int button_type);
    friend void handle_gpiote_callback(int channel);
};

extern ButtonController button_controller;
} // namespace edge::drivers
