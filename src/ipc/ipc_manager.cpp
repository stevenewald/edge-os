#include "ipc/ipc_manager.hpp"

#include "util.hpp"

#include <stdio.h>

namespace edge {
IPCManager& IPCManager::get()
{
    static IPCManager ipc_manager;
    return ipc_manager;
}

void IPCManager::register_callback(
    ProcessId process_id, const ProcessName& new_process_name,
    ProcessCallbackPtr callback
)
{
    ipc_communicators.set_callback(process_id, callback);
    name_to_id[new_process_name] = process_id;
}

void IPCManager::send_message(const ProcessName& destination_name, int value)
{
    if (name_to_id.find(destination_name) == name_to_id.end()) [[unlikely]] {
        printf("IPC sent but no process matched name %s\n", destination_name.data());
        return;
    }

    auto process_id = name_to_id[destination_name];
    ipc_communicators.call_callback(process_id, value);
}
} // namespace edge
