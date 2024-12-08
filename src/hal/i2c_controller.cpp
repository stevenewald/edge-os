#include "hal/i2c_controller.hpp"
#include "hal/i2c_wrapper.hpp"

namespace edge::aidan {
static constexpr int QUEUE_SIZE = 1;

NRF_TWI_MNGR_DEF(twi_mngr, QUEUE_SIZE, 0);

I2CController& I2CController::get()
{
    static I2CController controller;
    return controller;
}

I2CController::I2CController()
{
    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = I2C_INTERNAL_SCL;
    i2c_config.sda = I2C_INTERNAL_SDA;
    i2c_config.frequency = NRF_DRV_TWI_FREQ_100K;
    i2c_config.interrupt_priority = 0;
    nrf_twi_mngr_init(&twi_mngr, &i2c_config);
    manager = twi_mngr;
    lsm303agr_init(&twi_mngr);
}

} // namespace edge::aidan
