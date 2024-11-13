#include "error_handler.hpp"

#include "nrf52.h"
#include "pending_process_callbacks.hpp"
#include "scheduler.hpp"

#include <stdio.h>

namespace edge {
namespace {
uint32_t increment_pc(uint32_t program_counter)
{
    bool thumb_mode = program_counter & 0x1;
    if (thumb_mode)
        return program_counter + 2;
    else
        return program_counter + 4;
}
} // namespace

ErrorHandler::ErrorHandler()
{
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
}

ErrorHandler::~ErrorHandler()
{
    SCB->SHCSR &= ~SCB_SHCSR_USGFAULTENA_Msk;
}

ErrorHandler& ErrorHandler::get()
{
    static ErrorHandler error_handler;
    return error_handler;
}

void ErrorHandler::set_fault_callback(
    uint8_t process_id, ProcessCallbackPtr callback_ptr
)
{
    if (error_callbacks[process_id]) {
        printf("Process %d already set callback for fault handling\n", process_id);
        return;
    }
    error_callbacks[process_id] = callback_ptr;
}

void ErrorHandler::fault_triggered(FaultType fault_type, uint32_t* stack_ptr)
{
    uint8_t current_task = scheduler.get_current_task();
    if (!error_callbacks[current_task].has_value()) [[unlikely]] {
        panic("Error handler not set");
    }

    stack_ptr[6] = increment_pc(stack_ptr[6]);
    PendingProcessCallbacks::get().add_ready_callback(
        current_task, error_callbacks[current_task].value()
    );
}
} // namespace edge
