#pragma once

namespace edge::userlib {

void change_priority(uint8_t new_priority);

void yield();

void set_led(uint8_t row, uint8_t col, bool enabled);

} // namespace edge::userlib
