#include "drivers/driver_commands.hpp"
#include "drivers/driver_enums.hpp"
#include "faults/fault_handler.hpp"
#include "ipc/ipc_command_types.hpp"
#include "ipc/ipc_manager.hpp"
#include "scheduler/scheduler.hpp"
#include "userlib/system_call_type.hpp"
#include "util.hpp"

#include <cstdio>

namespace edge {

void handle_yield()
{
    scheduler.yield_current_task();
}

void handle_change_priority(exception_stack_registers* stack_regs)
{
    scheduler.change_current_task_priority(stack_regs->R0);
}

etl::optional<int> handle_driver_command(exception_stack_registers* stack_regs)
{
    auto type = static_cast<drivers::DriverCommand>(stack_regs->R0);
    return drivers::handle_command(
        type, stack_regs->R1, stack_regs->R2, stack_regs->R3
    );
}

void handle_driver_subscribe(exception_stack_registers* stack_regs)
{
    auto type = static_cast<drivers::DriverSubscribe>(stack_regs->R0);
    auto callback = reinterpret_cast<ProcessCallbackPtr>(stack_regs->R1);
    int arg1 = stack_regs->R2;
    int arg2 = stack_regs->R3;
    return drivers::handle_subscribe(
        type, callback, arg1, arg2, scheduler.get_current_task()
    );
}

void handle_set_fault_handler(exception_stack_registers* stack_regs)
{
    uint32_t current_task_id = scheduler.get_current_task();
    auto callback = reinterpret_cast<ProcessCallbackPtr>(stack_regs->R0);
    FaultHandler::get().set_fault_callback(current_task_id, callback);
}

void handle_ipc(exception_stack_registers* stack_regs)
{
    auto type = static_cast<IPCCommandType>(stack_regs->R0);
    auto name = reinterpret_cast<const char*>(stack_regs->R1);
    etl::string<20> name_str{name};

    if (type == IPCCommandType::SEND) [[likely]] {
        auto message = static_cast<int>(stack_regs->R2);
        IPCManager::get().send_message(name_str, message);
    }
    else if (type == IPCCommandType::REGISTER) {
        auto callback = reinterpret_cast<ProcessCallbackPtr>(stack_regs->R2);
        IPCManager::get().register_callback(
            scheduler.get_current_task(), name_str, callback
        );
    }
    else {
        panic("Unknown IPC command type received");
    }
}

etl::optional<int> handle_call(exception_stack_registers* stack_regs)
{
    auto call_type =
        static_cast<SystemCallType>(((char*)(stack_regs->RETURN_ADDR))[-2]);
    switch (call_type) {
        case SystemCallType::CHANGE_PRIORITY:
            handle_change_priority(stack_regs);
            break;
        case SystemCallType::YIELD:
            handle_yield();
            break;
        case SystemCallType::COMMAND:
            return handle_driver_command(stack_regs);
        case SystemCallType::SUBSCRIBE:
            handle_driver_subscribe(stack_regs);
            break;
        case edge::SystemCallType::IPC:
            handle_ipc(stack_regs);
            break;
        case edge::SystemCallType::SET_FAULT_HANDLER:
            handle_set_fault_handler(stack_regs);
            break;
    }
    return etl::nullopt;
}

extern "C" {
__attribute__((used)) void SVC_Handler(void)
{
    static bool has_hit = false;
    if (!has_hit) {
        scheduler.handle_first_svc_hit();
        has_hit = true;
        return;
    }
    exception_stack_registers* stack_regs;
    asm("MRS %0,PSP" : "=r"(stack_regs));
    auto ret_opt = handle_call(stack_regs);
    if (ret_opt) {
        stack_regs->R0 = *ret_opt;
    }
}
}
} // namespace edge
