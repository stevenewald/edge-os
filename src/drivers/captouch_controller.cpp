#include "drivers/captouch_controller.hpp"

#include "drivers/driver_enums.hpp"
#include "drivers/gpio_pin_event.hpp"
#include "drivers/virtual_timer_controller.hpp"
#include "hal/gpio_wrapper.hpp"
#include "hal/hal_enums.hpp"
#include "microbit_v2.h"
#include "nrf_gpio.h"
#include "userlib/syscalls.hpp"

namespace edge::drivers {


CapsenseController::CapsenseController() :
    touched(false),
    test_done(false),
    event{
        TOUCH_LOGO, GPIOConfiguration::IN_NORES,
        aidan::GPIOEventController::GPIOEventCallback::create<
            CapsenseController, &CapsenseController::handle_gpio_interrupt>(*this)
    }
{
    VirtualTimerController& virtualTimerController = VirtualTimerController::get();
    virtualTimerController.virtual_timer_start(100000, etl::delegate<void()>::create<CapsenseController, &CapsenseController::start_capacitive_test>(*this), 0, true);
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
    if (test_done == false)
    {
        test_done = true;
        touched = false;
        printf("Not touched\n");
    }
    VirtualTimerController::get().virtual_timer_cancel(timed_id);
}

void CapsenseController::handle_timer_interrupt()
{
    if (test_done == false)
    {
        test_done = true;
        touched = true;
        printf("Touched\n");
    }

}

void CapsenseController::start_capacitive_test()
{
    printf("Starting cap test\n");
    test_done = false;
    VirtualTimerController& vtcontroller = VirtualTimerController::get();
    touch_logo.clear();
    /* vtcontroller.virtual_timer_start(30000, etl::delegate<void()>::create<CapsenseController, &CapsenseController::handle_timer_interrupt>(*this), 0, false); */
    nrf_gpio_cfg(
        TOUCH_LOGO, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE
    );
}


} // namespace edge::drivers
