#include "drivers/driver_enums.hpp"
#include "hal/hal_enums.hpp"
#include "util.hpp"

namespace edge::drivers
{
aidan::PinPullMode to_pin_pull_mode(GPIOConfiguration configuration)
{
    switch (configuration) {
        case GPIOConfiguration::IN_NORES:
            return aidan::PinPullMode::NONE;
        case GPIOConfiguration::IN_PDR:
            return aidan::PinPullMode::PDR;
        case GPIOConfiguration::IN_PUR:
            return aidan::PinPullMode::PUR;
        default:
            panic("Unexpected GPIO Configuration conversion to PinPullMode.");
    }
}


} // namespace edge::drivers
