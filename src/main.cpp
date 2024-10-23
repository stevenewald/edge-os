#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "timer.hpp"
#include "scheduler.hpp"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Pin configurations
#include "microbit_v2.h"

int
main(void)
{
    // Initialize.
    nrf_gpio_cfg_output(LED_MIC);

	edge::KernelTimerController::get_instance();

    // Enter main loop.
    while (1) {
        nrf_delay_ms(10000);
		printf("Hi, currtime %lu\n", edge::KernelTimerController::get_instance().get_time());
        // printf("%ld\n", edge::TimerController::get_instance().get_time());
    }
}
