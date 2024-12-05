#pragma once

#include "util.hpp"

namespace edge {

constexpr size_t MAX_CALLBACKS = 128;

class UserCallbackStorage {
    etl::unordered_map<ProcessId, ProcessCallbackPtr, MAX_CALLBACKS> callbacks_;

public:
    bool has_callback(ProcessId id);

    void set_callback(ProcessId id, ProcessCallbackPtr ptr);

    void call_callback(ProcessId id, int arg1 = 0, int arg2 = 0);
};
} // namespace edge
