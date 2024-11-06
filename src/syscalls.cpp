#include "userlib/syscalls.hpp"

#include "drivers/driver_enums.hpp"
#include "register_utils.hpp"
#include "userlib/system_call_type.hpp"

#include <stdio.h>

// Compiler should remove this. If not, whatever. lol
// I think required to avoid messy/unnecessary cleanup at end of func
#define RETURN_REGISTER(reg)                                                           \
    int ret;                                                                           \
    READ_REGISTER(reg, ret);                                                           \
    return ret;

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

void get_button_pressed(
    drivers::ButtonType button_type, void (*callback)(drivers::ButtonType)
)
{
    SET_REGISTER(r0, (int)drivers::DriverSubscribe::NOTIFY_BUTTON_PRESS);
    SET_REGISTER(r1, (int)callback);
    SET_REGISTER(r2, (int)button_type);
    TRIGGER_SVC(SystemCallType::SUBSCRIBE);
}

void set_led(uint8_t row, uint8_t col, bool enabled)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::LED_DISPLAY);
    SET_REGISTER(r1, (int)row);
    SET_REGISTER(r2, (int)col);
    SET_REGISTER(r3, (int)enabled);
    TRIGGER_SVC(SystemCallType::COMMAND);
}

bool get_button_pressed(drivers::ButtonType button_type)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::BUTTONS);
    SET_REGISTER(r1, button_type);
    TRIGGER_SVC(SystemCallType::COMMAND);
    RETURN_REGISTER(r0);
}

int get_time_us()
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::GET_TIME);
    TRIGGER_SVC(SystemCallType::COMMAND);
    RETURN_REGISTER(r0);
}

void debug_print(const char* val)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::TERMINAL_OUTPUT);
    SET_REGISTER(r1, (int)val);
    TRIGGER_SVC(SystemCallType::COMMAND);
}
} // namespace edge::userlib
