#include "hal/hal_enums.hpp"

#include <cstdint>

namespace edge::aidan {

extern "C" void GPIOTE_IRQHandler();

static constexpr uint8_t GPIO_PINS = 32;

// TODO: Add a controller for other GPIOTE events (channels)?
class GPIOEventController {
public:
    using GPIOEventCallback = void (*)(nrf_gpio_pin_sense_t sense, int pin);

private:
    etl::array<GPIOEventCallback, GPIO_PINS> callbacks{nullptr};

    void handle_gpiote_port_event() const;
    GPIOEventController();

public:
    ~GPIOEventController();

    static GPIOEventController& get()
    {
        static GPIOEventController controller;
        return controller;
    }

    GPIOEventController& operator=(const GPIOEventController&) = delete;
    GPIOEventController& operator=(GPIOEventController&&) = delete;
    GPIOEventController(const GPIOEventController&) = delete;
    GPIOEventController(GPIOEventController&&) = delete;

    // TODO/idea, what if we instead made this a factory?
    void set_gpio_callback(
        uint32_t pin, PinPullMode pin_resistance, GPIOEventCallback callback
    );

    void clear_gpio_callback(uint32_t pin);

    friend void GPIOTE_IRQHandler();
};

} // namespace edge::aidan
