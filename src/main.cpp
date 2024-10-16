// Blink app
//
// Blinks an LED
//
#define ETL_NO_STL 1

// #include <array.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include <stdbool.h>
#include <stdint.h>

// Pin configurations
#include "microbit_v2.h"

int main(void) {
  etl::array<int, 5> arr = {250, 500, 750, 1000, 2000, 5000};

  // Initialize.
  nrf_gpio_cfg_output(LED_MIC);

  int i = 0;
  // Enter main loop.
  while (1) {
    nrf_gpio_pin_toggle(LED_MIC);
    i = (i + 1) % arr.size();
    nrf_delay_ms(arr[i]);
    nrf_gpio_pin_toggle(LED_MIC);
    nrf_delay_ms(200);
  }
}
