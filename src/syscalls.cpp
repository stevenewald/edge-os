#include "userlib/syscalls.hpp"

#include "drivers/driver_types.hpp"
#include "register_utils.hpp"
#include "userlib/system_call_type.hpp"

#include <stdio.h>

namespace edge::userlib {
void change_priority(uint8_t new_priority)
{
    SET_REGISTER(r0, new_priority);
    TRIGGER_SVC(SystemCallType::CHANGE_PRIORITY);
}

void yield()
{
    TRIGGER_SVC(SystemCallType::YIELD);
}

void set_led(uint8_t row, uint8_t col, bool enabled)
{
    SET_REGISTER(r0, (int)drivers::DriverType::LED_DISPLAY);
    SET_REGISTER(r1, (int)row);
    SET_REGISTER(r2, (int)col);
    SET_REGISTER(r3, (int)enabled);
    TRIGGER_SVC(SystemCallType::COMMAND);
}

int get_time()
{
    SET_REGISTER(r0, (int)drivers::DriverType::GET_TIME);
    TRIGGER_SVC(SystemCallType::COMMAND);
    int ret;
    READ_REGISTER(r0, ret);
    return ret;
}

void debug_print(const char* val)
{
    SET_REGISTER(r0, (int)drivers::DriverType::TERMINAL_OUTPUT);
    SET_REGISTER(r1, (int)val);
    TRIGGER_SVC(SystemCallType::COMMAND);
}
} // namespace edge::userlib
