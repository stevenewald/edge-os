#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "scheduler.hpp"
#include "syscalls.hpp"
#include "timer.hpp"

#include <stdbool.h>
#include <stdio.h>

// Pin configurations
#include "microbit_v2.h"

static constexpr auto TASK0_PRIO = 5;
static constexpr auto TASK1_PRIO = 10;
static constexpr auto TASK2_PRIO = 20;

void
task0(void)
{
    nrf_gpio_cfg_output(LED_COL1);
    int i = 0;
    while (1) {
        if (i++ % 300000 == 0)
            nrf_gpio_pin_toggle(LED_COL1);
    }
}

// Demonstrates priority change
void
task1(void)
{
    edge::userlib::change_priority(2);

    nrf_gpio_cfg_output(LED_COL2);
    int i = 0;
    while (1) {
        if (i++ % 300000 == 0)
            nrf_gpio_pin_toggle(LED_COL2);
    }
}

// Demonstrates yielding
// Toggle LED then yield
void
task2(void)
{
    nrf_gpio_cfg_output(LED_COL3);
    while (1) {
        nrf_gpio_pin_toggle(LED_COL3);
        edge::userlib::yield();
    }
}

int
main(void)
{
    // edge::KernelTimerController::get_instance();

    printf("Starting EdgeOS\n");
    nrf_gpio_cfg_output(LED_ROW1);
    nrf_gpio_pin_set(LED_ROW1);

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
