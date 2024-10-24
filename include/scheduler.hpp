#pragma once

#include "nrf52833.h"

#include <stdio.h>

static constexpr auto MAX_PROCESSES = 10;
static constexpr size_t STACK_SIZE_BYTES = 2048;
static constexpr size_t QUANTUM_MILLIS = 10;

namespace edge {
extern "C" {
void PendSV_Handler();
void SVC_Handler();
}

class Scheduler {
    unsigned current_task_index = 0;
    uint8_t slices_remaining = 1;

    struct process_metadata {
        const unsigned RESERVED1[5]{};
        const unsigned RETURN_ADDR;
        const unsigned FLAG;
        const unsigned RESERVED2[2]{};

        process_metadata(unsigned return_addr, unsigned flag = 0x01000000) :
            RETURN_ADDR(return_addr), FLAG(flag)
        {}
    };

    struct task {
        static constexpr size_t STACK_SIZE_IN_UNSIGNED =
            STACK_SIZE_BYTES / sizeof(unsigned);

        // ===== DO NOT REARRANGE THESE =====
        etl::array<unsigned, STACK_SIZE_IN_UNSIGNED> stack{};
        process_metadata METADATA;
        // ==================================

        unsigned* stack_ptr_loc{&stack.back()};
        uint8_t priority;

        task(const process_metadata& metadata, uint8_t initial_priority) :
            METADATA(metadata), priority(initial_priority)
        {}
    };

    etl::vector<task, MAX_PROCESSES> task_stack{};

public:
    void add_task(void (*function)(void), uint8_t priority = 1);

    void start_scheduler();

    void print_info() const;

private:
    bool continue_current_task();

    void advance_turn();

    void handle_first_svc_hit();

    void handle_priority_change();

    friend void PendSV_Handler(void);
    friend void SVC_Handler(void);
};

extern Scheduler scheduler;

} // namespace edge
