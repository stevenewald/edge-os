#include "hil/gpiote_wrapper.hpp"

#include <nrf_gpiote.h>

#include <cstdint>

namespace edge::aidan {

constexpr uint32_t MAX_GPIOTE_CHANNELS = 8;

etl::array<void (*)(int), MAX_GPIOTE_CHANNELS> gpiote_callbacks{nullptr};

void set_gpiote_callback(uint32_t channel, void (*callback)(int))
{
    gpiote_callbacks[channel] = callback;
    nrf_gpiote_int_enable(NRF_GPIOTE_INT_IN0_MASK << channel);
}

void clear_gpiote_event(uint32_t channel)
{
    nrf_gpiote_event_disable(channel);
    nrf_gpiote_int_disable(NRF_GPIOTE_INT_IN0_MASK << channel);
    gpiote_callbacks[channel] = nullptr;
}

void configure_gpiote_event(
    uint32_t channel, uint32_t pin_number, GPIOTEEventPolarity polarity
)
{
    if (channel < MAX_GPIOTE_CHANNELS) {
        nrf_gpiote_event_configure(
            channel, pin_number, static_cast<nrf_gpiote_polarity_t>(polarity)
        );
        nrf_gpiote_event_enable(channel);
    }
    NVIC_EnableIRQ(GPIOTE_IRQn);
}

extern "C" {
void GPIOTE_IRQHandler()
{
    static constexpr etl::array GPIOTE_EVENTS{
        NRF_GPIOTE_EVENTS_IN_0, NRF_GPIOTE_EVENTS_IN_1, NRF_GPIOTE_EVENTS_IN_2,
        NRF_GPIOTE_EVENTS_IN_3, NRF_GPIOTE_EVENTS_IN_4, NRF_GPIOTE_EVENTS_IN_5,
        NRF_GPIOTE_EVENTS_IN_6, NRF_GPIOTE_EVENTS_IN_7
    };

    for (uint32_t channel = 0; channel < MAX_GPIOTE_CHANNELS; ++channel) {
        if (nrf_gpiote_event_is_set(GPIOTE_EVENTS[channel])) {
            nrf_gpiote_event_clear(GPIOTE_EVENTS[channel]);

            if (gpiote_callbacks[channel] != nullptr) {
                gpiote_callbacks[channel](channel);
            }
        }
    }
}
}

} // namespace edge::aidan
