#include "microbit_v2.h"
#include "userlib/syscalls.hpp"

#include <stdint.h>

#include <cstdio>

uint8_t row_index = 0;
bool led_states[5][5];
etl::array<uint32_t, 5> rows = {LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5};
etl::array<uint32_t, 5> cols = {LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5};

uint8_t control_row = 0;
uint8_t control_col = 0;

void upkeep_led_matrix(uint32_t id)
{
    etl::for_each(rows.begin(), rows.end(), [](auto& row) {
        edge::userlib::clear_pin(row);
    });
    for (int col_index = 0; col_index < 5; ++col_index) {
        bool state = led_states[row_index][col_index];
        if (!state) {
            edge::userlib::set_pin(cols[col_index]);
        }
        else {
            edge::userlib::clear_pin(cols[col_index]);
        }
    }
    edge::userlib::set_pin(rows[row_index]);
    row_index++;
    row_index = row_index % 5;
}

void update_led_states(uint32_t id)
{
    bool st = led_states[control_row][control_col];
    led_states[control_row][control_col] = !st;

    control_col++;
    if (control_col == 5) {
        control_row++;
        control_col = 0;
    }
    control_row %= 5;
}

void led_matrix_task(void)
{
    using namespace edge::userlib;
    etl::for_each(rows.begin(), rows.end(), [](auto& row) { config_pin_output(row); });
    etl::for_each(cols.begin(), cols.end(), [](auto& col) { config_pin_output(col); });
    /* config_pin_output(LED_COL1); */
    /* make_pin_output(LED_COL2); */
    /* make_pin_output(LED_COL3); */
    /* make_pin_output(LED_COL4); */
    /* make_pin_output(LED_COL5); */
    /**/
    /* make_pin_output(LED_ROW1); */
    /* make_pin_output(LED_ROW2); */
    /* make_pin_output(LED_ROW3); */
    /* make_pin_output(LED_ROW4); */
    /* make_pin_output(LED_ROW5); */
    /**/
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            led_states[i][j] = false;
        }
    }

    start_timer(upkeep_led_matrix, 3'500, true);
    start_timer(update_led_states, 10'000, true);

    while (1) {
        yield();
    }
}
