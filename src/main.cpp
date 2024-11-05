#include "nrf_delay.h"
#include "scheduler.hpp"
#include "userlib/syscalls.hpp"

#include <stdbool.h>
#include <stdio.h>

// Pin configurations

static constexpr auto TASK0_PRIO = 1;
static constexpr auto TASK1_PRIO = 50;

template <int N>
void task(void)
{
    using namespace edge::userlib;
    using namespace edge::drivers;
    change_priority(1);

    static bool flipped = false;
    static int num = 0;
    static void (*on_button_press)(ButtonType) = [](ButtonType button_type) {
		// volatile int num = 0;
        // num++;
        // etl::string<20> v;
        // etl::to_string(num, v);
        // debug_println(v);
        flipped = !flipped;
        nrf_delay_ms(1000);
    };

    get_button_pressed(ButtonType::A, on_button_press);
    // get_button_pressed(ButtonType::B, on_button_press);

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

    // edge::scheduler.add_task(task<0>);
    // edge::scheduler.add_task(task<1>);
    // edge::scheduler.add_task(task<2>);
    // edge::scheduler.add_task(task<3>);
    edge::scheduler.add_task(task<4>);

    edge::scheduler.start_scheduler();
}
