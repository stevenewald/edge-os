// Before startup
//
// This is a function that allows for various C code to be run on start,
// notably after the initial configuration but before main is called

#include <stdio.h>

#include "nrf_delay.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// the constructor attribute ensures it's called before main
void called_first(void) __attribute__ ((constructor));

void called_first (void) {
  setbuf(stdout, NULL);

  // initialize logging
  ret_code_t error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // holding reset restarts repeatedly
  // this brief delay means `main()` only runs when reset is released
  nrf_delay_ms(10);
}

