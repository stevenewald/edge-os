#pragma once

#include "drivers/driver_enums.hpp"
#include "util.hpp"
#include "stdint.h"

namespace edge::userlib {

void change_priority(uint8_t new_priority);

void yield();

void set_led(uint8_t row, uint8_t col, bool enabled);

int get_time_us();

bool get_button_pressed(drivers::ButtonType button_type);

void get_button_pressed(
    drivers::ButtonType button_type,
    void (*callback)(drivers::ButtonType, drivers::ButtonState)
);

void send_ipc(const char* name, uint32_t message);

void subscribe_ipc(const char* name, void (*callback)(int message));

void set_fault_handler(void (*callback)(FaultType));

uint32_t start_vtimer(void (*callback)(), uint32_t microseconds);

void cancel_vtimer(uint32_t timer_id);

// We need a syscall for this because SVC will not be preempted by SysTick
// Technically this is insecure - it's mostly for debugging
void debug_print(const char* string);

} // namespace edge::userlib
