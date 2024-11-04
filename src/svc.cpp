#include "drivers/driver_controller.hpp"
#include "drivers/driver_enums.hpp"
#include "scheduler.hpp"
#include "userlib/system_call_type.hpp"

#include <cstdio>

namespace edge {

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
    return drivers::handle_subscribe(
        type, (void*)stack_ptr[1], stack_ptr[2], stack_ptr[3],
        scheduler.get_current_task()
    );
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
