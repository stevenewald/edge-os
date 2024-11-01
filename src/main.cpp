#include "gpio_pin.hpp"
#include "gpio_wrapper.hpp"
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

void task0(void)
{
    edge::drivers::GPIOPin col1(LED_COL1, edge::drivers::GPIOConfiguration::OUT);
    while (1) {
        col1.toggle();
    }
}

// Demonstrates priority change
void task1(void)
{
    edge::userlib::change_priority(2);
    edge::drivers::GPIOPin col2(LED_COL2, edge::drivers::GPIOConfiguration::OUT);
    col2.set();
    while (1) {
        col2.toggle();
    }
}

// Demonstrates yielding
// Toggle LED then yield
void task2(void)
{
    /* edge::aidan::set_gpio_output(LED_COL3); */
    edge::drivers::GPIOPin col3(LED_COL3, edge::drivers::GPIOConfiguration::OUT);
    while (1) {
        col3.toggle();
        edge::userlib::yield();
    }
}

int main(void)
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
