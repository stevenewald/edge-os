#include "drivers/led_matrix_controller.hpp"

#include "drivers/virtual_timer_controller.hpp"
#include "nrf_delay.h"
#include "userlib/syscalls.hpp"
#include "util.hpp"

namespace edge::drivers {

void handle_matrix_controller()
{
    static uint8_t row_index = 0;
    LedMatrixController& controller = LedMatrixController::get();

    etl::for_each(
        controller.led_rows.begin(), controller.led_rows.end(),
        [](auto& led_row) { led_row.clear(); }
    );

    for (uint8_t col_index = 0; col_index < 5; ++col_index) {
        bool state = controller.led_enabled[row_index][col_index];
        if (!state) {
            controller.led_cols[col_index].set();
        }
        else {
            controller.led_cols[col_index].clear();
        }
    }
    controller.led_rows[row_index].set();
    row_index++;
    row_index = row_index % 5;
}

LedMatrixController::LedMatrixController()
{
    etl::for_each(led_rows.begin(), led_rows.end(), [](auto& row) { row.clear(); });
    etl::for_each(led_cols.begin(), led_cols.end(), [](auto& col) { col.set(); });
    VirtualTimerController::get().virtual_timer_start(
        100, etl::delegate<void()>::create<handle_matrix_controller>(), 0, true
    );
}

LedMatrixController& LedMatrixController::get()
{
    static LedMatrixController controller;
    return controller;
}

void LedMatrixController::set_led(uint8_t row, uint8_t col, bool enabled)
{
    led_enabled[row][col] = enabled;
}

} // namespace edge::drivers
