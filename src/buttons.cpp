#include "drivers/buttons.hpp"

#include "drivers/driver_enums.hpp"

namespace edge::drivers {
ButtonController button_controller;

etl::optional<subscribe_callback>
ButtonController::get_ready_callback(uint8_t process_id)
{
    if (ready_callbacks[process_id].empty()) {
        return etl::nullopt;
    }
    subscribe_callback ret = ready_callbacks[process_id].back();
    ready_callbacks[process_id].pop_back();
    return ret;
}

void ButtonController::subscribe_button_press(
    ButtonType type, SubscribeCallbackPtr callback, uint8_t process_id
)
{
    if (type == ButtonType::A) {
        a_subscriptions[process_id] = callback;
    }
    else if (type == ButtonType::B) {
        b_subscriptions[process_id] = callback;
    }
}

bool ButtonController::get_button_pressed(ButtonType button_type)
{
    if (button_type == ButtonType::A) {
        return !button_a.read();
    }
    else {
        return !button_b.read();
    }
}

void ButtonController::handle_callback(ButtonType type, ButtonState state)
{
    SubscriptionArray& button_subscriptions =
        type == ButtonType::A ? a_subscriptions : b_subscriptions;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (button_subscriptions[i] != nullptr) {
            ASSERT(ready_callbacks[i].size() < MAX_READY_CALLBACKS);
            ready_callbacks[i].emplace_back(
                button_subscriptions[i], static_cast<int>(type), static_cast<int>(state)
            );
        }
    }
}

void handle_gpiote_callback(nrf_gpio_pin_sense_t sense, int pin)
{
    ButtonType type = pin == BTN_A ? ButtonType::A : ButtonType::B;
    ButtonState state =
        sense == NRF_GPIO_PIN_SENSE_LOW ? ButtonState::DOWN : ButtonState::UP;
    button_controller.handle_callback(type, state);
}
} // namespace edge::drivers
