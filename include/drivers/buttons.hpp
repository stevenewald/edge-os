#pragma once

#include "config.hpp"
#include "driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpiote_pin.hpp"
#include "microbit_v2.h"

namespace edge::drivers {
void handle_callback();

class ButtonController {
    GPIOPin button_a{BTN_A, GPIOConfiguration::IN_PUR};
    GPIOPin button_b{BTN_B, GPIOConfiguration::IN_PUR};

    GPIOTEPin int_button_a{
        0, BTN_A, aidan::GPIOTEEventPolarity::HI_TO_LO, &edge::drivers::handle_callback
    };

    etl::array<void*, MAX_PROCESSES> callbacks{nullptr};
    etl::array<bool, MAX_PROCESSES> callback_ready{false};

public:
    void handle_callback()
    {
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (callbacks[i] != nullptr) {
                callback_ready[i] = true;
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

    void subscribe_button_press(ButtonType type, void* callback, uint8_t process_id)
    {
        callbacks[process_id] = callback;
    }

    etl::optional<void*> get_ready_callback(uint8_t process_id)
    {
        if (callbacks[process_id] == nullptr || !callback_ready[process_id])
            return etl::nullopt;
        etl::optional<void*> opt = callbacks[process_id];
        callbacks[process_id] = nullptr;
        callback_ready[process_id] = false;
        return opt;
    }
};

extern ButtonController button_controller;
} // namespace edge::drivers
