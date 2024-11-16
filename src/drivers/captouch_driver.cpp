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

} // end ns drivers

