#pragma once

#include "config.hpp"
#include "scheduler/user_callback_storage.hpp"
#include "util.hpp"

namespace edge {
class IPCManager {
    etl::unordered_map<ProcessName, uint8_t, MAX_PROCESSES> name_to_id;
    UserCallbackStorage ipc_communicators{};

    IPCManager() = default;
    ~IPCManager() = default;

public:
    IPCManager(const IPCManager&) = delete;
    IPCManager(IPCManager&&) = delete;
    IPCManager operator=(const IPCManager&) = delete;
    IPCManager operator=(IPCManager&&) = delete;
    static IPCManager& get();

    void register_callback(
        uint8_t process_id, const ProcessName& process_name, ProcessCallbackPtr callback
    );
    void send_message(const ProcessName& destination_name, int value);
};

} // namespace edge
