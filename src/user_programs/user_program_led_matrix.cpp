#include "hal/gpio_wrapper.hpp"
#include "microbit_v2.h"
#include "scheduler/pending_process_callbacks.hpp"
#include "userlib/syscalls.hpp"

#include <stdint.h>

#include <cstdio>

uint8_t row_index = 0;
bool led_states[5][5] = {false};
uint32_t rows[] = {LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5};
uint32_t cols[] = {LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5};

static uint8_t control_row = 0;
static uint8_t control_col = 0;

void upkeep_led_matrix(uint32_t id)
{
    edge::userlib::clear_pin(rows[0]);
    edge::userlib::clear_pin(rows[1]);
    edge::userlib::clear_pin(rows[2]);
    edge::userlib::clear_pin(rows[3]);
    edge::userlib::clear_pin(rows[4]);
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
    // We want to call this so that the led_matrix driver sets up all the directions of
    // the pins so we don't have to...
    set_led(2, 2, true);
    set_led(2, 2, false);

    start_timer(upkeep_led_matrix, 2'500, true);
    start_timer(update_led_states, 10'000, true);

    while (1) {
        yield();
    }
}
