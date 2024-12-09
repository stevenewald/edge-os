#pragma once

#include "config.hpp"
#include "util.hpp"

namespace edge {

class Task {
    static constexpr size_t STACK_SIZE_IN_UNSIGNED =
        (STACK_SIZE_BYTES / sizeof(unsigned)) - sizeof(exception_stack_registers)
        - sizeof(unsigned*) - sizeof(uint8_t);

public:
    // ===== DO NOT REARRANGE THESE =====
    // Align for MPU purposes
    alignas(STACK_SIZE_BYTES) etl::array<unsigned, STACK_SIZE_IN_UNSIGNED> stack{};
    exception_stack_registers first_stack_frame;
    // ==================================

    // needs to point to first_stack_frame
    unsigned* stack_ptr_loc{&first_stack_frame.R0};

    // This is useful if we want to adjust the ratio of driver to process runtime
    uint8_t consecutive_quantums_to_run;

    Task(
        const exception_stack_registers& initial_stack_frame, uint8_t initial_priority
    ) :
        first_stack_frame(initial_stack_frame),
        consecutive_quantums_to_run(initial_priority)
    {}
};

static_assert(sizeof(Task) <= STACK_SIZE_BYTES);
} // namespace edge
