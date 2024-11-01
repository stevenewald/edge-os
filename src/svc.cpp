#include "drivers/driver_controller.hpp"
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

void handle_driver_command(uint32_t* stack_ptr)
{
    auto type = static_cast<drivers::DriverType>(stack_ptr[0]);
    drivers::driver_controller.handle_command(
        type, stack_ptr[1], stack_ptr[2], stack_ptr[3]
    );
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
    auto call_type = static_cast<SystemCallType>(((char*)SP_reg[6])[-2]);
    switch (call_type) {
        case SystemCallType::CHANGE_PRIORITY:
            handle_change_priority(SP_reg);
            break;
        case SystemCallType::YIELD:
            handle_yield();
            break;
        case SystemCallType::COMMAND:
            handle_driver_command(SP_reg);
            break;
    }
}
}
} // namespace edge
