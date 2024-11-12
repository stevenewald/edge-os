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

void IPCManager::register_callback(uint8_t process_id, ProcessCallbackPtr callback)
{
    ipc_communicators[process_id] = callback;
}

void IPCManager::send_message(uint8_t destination_process_id, int value)
{
    if (!ipc_communicators[destination_process_id]) {
        printf(
            "Sent message to process %d which has not been registered\n",
            destination_process_id
        );
        return;
    }

    ProcessCallbackStorage::get().add_ready_callback(
        destination_process_id, *ipc_communicators[destination_process_id], value
    );
}
} // namespace edge
