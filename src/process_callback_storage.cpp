#include "process_callback_storage.hpp"

#include <stdio.h>

namespace edge {

ProcessCallbackStorage& ProcessCallbackStorage::get()
{
    static ProcessCallbackStorage process_callback_storage;
    return process_callback_storage;
}

void ProcessCallbackStorage::add_ready_callback(
    uint8_t process_id, drivers::SubscribeCallbackPtr callback, int arg1, int arg2
)
{
    if (ready_callbacks[process_id].size() == MAX_READY_CALLBACKS) {
        printf("Process %d is OUT OF SPACE FOR CALLBACKS\n", process_id);
        return;
    }
    ready_callbacks[process_id].emplace_back(callback, arg1, arg2);
}

etl::optional<drivers::subscribe_callback>
ProcessCallbackStorage::get_ready_callback(uint8_t process_id)
{
    if (ready_callbacks[process_id].empty()) {
        return etl::nullopt;
    }

    auto return_value = ready_callbacks[process_id].back();
    ready_callbacks[process_id].pop_back();
    return return_value;
}
} // namespace edge
