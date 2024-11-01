#include "syscalls.hpp"

#include "register_utils.hpp"
#include "system_call_type.hpp"

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
} // namespace edge::userlib
