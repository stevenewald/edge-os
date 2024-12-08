#pragma once
#include "hal/i2c_wrapper.hpp"
#include "microbit_v2.h"
#include "nrf_drv_twi.h"
#include "nrf_twi.h"
#include "nrf_twi_mngr.h"


namespace edge::aidan {

class I2CController {
public:
    ~I2CController() = default;

    static I2CController& get();

    I2CController(I2CController&) = delete;
    I2CController(I2CController&&) = delete;
    I2CController& operator=(I2CController&) = delete;
    I2CController& operator=(I2CController&&) = delete;

private:
    I2CController();
    nrf_twi_mngr_t manager;
};

} // namespace edge::aidan
