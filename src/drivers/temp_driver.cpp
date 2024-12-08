#include "drivers/temp_driver.hpp"

#include "hal/i2c_wrapper.hpp"

namespace edge::drivers
{

float read_temperature()
{
    return 25.0;
}


} // namespace edge::drivers
