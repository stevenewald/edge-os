#include "ipc/ipc_manager.hpp"

#include "process_callback_storage.hpp"
#include "util.hpp"

#include <stdio.h>

namespace edge {
IPCManager& IPCManager::get()
{
    static IPCManager ipc_manager;
    return ipc_manager;
}

void IPCManager::register_callback(
    uint8_t process_id, const ProcessName& new_process_name, ProcessCallbackPtr callback
)
{
    if (ipc_communicators[process_id] != nullptr) [[unlikely]] {
        panic("IPC registered twice for a process");
    }

    ipc_communicators[process_id] = callback;
    name_to_id[new_process_name] = process_id;
}

void IPCManager::send_message(const ProcessName& destination_name, int value)
{
    if (name_to_id.find(destination_name) == name_to_id.end()) [[unlikely]] {
        printf("IPC sent but no process matched name %s\n", destination_name.data());
        return;
    }

    auto process_id = name_to_id[destination_name];
    auto callback = ipc_communicators[process_id];
    ProcessCallbackStorage::get().add_ready_callback(process_id, callback, value);
    return;
}
} // namespace edge
