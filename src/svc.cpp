#include "register_utils.hpp"
#include "scheduler.hpp"
#include "system_call_type.hpp"

#include <cstdio>

namespace edge {
void
handle_priority_change()
{
    unsigned int new_priority;
    READ_REGISTER(r0, new_priority);
    scheduler.change_current_task_priority(new_priority);
}

void
handle_yield()
{
    scheduler.yield_current_task();
}

extern "C" {
__attribute__((used)) void
SVC_Handler(void)
{
    static bool has_hit = false;
    if (!has_hit) {
        scheduler.handle_first_svc_hit();
        has_hit = true;
    }
    uint32_t* SP_reg;
    asm("MRS %0,PSP" : "=r"(SP_reg));
    auto call_type = static_cast<SystemCallType>(((char*)SP_reg[6])[-2]);
    switch (call_type) {
        case SystemCallType::CHANGE_PRIORITY:
            handle_priority_change();
            break;
        case SystemCallType::YIELD:
            handle_yield();
            break;
    }
}
}
} // namespace edge
