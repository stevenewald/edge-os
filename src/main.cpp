#include "drivers/driver_enums.hpp"
#include "scheduler.hpp"
#include "userlib/syscalls.hpp"

#include <stdbool.h>
#include <stdio.h>

// Pin configurations

template <int N>
void task(void)
{
    using namespace edge::userlib;
    using namespace edge::drivers;
    change_priority(1);

    static bool flipped = false;
    static void (*on_button_press)(ButtonType, ButtonState) = [](ButtonType type,
                                                                 ButtonState state) {
        if (state == ButtonState::DOWN) {
            if (type == ButtonType::A)
                flipped = true;
            else
                flipped = false;
        }
    };

    get_button_pressed(ButtonType::A, on_button_press);
    get_button_pressed(ButtonType::B, on_button_press);

    while (1) {
        if (flipped) {
            set_led(N, 0, true);
            yield();
            set_led(N, 0, false);
        }
        else {
            set_led(0, N, true);
            yield();
            set_led(0, N, false);
        }
    }
}

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::scheduler.add_task(task<4>);
    edge::scheduler.add_task(task<3>);
    edge::scheduler.add_task(task<2>);
    edge::scheduler.add_task(task<1>);
    edge::scheduler.add_task(task<0>);

    edge::scheduler.start_scheduler();
}
