#pragma once
#include "config.hpp"
#include "util.hpp"

#include <stdio.h>

#include <cstdint>

static constexpr size_t STACK_SIZE_BYTES = 2048;
static constexpr size_t QUANTUM_MILLIS = 5;

namespace edge {
extern "C" {
void PendSV_Handler();
void SVC_Handler();
}

class Scheduler {
    unsigned current_task_index = 0;
    uint8_t slices_remaining = 1;

    class Task {
        static constexpr size_t STACK_SIZE_IN_UNSIGNED =
            STACK_SIZE_BYTES / sizeof(unsigned);

        // ===== DO NOT REARRANGE THESE =====
        etl::array<unsigned, STACK_SIZE_IN_UNSIGNED> stack{};
        exception_stack_registers first_stack_frame;
        // ==================================

    public:
        // needs to point to first_stack_frame
        unsigned* stack_ptr_loc{&first_stack_frame.R0};

        // This is useful if we want to adjust the ratio of driver to process runtime
        uint8_t consecutive_quantums_to_run;

        Task(
            const exception_stack_registers& initial_stack_frame,
            uint8_t initial_priority
        ) :
            first_stack_frame(initial_stack_frame),
            consecutive_quantums_to_run(initial_priority)
        {}
    };

    etl::vector<Task, MAX_PROCESSES> task_stack{};

public:
    unsigned get_current_task() const { return current_task_index; }

    void add_task(void (*function)(void), uint8_t priority = 1);

    void start_scheduler();

    void change_current_task_priority(uint8_t new_priority);

    void yield_current_task();

private:
    void handle_first_svc_hit();

    friend void PendSV_Handler(void);
    friend void SVC_Handler(void);
};

extern Scheduler scheduler;

} // namespace edge
