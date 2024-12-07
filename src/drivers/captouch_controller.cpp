#include "drivers/captouch_controller.hpp"

#include "drivers/driver_enums.hpp"
#include "drivers/gpio_pin_event.hpp"
#include "hal/gpio_wrapper.hpp"
#include "hal/hal_enums.hpp"
#include "microbit_v2.h"
#include "nrf_gpio.h"
#include "userlib/syscalls.hpp"

namespace edge::drivers {

CapsenseController::CapsenseController() :
    touched(false),
    event{
        TOUCH_LOGO, GPIOConfiguration::IN_NORES,
        aidan::GPIOEventController::GPIOEventCallback::create<
            CapsenseController, &CapsenseController::handle_gpio_interrupt>(*this)
    }
{
    aidan::clear_gpio_pin(TOUCH_LOGO);
    nrf_gpio_cfg(
        TOUCH_LOGO, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
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
    subscriptions[process_id] = callback;
    return;
}

bool CapsenseController::get_captouch_pressed()
{
    return touched;
}

void CapsenseController::handle_gpio_interrupt(nrf_gpio_pin_sense_t sense, int pin)
{
    printf("interrupt?");
    touched = false;
}

} // namespace edge::drivers
