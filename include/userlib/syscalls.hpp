#pragma once

#include "drivers/button_type.hpp"

namespace edge::userlib {

void change_priority(uint8_t new_priority);

void yield();

void set_led(uint8_t row, uint8_t col, bool enabled);

int get_time_us();

bool get_button_pressed(drivers::ButtonType button_type);

// We need a syscall for this because SVC will not be preempted by SysTick
// Technically this is insecure - it's mostly for debugging
void debug_print(const char* string);

// yeah i know
template <size_t Length>
inline void debug_println(const etl::string<Length>& str)
{
    etl::string<Length + 1> str2 = str;
    str2 += "\n";
    debug_print(str2.data());
}

} // namespace edge::userlib
