#include "hil_enums.hpp"

#include <nrf_gpiote.h>

#include <cstdint>

namespace edge::aidan {

void set_gpiote_callback(uint32_t channel, void (*callback)(int));

void clear_gpiote_event(uint32_t channel);

void configure_gpiote_event(
    uint32_t channel, uint32_t pin_number, GPIOTEEventPolarity polarity
);
} // namespace edge::aidan
