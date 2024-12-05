#include "faults/fault_handler.hpp"

#include "nrf52.h"
#include "scheduler/pending_process_callbacks.hpp"
#include "scheduler/scheduler.hpp"

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

FaultHandler::FaultHandler()
{
    // Enable UsageFault
    // SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
    //
    // // Enable BusFault
    // SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
    //
    // // Enable MemoryFault
    // SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
}

FaultHandler::~FaultHandler()
{
    // Disable UsageFault
    SCB->SHCSR &= ~SCB_SHCSR_USGFAULTENA_Msk;

    // Disable BusFault
    SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;

    // Disable MemoryFault
    SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
}

FaultHandler& FaultHandler::get()
{
    static FaultHandler fault_handler;
    return fault_handler;
}

void FaultHandler::set_fault_callback(
    uint8_t process_id, ProcessCallbackPtr callback_ptr
)
{
    if (error_callbacks[process_id]) {
        printf("Process %d already set callback for fault handling\n", process_id);
        return;
    }
    error_callbacks[process_id] = callback_ptr;
}

void FaultHandler::fault_triggered(FaultType fault_type, uint32_t* stack_ptr)
{
    uint8_t current_task = scheduler.get_current_task();
    if (!error_callbacks[current_task].has_value()) [[unlikely]] {
        panic("Error handler not set");
    }

    stack_ptr[6] = increment_pc(stack_ptr[6]);
    PendingProcessCallbacks::get().add_ready_callback(
        current_task, error_callbacks[current_task].value(),
        static_cast<int>(fault_type)
    );
}
} // namespace edge
