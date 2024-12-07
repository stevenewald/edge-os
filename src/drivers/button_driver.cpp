#include "drivers/button_driver.hpp"

#include "drivers/driver_enums.hpp"
#include "scheduler/user_callback_storage.hpp"

namespace edge::drivers {

ButtonController& ButtonController::get()
{
    static ButtonController button_controller;
    return button_controller;
}

ButtonController::ButtonController() :
    event_a{
        BTN_A, GPIOConfiguration::IN_PUR,
        aidan::GPIOEventController::GPIOEventCallback::create<
            ButtonController, &ButtonController::handle_gpio_interrupt>(*this)
    },
    event_b{
        BTN_B, GPIOConfiguration::IN_PUR,
        aidan::GPIOEventController::GPIOEventCallback::create<
            ButtonController, &ButtonController::handle_gpio_interrupt>(*this)
    }
{}

void ButtonController::subscribe_button_press(
    ButtonType type, ProcessCallbackPtr callback, ProcessId process_id
)
{
    if (type == ButtonType::A) {
        a_subscriptions.set_callback(process_id, callback);
    }
    else if (type == ButtonType::B) {
        b_subscriptions.set_callback(process_id, callback);
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

void ButtonController::handle_button_press(ButtonType type, ButtonState state)
{
    UserCallbackStorage& button_subscriptions =
        type == ButtonType::A ? a_subscriptions : b_subscriptions;
    for (int process_id = 0; process_id < MAX_PROCESSES; process_id++) {
        if (button_subscriptions.has_callback(process_id)) {
            button_subscriptions.call_callback(
                process_id, static_cast<int>(type), static_cast<int>(state)
            );
        }
    }
}

void ButtonController::handle_gpio_interrupt(nrf_gpio_pin_sense_t sense, int pin)
{
    ButtonType type = pin == BTN_A ? ButtonType::A : ButtonType::B;
    ButtonState state =
        sense == NRF_GPIO_PIN_SENSE_LOW ? ButtonState::DOWN : ButtonState::UP;
    handle_button_press(type, state);
}
} // namespace edge::drivers
