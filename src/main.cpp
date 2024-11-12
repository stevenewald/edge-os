#include "drivers/driver_enums.hpp"
#include "scheduler.hpp"
#include "userlib/syscalls.hpp"

#include <stdbool.h>
#include <stdio.h>

void task1(void)
{
    using namespace edge::userlib;
    using namespace edge::drivers;

    static void (*on_button_press)(ButtonType, ButtonState) = [](ButtonType type,
                                                                 ButtonState state) {
        if (state == ButtonState::DOWN) {
            if (type == ButtonType::A)
                send_ipc("LED_DISPLAY", true);
            else
                send_ipc("LED_DISPLAY", false);
        }
    };

    get_button_pressed(ButtonType::A, on_button_press);
    get_button_pressed(ButtonType::B, on_button_press);

    while (1) {
        yield();
    }
}

void task0(void)
{
    using namespace edge::userlib;

    static bool flipped = false;
    static void (*ipc_callback)(int) = [](int value) { flipped = value; };

    subscribe_ipc("LED_DISPLAY", ipc_callback);

    while (1) {
        if (flipped) {
            set_led(4, 0, true);
            yield();
            set_led(4, 0, false);
        }
        else {
            set_led(0, 4, true);
            yield();
            set_led(0, 4, false);
        }
    }
}

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::scheduler.add_task(task0);
    edge::scheduler.add_task(task1);

    edge::scheduler.start_scheduler();
}
