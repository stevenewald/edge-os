#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "scheduler.hpp"
#include "timer.hpp"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Pin configurations
#include "microbit_v2.h"

static constexpr auto TASK0_PRIO = 5;
static constexpr auto TASK1_PRIO = 10;
static constexpr auto TASK2_PRIO = 20;

void
task0(void)
{
    // Change priority (optional)
    asm("SVC #14");

    nrf_gpio_cfg_output(LED_COL1);
    int i = 0;
    while (1) {
        if (i++ % 300000 == 0)
            nrf_gpio_pin_toggle(LED_COL1);
    }
}

void
task1(void)
{
    nrf_gpio_cfg_output(LED_COL2);
    int i = 0;
    while (1) {
        if (i++ % 300000 == 0)
            nrf_gpio_pin_toggle(LED_COL2);
    }
}

void
task2(void)
{
    nrf_gpio_cfg_output(LED_COL3);
    int i = 0;
    while (1) {
        if (i++ % 300000 == 0)
            nrf_gpio_pin_toggle(LED_COL3);
    }
}

int
main(void)
{
    // Initialize.
    // nrf_gpio_cfg_output(LED_MIC);

    // edge::KernelTimerController::get_instance();

    // Enter main loop.
    printf("Starting\n");
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
