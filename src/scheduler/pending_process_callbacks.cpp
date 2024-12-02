#include "scheduler/pending_process_callbacks.hpp"

#include <stdio.h>

namespace edge {

PendingProcessCallbacks& PendingProcessCallbacks::get()
{
    static PendingProcessCallbacks pending_process_callbacks;
    return pending_process_callbacks;
}

void PendingProcessCallbacks::add_ready_callback(
    uint8_t process_id, ProcessCallbackPtr callback, int arg1, int arg2
)
{
    printf("[begin] callbacks in p0: %d\n", ready_callbacks[0].size());
    if (ready_callbacks[process_id].size() == MAX_READY_CALLBACKS) {
        printf("Process %d is OUT OF SPACE FOR CALLBACKS\n", process_id);
        return;
    }
    ready_callbacks[process_id].emplace_back(callback, arg1, arg2);
    printf("[end] callbacks in p0: %d\n", ready_callbacks[0].size());
}

etl::optional<drivers::subscribe_callback>
PendingProcessCallbacks::get_ready_callback(uint8_t process_id)
{
    if (ready_callbacks[process_id].empty()) {
        return etl::nullopt;
    }

    auto return_value = ready_callbacks[process_id].back();
    ready_callbacks[process_id].pop_back();
    return return_value;
}
} // namespace edge
