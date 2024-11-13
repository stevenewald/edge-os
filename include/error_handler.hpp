#pragma once

#include "config.hpp"
#include "util.hpp"

namespace edge {

class ErrorHandler {
    ErrorHandler();
    ~ErrorHandler();

    etl::array<etl::optional<ProcessCallbackPtr>, MAX_PROCESSES> error_callbacks;

public:
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler(ErrorHandler&&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    ErrorHandler& operator=(ErrorHandler&&) = delete;

    static ErrorHandler& get();

    // To be called by raw handlers in raw_fault_handling.cpp
    void fault_triggered(FaultType fault_type, uint32_t* stack_ptr);

    void set_fault_callback(uint8_t process_id, ProcessCallbackPtr callback_ptr);
};
} // namespace edge
