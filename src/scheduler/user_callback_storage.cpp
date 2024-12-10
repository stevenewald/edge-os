#include "scheduler/user_callback_storage.hpp"

#include "scheduler/pending_process_callbacks.hpp"

namespace edge {
bool UserCallbackStorage::has_callback(ProcessId id)
{
    return callbacks_.find(id) != callbacks_.end();
}

void UserCallbackStorage::set_callback(ProcessId id, ProcessCallbackPtr ptr)
{
    if (has_callback(id)) [[unlikely]] {
        panic("Attempted to set callback when callback already exists");
    }

    callbacks_[id] = ptr;
}

void UserCallbackStorage::call_callback(ProcessId id, int arg1, int arg2)
{
    if (!has_callback(id)) {
        printf(
            "Attempted to call callback when it has not been set for process %d\n", id
        );
    }

    PendingProcessCallbacks::get().add_ready_callback(id, callbacks_[id], arg1, arg2);
}
} // namespace edge
