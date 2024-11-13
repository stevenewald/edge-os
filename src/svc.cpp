#include "drivers/driver_commands.hpp"
#include "drivers/driver_enums.hpp"
#include "error_handler.hpp"
#include "ipc/ipc_command_types.hpp"
#include "ipc/ipc_manager.hpp"
#include "scheduler.hpp"
#include "userlib/system_call_type.hpp"
#include "util.hpp"

#include <cstdio>

namespace edge {

// TODO: replace uint32_t* stack_ptr with an actual structure

void handle_yield()
{
    scheduler.yield_current_task();
}

void handle_change_priority(uint32_t* stack_ptr)
{
    scheduler.change_current_task_priority(stack_ptr[0]);
}

etl::optional<int> handle_driver_command(uint32_t* stack_ptr)
{
    auto type = static_cast<drivers::DriverCommand>(stack_ptr[0]);
    return drivers::handle_command(type, stack_ptr[1], stack_ptr[2], stack_ptr[3]);
}

void handle_driver_subscribe(uint32_t* stack_ptr)
{
    auto type = static_cast<drivers::DriverSubscribe>(stack_ptr[0]);
    auto callback = reinterpret_cast<ProcessCallbackPtr>(stack_ptr[1]);
    int arg1 = stack_ptr[2];
    int arg2 = stack_ptr[3];
    return drivers::handle_subscribe(
        type, callback, arg1, arg2, scheduler.get_current_task()
    );
}

void handle_set_fault_handler(uint32_t* stack_ptr)
{
    uint32_t current_task_id = scheduler.get_current_task();
    auto callback = reinterpret_cast<ProcessCallbackPtr>(stack_ptr[0]);
    ErrorHandler::get().set_fault_callback(current_task_id, callback);
}

void handle_ipc(uint32_t* stack_ptr)
{
    auto type = static_cast<IPCCommandType>(stack_ptr[0]);
    auto name = reinterpret_cast<const char*>(stack_ptr[1]);
    etl::string<20> name_str{name};

    if (type == IPCCommandType::SEND) [[likely]] {
        auto message = static_cast<int>(stack_ptr[2]);
        IPCManager::get().send_message(name_str, message);
    }
    else if (type == IPCCommandType::REGISTER) {
        auto callback = reinterpret_cast<ProcessCallbackPtr>(stack_ptr[2]);
        IPCManager::get().register_callback(
            scheduler.get_current_task(), name_str, callback
        );
    }
    else {
        panic("Unknown IPC command type received");
    }
}

etl::optional<int> handle_call(uint32_t* stack_ptr)
{
    auto call_type = static_cast<SystemCallType>(((char*)stack_ptr[6])[-2]);
    switch (call_type) {
        case SystemCallType::CHANGE_PRIORITY:
            handle_change_priority(stack_ptr);
            break;
        case SystemCallType::YIELD:
            handle_yield();
            break;
        case SystemCallType::COMMAND:
            return handle_driver_command(stack_ptr);
        case SystemCallType::SUBSCRIBE:
            handle_driver_subscribe(stack_ptr);
            break;
        case edge::SystemCallType::IPC:
            handle_ipc(stack_ptr);
            break;
        case edge::SystemCallType::SET_FAULT_HANDLER:
            handle_set_fault_handler(stack_ptr);
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
    uint32_t* SP_reg;
    asm("MRS %0,PSP" : "=r"(SP_reg));
    auto ret_opt = handle_call(SP_reg);
    if (ret_opt) {
        SP_reg[0] = *ret_opt;
    }
}
}
} // namespace edge
