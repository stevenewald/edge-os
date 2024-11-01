#include "drivers/led_display.hpp"
#include "nrf_delay.h"
#include "scheduler.hpp"
#include "syscalls.hpp"
#include "timer.hpp"

#include <stdbool.h>
#include <stdio.h>

// Pin configurations

static constexpr auto TASK0_PRIO = 20;
static constexpr auto TASK1_PRIO = 20;
static constexpr auto TASK2_PRIO = 20;

int i = 0;
int j = 0;

void task0(void)
{
    edge::drivers::LedDisplay display;
    while (1) {
        if (i++ % 200 == 0) {
            display.set_led(j % 5, (j % 25) / 5, false);
            j++;
            display.set_led(j % 5, (j % 25) / 5, true);
        }
        display.do_work();
    }
}

// Demonstrates priority change
void task1(void)
{
    edge::drivers::LedDisplay display;
    while (1) {
        if (i++ % 200 == 0) {
            display.set_led((j % 5), ((j % 25) / 5), false);
            j++;
            display.set_led(4 - (j % 5), 4 - ((j % 25) / 5), true);
        }
        display.do_work();
    }
}

// Demonstrates yielding
// Toggle LED then yield
void task2(void)
{
    // edge::drivers::GPIOPin col3(LED_COL3, edge::drivers::GPIOConfiguration::OUT);
    while (1) {
        // col3.toggle();
        edge::userlib::yield();
    }
}

int main(void)
{
    // edge::KernelTimerController::get_instance();

    printf("Starting EdgeOS\n");

    edge::scheduler.add_task(task0, TASK0_PRIO);
    edge::scheduler.add_task(task1, TASK1_PRIO);
    edge::scheduler.add_task(task2, TASK2_PRIO);

    edge::scheduler.start_scheduler();

    while (1) {
        nrf_delay_ms(10000);
        printf(
            "Hi, currtime %lu\n", edge::KernelTimerController::get_instance().get_time()
        );
        // printf("%ld\n", edge::TimerController::get_instance().get_time());
    }
}
