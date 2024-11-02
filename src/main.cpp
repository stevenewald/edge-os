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
	using edge::userlib::set_led;
	using edge::userlib::get_time_us;
    change_priority(1);
    int j = 0;
    while (1) {
        set_led(4 - (N + j) % 5, 4 - N, false);
        set_led((N + j) % 5, N, false);
        j++;
        if (get_time_us()/2'000'000 & 1) {
            set_led((N + j) % 5, N, true);
        }
        else {
            set_led(4 - (N + j) % 5, 4 - N, true);
        }
        nrf_delay_ms(15);
    }
}

[[maybe_unused]] void print_time(void)
{
    while (1) {
        etl::string<50> str;
        etl::to_string(edge::userlib::get_time_us(), str);
        edge::userlib::debug_println(str);
        edge::userlib::yield();
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
