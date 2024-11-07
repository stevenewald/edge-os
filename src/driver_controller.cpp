#include "drivers/driver_controller.hpp"

#include "drivers/buttons.hpp"
#include "drivers/driver_enums.hpp"
#include "drivers/led_display.hpp"
#include "drivers/timer.hpp"

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
            return button_controller.get_button_pressed(static_cast<ButtonType>(arg1));
    }
    return etl::nullopt;
}

void handle_subscribe(
    DriverSubscribe type, SubscribeCallbackPtr callback, int arg1, int arg2,
    uint8_t process_id
)
{
    switch (type) {
        case DriverSubscribe::NOTIFY_BUTTON_PRESS:
            button_controller.subscribe_button_press(
                static_cast<ButtonType>(arg1), callback, process_id
            );
            break;
    }
}

etl::optional<subscribe_callback> get_ready_callback(uint8_t process_id)
{
    return button_controller.get_ready_callback(process_id);
}

} // namespace edge::drivers
