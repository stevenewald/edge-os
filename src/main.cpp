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
    using edge::userlib::change_priority;
    using edge::userlib::get_button_pressed;
    using edge::userlib::get_time_us;
    using edge::userlib::set_led;
    change_priority(1);
    int j = N;
    while (1) {
        j++;
        if (get_button_pressed(edge::drivers::ButtonType::A)) {
            set_led(4 - N, 4 - (j % 5), true);
            nrf_delay_ms(15);
            set_led(4 - N, 4 - (j % 5), false);
        }
        else {
            set_led(N, j % 5, true);
            nrf_delay_ms(15);
            set_led(N, j % 5, false);
        }
    }
}

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::scheduler.add_task(task<0>);
    edge::scheduler.add_task(task<1>);
    edge::scheduler.add_task(task<2>);
    edge::scheduler.add_task(task<3>);
    edge::scheduler.add_task(task<4>);

    edge::scheduler.start_scheduler();
}
