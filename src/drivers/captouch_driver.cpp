#include "drivers/driver_enums.hpp"
#include "drivers/driver_enums.hpp"
#include "drivers/capsense_driver.hpp"

namespace edge::drivers {

CapsenseController& CapsenseController::get()
{
    static CapsenseController capsenseController;
    return capsenseController;
}

void CapsenseController::subscribe_captouch_press(ProcessCallbackPtr callback, uint8_t process_id)
{
    subscriptions[process_id] = callback;
    return;
}

bool CapsenseController::get_captouch_pressed()
{
    return touched;
}

void CapsenseController::handle_gpio_interrupt(nrf_gpio_pin_sense_t sense)
{
    touched = true;
}

void CapsenseController::handle_timer_interrupt(nrf_timer_event_t event, void* context)
{
    touched = false;
}

} // end ns drivers

