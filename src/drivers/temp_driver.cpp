#include "drivers/temp_driver.hpp"

#include "hal/i2c_wrapper.hpp"

namespace edge::drivers
{

float read_temperature()
{
    uint8_t lsb = aidan::i2c_reg_read(aidan::LSM303AGR_ACC_ADDRESS, aidan::OUT_TEMP_L_A);
    uint8_t msb = aidan::i2c_reg_read(aidan::LSM303AGR_ACC_ADDRESS, aidan::OUT_TEMP_H_A);
    int16_t temp = (msb << 8) | lsb;
    float ftemp = static_cast<float>(temp);
    ftemp *= (1.0 / 256.0);
    ftemp += 25.0;
    return ftemp;
}


} // namespace edge::drivers
