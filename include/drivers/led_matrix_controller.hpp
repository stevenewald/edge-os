#pragma once
#include "drivers/gpio_pin.hpp"
#include "drivers/virtual_timer_controller.hpp"
#include "microbit_v2.h"

namespace edge::drivers {

// Synchronous
class LedMatrixController {
    static constexpr uint8_t WIDTH = 5;
    static constexpr uint8_t HEIGHT = 5;

    etl::array<etl::array<bool, WIDTH>, HEIGHT> led_enabled{};

    etl::array<GPIOPin, HEIGHT> led_rows = {
        GPIOPin{LED_ROW1, GPIOConfiguration::OUT},
        GPIOPin{LED_ROW2, GPIOConfiguration::OUT},
        GPIOPin{LED_ROW3, GPIOConfiguration::OUT},
        GPIOPin{LED_ROW4, GPIOConfiguration::OUT},
        GPIOPin{LED_ROW5, GPIOConfiguration::OUT}
    };

    etl::array<GPIOPin, WIDTH> led_cols = {
        GPIOPin{LED_COL1, GPIOConfiguration::OUT},
        GPIOPin{LED_COL2, GPIOConfiguration::OUT},
        GPIOPin{LED_COL3, GPIOConfiguration::OUT},
        GPIOPin{LED_COL4, GPIOConfiguration::OUT},
        GPIOPin{LED_COL5, GPIOConfiguration::OUT}
    };

public:
    static LedMatrixController& get();

    void set_led(uint8_t row, uint8_t col, bool enabled);

    LedMatrixController(LedMatrixController&) = delete;
    LedMatrixController(LedMatrixController&&) = delete;
    LedMatrixController& operator=(VirtualTimerController&) = delete;
    LedMatrixController& operator=(VirtualTimerController&&) = delete;

    friend void handle_matrix_controller();

private:
    LedMatrixController();
    ~LedMatrixController() = default;
};

} // namespace edge::drivers
