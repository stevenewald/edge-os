#pragma once
#include "config.hpp"
#include "task.hpp"
#include "util.hpp"

#include <stdio.h>

#include <cstdint>

static constexpr size_t QUANTUM_MILLIS = 5;

namespace edge {
extern "C" {
void PendSV_Handler();
void SVC_Handler();
}

class Scheduler {
    unsigned current_task_index = 0;
    uint8_t slices_remaining = 1;

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
