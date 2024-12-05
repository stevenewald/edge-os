#include "drivers/driver_commands.hpp"

#include "drivers/button_driver.hpp"
#include "drivers/driver_enums.hpp"
#include "drivers/led_display.hpp"
#include "drivers/timer.hpp"
#include "drivers/virtual_timer_controller.hpp"

#include <stdio.h>

namespace edge::drivers {

// Runs on context switch
void do_async_work()
{
    led_display.do_async_work();
}

etl::optional<int> handle_command(DriverCommand type, int arg1, int arg2, int arg3)
{
    switch (type) {
        case DriverCommand::LED_DISPLAY:
            led_display.set_led(arg1, arg2, arg3);
            break;
        case DriverCommand::GET_TIME:
            return timer4_controller.get_time_us();
        case DriverCommand::TERMINAL_OUTPUT:
            printf((char*)arg1);
            break;
        case DriverCommand::BUTTONS:
            return ButtonController::get().get_button_pressed(
                static_cast<ButtonType>(arg1)
            );
        case DriverCommand::TIMER_START:
            return VirtualTimerController::get().virtual_timer_start(static_cast<uint32_t>(arg2), (void*) arg1);
        case DriverCommand::TIMER_CANCEL:
            VirtualTimerController::get().virtual_timer_cancel(static_cast<uint32_t>(arg1));
            break;
    }
    return etl::nullopt;
}

void handle_subscribe(
    DriverSubscribe type, ProcessCallbackPtr callback, int arg1, int arg2,
    ProcessId process_id
)
{
    switch (type) {
        case DriverSubscribe::NOTIFY_BUTTON_PRESS:
            ButtonController::get().subscribe_button_press(
                static_cast<ButtonType>(arg1), callback, process_id
            );
            break;
    }
}

} // namespace edge::drivers
