#include "drivers/captouch_controller.hpp"

#include "config.hpp"
#include "microbit_v2.h"
#include "nrf_gpio.h"

namespace edge::drivers {

CapsenseController::CapsenseController() :
    touched(false), prev_touched(false),
    event{
        TOUCH_LOGO, GPIOConfiguration::IN_NORES,
        aidan::GPIOEventController::GPIOEventCallback::create<
            CapsenseController, &CapsenseController::handle_gpio_interrupt>(*this)
    }
{
    touch_logo.clear();
    nrf_gpio_cfg(
        TOUCH_LOGO, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE
    );
};

CapsenseController& CapsenseController::get()
{
    static CapsenseController capsenseController;
    return capsenseController;
}

void CapsenseController::subscribe_captouch_press(
    ProcessCallbackPtr callback, uint8_t process_id
)
{
    subscriptions.set_callback(process_id, callback);
}

bool CapsenseController::get_captouch_pressed()
{
    return touched;
}

void CapsenseController::handle_gpio_interrupt(nrf_gpio_pin_sense_t sense, int pin)
{
    if (sense == NRF_GPIO_PIN_SENSE_LOW) {
        touched = true;
    }
    else if (sense == NRF_GPIO_PIN_SENSE_HIGH) {
        touched = false;
    }
    if ((prev_touched != touched)) {
        for (int process_id = 0; process_id < MAX_PROCESSES; ++process_id) {
            if (subscriptions.has_callback(process_id)) {
                subscriptions.call_callback(
                    process_id, static_cast<int>(to_cap_sense_state(sense))
                );
            }
        }
    }
    prev_touched = touched;
}

} // namespace edge::drivers
