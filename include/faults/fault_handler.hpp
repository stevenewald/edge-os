#pragma once

#include "config.hpp"
#include "scheduler/user_callback_storage.hpp"
#include "util.hpp"

namespace edge {

class FaultHandler {
    FaultHandler();
    ~FaultHandler();

    UserCallbackStorage error_callbacks;

public:
    FaultHandler(const FaultHandler&) = delete;
    FaultHandler(FaultHandler&&) = delete;
    FaultHandler& operator=(const FaultHandler&) = delete;
    FaultHandler& operator=(FaultHandler&&) = delete;

    static FaultHandler& get();

    // To be called by raw handlers in raw_fault_handling.cpp
    void fault_triggered(FaultType fault_type, uint32_t* stack_ptr);

    void set_fault_callback(uint8_t process_id, ProcessCallbackPtr callback_ptr);
};
} // namespace edge
