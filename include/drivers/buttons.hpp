#pragma once

#include "config.hpp"
#include "driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpiote_pin.hpp"
#include "microbit_v2.h"

namespace edge::drivers {
void handle_callback(int channel);

class ButtonController {
    GPIOPin button_a{BTN_A, GPIOConfiguration::IN_PUR};
    GPIOPin button_b{BTN_B, GPIOConfiguration::IN_PUR};

    GPIOTEPin int_button_a{
        0, BTN_A, aidan::GPIOTEEventPolarity::HI_TO_LO, &edge::drivers::handle_callback
    };
    GPIOTEPin int_button_b{
        1, BTN_B, aidan::GPIOTEEventPolarity::HI_TO_LO, &edge::drivers::handle_callback
    };

    using SubscriptionArray = etl::array<ButtonCallbackPtr, MAX_PROCESSES>;
    etl::array<SubscriptionArray, 2> subscriptions;
    etl::array<etl::vector<button_subscribe_callback, 10>, MAX_PROCESSES>
        ready_callbacks;

public:
    void handle_callback(int button_type)
    {
        auto& button_subscriptions = subscriptions[button_type];
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (button_subscriptions[i]) {
                ready_callbacks[i].emplace_back(
                    button_subscriptions[i], static_cast<ButtonType>(button_type)
                );
            }
        }
    }

    bool get_button_pressed(ButtonType button_type)
    {
        if (button_type == ButtonType::A) {
            return !button_a.read();
        }
        else {
            return !button_b.read();
        }
    }

    void subscribe_button_press(
        ButtonType type, ButtonCallbackPtr callback, uint8_t process_id
    )
    {
        subscriptions[static_cast<unsigned>(type)][process_id] = callback;
    }

    etl::optional<button_subscribe_callback> get_ready_callback(uint8_t process_id)
    {
        if (ready_callbacks[process_id].empty()) {
            return etl::nullopt;
        }
        button_subscribe_callback ret = ready_callbacks[process_id].back();
        ready_callbacks[process_id].pop_back();
        return ret;
    }
};

extern ButtonController button_controller;
} // namespace edge::drivers
