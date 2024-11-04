#pragma once

#include <nrf_gpiote.h>

namespace edge::aidan {
enum class InputResistor { PUR, PDR, NONE };

enum class GPIOTEEventPolarity {
    LO_TO_HI = NRF_GPIOTE_POLARITY_LOTOHI,
    HI_TO_LO = NRF_GPIOTE_POLARITY_HITOLO,
    TOGGLE = NRF_GPIOTE_POLARITY_TOGGLE,
};

enum class GPIOTETask { SET, CLEAR };
} // namespace edge::aidan
