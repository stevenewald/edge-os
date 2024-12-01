#include "hal/i2c_wrapper.hpp"
#include "nrf_drv_twi.h"
#include "nrf_twi_mngr.h"
#include "nrf_twim.h"
#include "microbit_v2.h"
#include "userlib/syscalls.hpp"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

void temp_task(void)
{
    edge::userlib::debug_print("debug1\n");
    nrf_drv_twi_config_t config;
    config.frequency = NRF_DRV_TWI_FREQ_100K;
    config.scl = I2C_INTERNAL_SCL;
    config.sda = I2C_INTERNAL_SDA;
    config.interrupt_priority = 0;
    config.clear_bus_init = false;

    edge::userlib::debug_print("debug2\n");
    nrf_twi_mngr_init(&twi_mngr_instance, &config);
    edge::userlib::debug_print("debug3\n");
    edge::aidan::lsm303agr_init(&twi_mngr_instance);
    edge::userlib::debug_print("debug4\n");
    while (1) {
        edge::userlib::get_temperature();
        /* edge::userlib::debug_print("read temp"); */
    }
    edge::userlib::yield();
}
