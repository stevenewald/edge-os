#include "drivers/buttons.hpp"

namespace edge::drivers {
ButtonController button_controller;

etl::optional<button_subscribe_callback>
ButtonController::get_ready_callback(uint8_t process_id)
{
    if (ready_callbacks[process_id].empty()) {
        return etl::nullopt;
    }
    button_subscribe_callback ret = ready_callbacks[process_id].back();
    ready_callbacks[process_id].pop_back();
    return ret;
}

void ButtonController::subscribe_button_press(
    ButtonType type, ButtonCallbackPtr callback, uint8_t process_id
)
{
    subscriptions[static_cast<unsigned>(type)][process_id] = callback;
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

void ButtonController::handle_callback(int button_type)
{
    SubscriptionArray& button_subscriptions = subscriptions[button_type];
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (button_subscriptions[i] != nullptr) {
            if (ready_callbacks[i].size() == MAX_READY_CALLBACKS) {
                printf("WARNING: Ready callbacks is at capacity for process %d\n", i);
                continue;
            }
            ready_callbacks[i].emplace_back(
                button_subscriptions[i], static_cast<ButtonType>(button_type)
            );
        }
    }
}

void handle_gpiote_callback(int channel)
{
    button_controller.handle_callback(channel);
}
} // namespace edge::drivers
