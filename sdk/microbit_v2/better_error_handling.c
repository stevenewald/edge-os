// Better app error handler
//
// Prints app error conditions over printf and blinks LEDs. If printf is
// already set up, the print statements should appear over the normal medium
// (probably RTT). LED pins are board-specific configurations. Mostly copied
// from `app_error_weak.c`

#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "hardfault.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// include softdevice faults if applicable
#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#endif

// ***** Board Specific Configuration *****
#include "microbit_v2.h"
#define GRID 5
static const uint8_t LED_ROWS[GRID] = {LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5};
static const uint8_t LED_COLS[GRID] = {LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5};

static void error_blink_pattern(void) {
  // Note: we are ignoring error codes from the following GPIO calls. We don't
  // have a way to deal with them other than ignore them at this point anyways

  // reset GPIO pins
  for (int i=0; i<GRID; i++) {
    nrf_gpio_cfg_default(LED_ROWS[i]);
    nrf_gpio_cfg_default(LED_COLS[i]);
  }

  // configure desired pins (four corners)
  nrf_gpio_cfg(LED_ROWS[0], NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0H1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_cfg(LED_ROWS[4], NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0H1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_cfg(LED_COLS[0], NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0S1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_cfg(LED_COLS[4], NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0S1, NRF_GPIO_PIN_NOSENSE);

  // initialize pins
  nrf_gpio_pin_set(LED_ROWS[0]);
  nrf_gpio_pin_set(LED_ROWS[4]);
  nrf_gpio_pin_clear(LED_COLS[0]);
  nrf_gpio_pin_clear(LED_COLS[4]);

  // loop forever blinking LEDs in a particular pattern to attact attention
  while (1) {
    nrf_gpio_pin_clear(LED_COLS[0]);
    nrf_gpio_pin_clear(LED_COLS[4]);
    nrf_delay_ms(500);

    nrf_gpio_pin_set(LED_COLS[0]);
    nrf_gpio_pin_set(LED_COLS[4]);
    nrf_delay_ms(50);

    nrf_gpio_pin_clear(LED_COLS[0]);
    nrf_gpio_pin_clear(LED_COLS[4]);
    nrf_delay_ms(500);

    nrf_gpio_pin_set(LED_COLS[0]);
    nrf_gpio_pin_set(LED_COLS[4]);
    nrf_delay_ms(250);
  }
}
// ****************************************

// app error handler. Overwrites weak definition in app_error_weak.c
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  // halt all existing state
  __disable_irq();
  NRF_LOG_FINAL_FLUSH();

  // print banner
  printf("\n\n***** App Error *****\n");

  // check cause of error
  switch (id) {
#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
    case NRF_FAULT_ID_SD_ASSERT: {
      printf("SOFTDEVICE: ASSERTION FAILED");
      break;
    }
    case NRF_FAULT_ID_APP_MEMACC: {
      printf("SOFTDEVICE: INVALID MEMORY ACCESS");
      break;
    }
#endif
    case NRF_FAULT_ID_SDK_ASSERT: {
        assert_info_t * p_info = (assert_info_t *)info;
        printf("ASSERTION FAILED at %s:%u\n",
            p_info->p_file_name,
            p_info->line_num);
        break;
      }
    case NRF_FAULT_ID_SDK_ERROR: {
        error_info_t * p_info = (error_info_t *)info;
        printf("ERROR %lu [%s] at %s:%lu\t\tPC at: 0x%08lX\n",
            p_info->err_code,
            nrf_strerror_get(p_info->err_code),
            p_info->p_file_name,
            p_info->line_num,
            pc);
        break;
      }
    default: {
      printf("UNKNOWN FAULT at 0x%08lX\n", pc);
      break;
    }
  }

  error_blink_pattern();
}

// process-specific HardFault handler. Overwrites weak definition in hardfault_implementation.c
void HardFault_process(HardFault_stack_t* p_stack) {
  error_blink_pattern();
}

// Copied from `hardfault_implementation.c` in SDK 16.0.0
void HardFault_c_handler(uint32_t * p_stack_address)
{
    NRF_LOG_FINAL_FLUSH();

#if (__CORTEX_M == 0x04)

#ifndef CFSR_MMARVALID
  #define CFSR_MMARVALID (1 << (0 + 7))
#endif

#ifndef CFSR_BFARVALID
  #define CFSR_BFARVALID (1 << (8 + 7))
#endif

    HardFault_stack_t * p_stack = (HardFault_stack_t *)p_stack_address;
    static const char *cfsr_msgs[] = {
        [0]  = "The processor has attempted to execute an undefined instruction",
        [1]  = "The processor attempted a load or store at a location that does not permit the operation",
        [2]  = NULL,
        [3]  = "Unstack for an exception return has caused one or more access violations",
        [4]  = "Stacking for an exception entry has caused one or more access violations",
        [5]  = "A MemManage fault occurred during floating-point lazy state preservation",
        [6]  = NULL,
        [7]  = NULL,
        [8]  = "Instruction bus error",
        [9]  = "Data bus error (PC value stacked for the exception return points to the instruction that caused the fault)",
        [10] = "Data bus error (return address in the stack frame is not related to the instruction that caused the error)",
        [11] = "Unstack for an exception return has caused one or more BusFaults",
        [12] = "Stacking for an exception entry has caused one or more BusFaults",
        [13] = "A bus fault occurred during floating-point lazy state preservation",
        [14] = NULL,
        [15] = NULL,
        [16] = "The processor has attempted to execute an undefined instruction",
        [17] = "The processor has attempted to execute an instruction that makes illegal use of the EPSR",
        [18] = "The processor has attempted an illegal load of EXC_RETURN to the PC, as a result of an invalid context, or an invalid EXC_RETURN value",
        [19] = "The processor has attempted to access a coprocessor",
        [20] = NULL,
        [21] = NULL,
        [22] = NULL,
        [23] = NULL,
        [24] = "The processor has made an unaligned memory access",
        [25] = "The processor has executed an SDIV or UDIV instruction with a divisor of 0",
    };

    uint32_t cfsr = SCB->CFSR;

    if (p_stack != NULL)
    {
        // Print information about error.
        NRF_LOG_ERROR("HARD FAULT at 0x%08X", p_stack->pc);
        NRF_LOG_ERROR("  R0:  0x%08X  R1:  0x%08X  R2:  0x%08X  R3:  0x%08X",
                             p_stack->r0, p_stack->r1, p_stack->r2, p_stack->r3);
        NRF_LOG_ERROR("  R12: 0x%08X  LR:  0x%08X  PSR: 0x%08X",
                             p_stack->r12, p_stack->lr, p_stack->psr);
    }
    else
    {
        NRF_LOG_ERROR("Stack violation: stack pointer outside stack area.");
    }

    if (SCB->HFSR & SCB_HFSR_VECTTBL_Msk)
    {
        NRF_LOG_ERROR("Cause: BusFault on a vector table read during exception processing.");
    }

    for (uint32_t i = 0; i < sizeof(cfsr_msgs) / sizeof(cfsr_msgs[0]); i++)
    {
        if (((cfsr & (1 << i)) != 0) && (cfsr_msgs[i] != NULL))
        {
            NRF_LOG_ERROR("Cause: %s.", (uint32_t)cfsr_msgs[i]);
        }
    }

    if (cfsr & CFSR_MMARVALID)
    {
        NRF_LOG_ERROR("MemManage Fault Address: 0x%08X", SCB->MMFAR);
    }

    if (cfsr & CFSR_BFARVALID)
    {
        NRF_LOG_ERROR("Bus Fault Address: 0x%08X", SCB->BFAR);
    }

// Branden: I removed this because it was causing Microbits to halt with OpenOCD and not blink the LEDs
//#if defined(DEBUG)
//
//    NRF_BREAKPOINT_COND;
//
// #endif // defined (DEBUG)

#endif // __CORTEX_M == 0x04

    HardFault_process((HardFault_stack_t *)p_stack_address);
}

