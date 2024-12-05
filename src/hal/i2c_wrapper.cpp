#include "hal/i2c_wrapper.hpp"

#include "nrf_error.h"
#include "nrf_twi_mngr.h"
#include "sdk_errors.h"

namespace edge::aidan {

NRF_TWI_MNGR_DEF(twi_mngr, 1, 0);
static const nrf_twi_mngr_t* i2c_manager = &twi_mngr;

uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr)
{
    uint8_t rx_buf = 0;
    nrf_twi_mngr_transfer_t const read_transfer[] = {
        NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, NRF_TWI_MNGR_NO_STOP)
    };
    ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
    if (result != NRF_SUCCESS) {
        printf("I2C TRANSACTION FAILED!");
    }
    return rx_buf;
};

void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
    uint16_t rx_buf = (reg_addr << 8) | data;
    rx_buf = (data << 8) | reg_addr;
    nrf_twi_mngr_transfer_t const write_transfer[] = {
        NRF_TWI_MNGR_WRITE(i2c_addr, &rx_buf, 2, NRF_TWI_MNGR_NO_STOP),
    };
    ret_code_t result =
        nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
    if (result != NRF_SUCCESS) {
        printf("I2C transaction failed! Error: %lX\n", result);
    }
}

void lsm303agr_init()
{
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG5_A, 0x80);
    nrf_delay_ms(100); // needs delay to wait for reboot

    // Enable Block Data Update
    // Only updates sensor data when both halves of the data has been read
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG4_A, 0x80);

    // Configure accelerometer at 100Hz, normal mode (10-bit)
    // Enable x, y and z axes
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG1_A, 0x57);

    // ---Initialize Magnetometer---

    // Reboot magnetometer
    i2c_reg_write(LSM303AGR_MAG_ADDRESS, CFG_REG_A_M, 0x40);
    nrf_delay_ms(100); // needs delay to wait for reboot

    // Enable Block Data Update
    // Only updates sensor data when both halves of the data has been read
    i2c_reg_write(LSM303AGR_MAG_ADDRESS, CFG_REG_C_M, 0x10);

    // Configure magnetometer at 100Hz, continuous mode
    i2c_reg_write(LSM303AGR_MAG_ADDRESS, CFG_REG_A_M, 0x0C);

    i2c_reg_write(LSM303AGR_ACC_ADDRESS, TEMP_CFG_REG_A, 0xC0);
}

} // namespace edge::aidan
