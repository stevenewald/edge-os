#pragma once

#include "drivers/driver_enums.hpp"
#include "register_utils.hpp"
#include "userlib/system_call_type.hpp"

namespace edge::userlib {

void change_priority(uint8_t new_priority);

/**
 * We call callbacks by modifying lr and psp
 * Consider what happens if we *don't* inline this function
 * After the SVC call, it will restore the stack frame and b lr
 * We don't want this, because we want to directly jump to the callback
 *
 * In other words, this must be inlined
 */
__attribute__((always_inline)) inline void yield()
{
    TRIGGER_SVC(SystemCallType::YIELD);
}

void set_led(uint8_t row, uint8_t col, bool enabled);

int get_time_us();

bool get_button_pressed(drivers::ButtonType button_type);

void get_button_pressed(drivers::ButtonType button_type, void* callback);

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
