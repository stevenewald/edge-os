#include "drivers/led_display.hpp"

#include "nrf_delay.h"

namespace edge::drivers {

void LedDisplay::set_output(uint8_t row, uint8_t col, bool enabled)
{
    led_rows[row].write(enabled);
    led_cols[col].write(!enabled);
}

LedDisplay::LedDisplay()
{
    etl::for_each(led_rows.begin(), led_rows.end(), [](auto& row) { row.clear(); });
    etl::for_each(led_cols.begin(), led_cols.end(), [](auto& col) { col.set(); });
}

void LedDisplay::set_led(uint8_t row, uint8_t col, bool enabled)
{
    led_enabled[row][col] = enabled;
}

void LedDisplay::display_pixels_once()
{
    for (uint8_t row = 0; row < HEIGHT; row++) {
        for (uint8_t col = 0; col < WIDTH; col++) {
            if (!led_enabled[row][col]) {
                continue;
            }
            set_output(row, col, true);
            nrf_delay_us(200);
            set_output(row, col, false);
        }
    }
}

LedDisplay led_display;
} // namespace edge::drivers
