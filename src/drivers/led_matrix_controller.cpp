#include "drivers/led_matrix_controller.hpp"

#include "drivers/virtual_timer_controller.hpp"
#include "nrf_delay.h"
#include "userlib/syscalls.hpp"
#include "util.hpp"

namespace edge::drivers {

void handle_matrix_controller()
{
    static uint8_t row_index = 0;
    static uint8_t col_index = 0;
    LedMatrixController& controller = LedMatrixController::get();

    etl::for_each(
        controller.led_rows.begin(), controller.led_rows.end(),
        [](auto& led_row) { led_row.clear(); }
    );

    bool enabled = controller.led_enabled[row_index][col_index];
    controller.set_output(row_index, col_index, enabled);

    col_index++;
    if (col_index == 5) {
        ++row_index;
        col_index = 0;
    }
    row_index %= 5;
}

LedMatrixController::LedMatrixController()
{
    etl::for_each(led_rows.begin(), led_rows.end(), [](auto& row) { row.clear(); });
    etl::for_each(led_cols.begin(), led_cols.end(), [](auto& col) { col.set(); });
    VirtualTimerController::get().virtual_timer_start(
        25 / 5, etl::delegate<void()>::create<handle_matrix_controller>(), 0, true
    );
}

LedMatrixController& LedMatrixController::get()
{
    static LedMatrixController controller;
    return controller;
}

void LedMatrixController::set_output(uint8_t row, uint8_t col, bool enabled)
{
    led_rows[row].write(enabled);
    led_cols[col].write(!enabled);
}

void LedMatrixController::set_led(uint8_t row, uint8_t col, bool enabled)
{
    led_enabled[row][col] = enabled;
}

/* void LedMatrixController::do_async_work(uint32_t id) */
/* { */
/*     for (uint8_t row = 0; row < HEIGHT; row++) { */
/*         for (uint8_t col = 0; col < WIDTH; col++) { */
/*             if (!led_enabled[row][col]) { */
/*                 continue; */
/*             } */
/*             set_output(row, col, true); */
/*             nrf_delay_us(200); */
/*             set_output(row, col, false); */
/*         } */
/*     } */
/* } */

} // namespace edge::drivers
