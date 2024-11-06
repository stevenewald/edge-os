#pragma once

#include "hil/gpiote_wrapper.hpp"
#include "hil/hil_enums.hpp"

namespace edge::drivers {
class GPIOTEPin {
    uint32_t channel;

public:
    // TODO: use PORT event instead so we aren't limited to 8 of these
    GPIOTEPin(
        uint32_t channel, uint32_t pin_number, aidan::GPIOTEEventPolarity polarity,
        void (*callback)(int)
    ) : channel(channel)
    {
        aidan::configure_gpiote_event(channel, pin_number, polarity);
        aidan::set_gpiote_callback(channel, callback);
    }

    GPIOTEPin(const GPIOTEPin&) = delete;
    GPIOTEPin(GPIOTEPin&&) = delete;
    GPIOTEPin& operator=(const GPIOTEPin&) = delete;
    GPIOTEPin& operator=(GPIOTEPin&&) = delete;

    ~GPIOTEPin() { aidan::clear_gpiote_event(channel); }
};
} // namespace edge::drivers
