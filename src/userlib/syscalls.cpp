#include "userlib/syscalls.hpp"

#include "drivers/driver_enums.hpp"
#include "ipc/ipc_command_types.hpp"
#include "userlib/system_call_type.hpp"
#include "util.hpp"

#include <stdio.h>

#define TRIGGER_SVC(call_type)   asm volatile("svc %0" ::"I"(call_type))
#define SET_REGISTER(reg, value) asm volatile("mov " #reg ", %0" ::"r"(value) : #reg)
#define READ_REGISTER(reg, var)  asm volatile("mov %0, " #reg : "=r"(var))

#define RETURN_REGISTER(reg)                                                           \
    int ret;                                                                           \
    READ_REGISTER(reg, ret);                                                           \
    return ret;

namespace edge::userlib {
void USER_CODE change_priority(uint8_t new_priority)
{
    SET_REGISTER(r0, new_priority);
    TRIGGER_SVC(SystemCallType::CHANGE_PRIORITY);
}

void USER_CODE yield()
{
    TRIGGER_SVC(SystemCallType::YIELD);
}

void USER_CODE set_fault_handler(void (*callback)(FaultType))
{
    SET_REGISTER(r0, callback);
    TRIGGER_SVC(SystemCallType::SET_FAULT_HANDLER);
}

void USER_CODE send_ipc(const char* name, uint32_t message)
{
    SET_REGISTER(r0, IPCCommandType::SEND);
    SET_REGISTER(r1, name);
    SET_REGISTER(r2, message);
    TRIGGER_SVC(SystemCallType::IPC);
}

void USER_CODE subscribe_ipc(const char* name, void (*callback)(int message))
{
    SET_REGISTER(r0, IPCCommandType::REGISTER);
    SET_REGISTER(r1, name);
    SET_REGISTER(r2, callback);
    TRIGGER_SVC(SystemCallType::IPC);
}

void USER_CODE get_button_pressed(
    drivers::ButtonType button_type,
    void (*callback)(drivers::ButtonType, drivers::ButtonState)
)
{
    SET_REGISTER(r0, (int)drivers::DriverSubscribe::NOTIFY_BUTTON_PRESS);
    SET_REGISTER(r1, (int)callback);
    SET_REGISTER(r2, (int)button_type);
    TRIGGER_SVC(SystemCallType::SUBSCRIBE);
}

void USER_CODE set_led(uint8_t row, uint8_t col, bool enabled)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::LED_DISPLAY);
    SET_REGISTER(r1, (int)row);
    SET_REGISTER(r2, (int)col);
    SET_REGISTER(r3, (int)enabled);
    TRIGGER_SVC(SystemCallType::COMMAND);
}

bool USER_CODE get_button_pressed(drivers::ButtonType button_type)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::BUTTONS);
    SET_REGISTER(r1, button_type);
    TRIGGER_SVC(SystemCallType::COMMAND);
    RETURN_REGISTER(r0);
}

int USER_CODE get_time_us()
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::GET_TIME);
    TRIGGER_SVC(SystemCallType::COMMAND);
    RETURN_REGISTER(r0);
}

uint32_t USER_CODE
start_timer(void (*callback)(uint32_t), uint32_t microseconds, bool periodic)
{
    SET_REGISTER(r0, (int)drivers::DriverSubscribe::TIMER_START);
    SET_REGISTER(r1, (int)callback);
    SET_REGISTER(r2, (int)microseconds);
    SET_REGISTER(r3, (int)periodic);
    TRIGGER_SVC(SystemCallType::SUBSCRIBE);
    RETURN_REGISTER(r0);
}

void USER_CODE cancel_timer(uint32_t timer_id)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::TIMER_CANCEL);
    SET_REGISTER(r1, (int)timer_id);
    TRIGGER_SVC(SystemCallType::COMMAND);
}

void USER_CODE set_pin(uint32_t pin)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::SET_PIN);
    SET_REGISTER(r1, (int)pin);
    TRIGGER_SVC(SystemCallType::COMMAND);
}

void USER_CODE clear_pin(uint32_t pin)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::CLEAR_PIN);
    SET_REGISTER(r1, (int)pin);
    TRIGGER_SVC(SystemCallType::COMMAND);
}

void USER_CODE debug_print(const char* val)
{
    SET_REGISTER(r0, (int)drivers::DriverCommand::TERMINAL_OUTPUT);
    SET_REGISTER(r1, (int)val);
    TRIGGER_SVC(SystemCallType::COMMAND);
}
} // namespace edge::userlib
