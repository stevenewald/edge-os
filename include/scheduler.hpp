#pragma once

#include "config.hpp"

#include <stdio.h>

#include <cstdint>

static constexpr size_t STACK_SIZE_BYTES = 2048;
static constexpr size_t QUANTUM_MILLIS = 5;

namespace edge {
extern "C" {
void PendSV_Handler();
void SVC_Handler();
void restore();
}

class Scheduler {
    unsigned current_task_index = 0;
    uint8_t slices_remaining = 1;

    struct saved_registers {
        const unsigned HARDWARE_REGS[5]{};
        const unsigned RETURN_ADDR;
        const unsigned FLAG;
        const unsigned SOFTWARE_REGS[8]{};

        saved_registers(unsigned return_addr, unsigned flag = 0x01000000) :
            RETURN_ADDR(return_addr), FLAG(flag)
        {}
    };

    struct task {
        static constexpr size_t STACK_SIZE_IN_UNSIGNED =
            STACK_SIZE_BYTES / sizeof(unsigned);

        // ===== DO NOT REARRANGE THESE =====
        etl::array<unsigned, STACK_SIZE_IN_UNSIGNED> stack{};
        saved_registers METADATA;
        // ==================================

        unsigned* stack_ptr_loc{&stack.back()};

        // This is useful if we want to adjust the ratio of driver to process runtime
        uint8_t consecutive_quantums_to_run;

        task(const saved_registers& metadata, uint8_t initial_priority) :
            METADATA(metadata), consecutive_quantums_to_run(initial_priority)
        {}
    };

    etl::vector<task, MAX_PROCESSES> task_stack{};

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
    friend void restore(void);
};

extern Scheduler scheduler;

} // namespace edge
