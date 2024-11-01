#pragma once
#include "gpio_pin.hpp"
#include "microbit_v2.h"
#include "nrf_delay.h"

namespace edge::drivers {

// Synchronous
class LedDisplay {
    static constexpr size_t WIDTH = 5;
    static constexpr size_t HEIGHT = 5;

    etl::array<etl::array<bool, WIDTH>, HEIGHT> led_enabled{};

    etl::array<GPIOPin, HEIGHT> led_rows = {
        GPIOPin{LED_ROW1, OUT},
        GPIOPin{LED_ROW2, OUT},
        GPIOPin{LED_ROW3, OUT},
        GPIOPin{LED_ROW4, OUT},
        GPIOPin{LED_ROW5, OUT}
    };

    etl::array<GPIOPin, WIDTH> led_cols = {
        GPIOPin{LED_COL1, OUT},
        GPIOPin{LED_COL2, OUT},
        GPIOPin{LED_COL3, OUT},
        GPIOPin{LED_COL4, OUT},
        GPIOPin{LED_COL5, OUT}
    };

    void set_output(size_t row, size_t col, bool enabled)
    {
        led_rows[row].write(enabled);
        led_cols[col].write(!enabled);
    }

public:
    LedDisplay()
    {
        etl::for_each(led_rows.begin(), led_rows.end(), [](auto& row) { row.clear(); });
        etl::for_each(led_cols.begin(), led_cols.end(), [](auto& col) { col.set(); });
    }

    void set_led(size_t row, size_t col, bool enabled)
    {
        led_enabled[row][col] = enabled;
    }

    void do_work()
    {
        for (size_t row = 0; row < HEIGHT; row++) {
            for (size_t col = 0; col < WIDTH; col++) {
                if (led_enabled[row][col]) {
                    set_output(row, col, true);
                }
                nrf_delay_us(10);
                if (led_enabled[row][col]) {
                    set_output(row, col, false);
                }
            }
        }
    }
};
} // namespace edge::drivers
