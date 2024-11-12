#pragma once

#include "config.hpp"
#include "util.hpp"

namespace edge {
class IPCManager {
    ProcessCallbackPtr callback;

    etl::array<etl::optional<ProcessCallbackPtr>, MAX_PROCESSES> ipc_communicators;

    IPCManager() = default;
    ~IPCManager() = default;

public:
    IPCManager(const IPCManager&) = delete;
    IPCManager(IPCManager&&) = delete;
    IPCManager operator=(const IPCManager&) = delete;
    IPCManager operator=(IPCManager&&) = delete;
    static IPCManager& get();

    void register_callback(uint8_t process_id, ProcessCallbackPtr callback);
    void send_message(uint8_t destination_process_id, int value);
};

} // namespace edge
