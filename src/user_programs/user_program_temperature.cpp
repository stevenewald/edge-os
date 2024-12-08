#include "hal/i2c_wrapper.hpp"
#include "nrf_drv_twi.h"
#include "nrf_twi_mngr.h"
#include "nrf_twim.h"
#include "microbit_v2.h"
#include "userlib/syscalls.hpp"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

void temp_task(void)
{
    edge::userlib::debug_print("Start temperature call.\n");
    edge::userlib::get_temperature();
    edge::userlib::debug_print("End temperature call.\n");
    while (1) {
        edge::userlib::yield();
    }
}
