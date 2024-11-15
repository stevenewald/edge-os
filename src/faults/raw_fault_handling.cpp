#include "faults/fault_handler.hpp"
#include "nrf52833.h"

#include <stdio.h>

namespace {
extern "C" {
void __attribute__((used, naked)) UsageFault_Handler(void)
{
    asm volatile("TST LR, #4                        \n"
                 "ITE EQ                            \n"
                 "MRSEQ R0, MSP                     \n"
                 "MRSNE R0, PSP                     \n"
                 "B UsageFault_Handler_CPP            \n");
}

void UsageFault_Handler_CPP(uint32_t* stack_ptr)
{
    using edge::FaultHandler;
    FaultHandler::get().fault_triggered(edge::FaultType::Usage, stack_ptr);
}

void __attribute__((used, naked)) BusFault_Handler(void)
{
    asm volatile("TST LR, #4                        \n"
                 "ITE EQ                            \n"
                 "MRSEQ R0, MSP                     \n"
                 "MRSNE R0, PSP                     \n"
                 "B BusFault_Handler_CPP            \n");
}

void BusFault_Handler_CPP(uint32_t* stack_ptr)
{
    using edge::FaultHandler;
    FaultHandler::get().fault_triggered(edge::FaultType::Bus, stack_ptr);
}

void __attribute__((used, naked)) MemoryManagement_Handler(void)
{
    asm volatile("TST LR, #4                        \n"
                 "ITE EQ                            \n"
                 "MRSEQ R0, MSP                     \n"
                 "MRSNE R0, PSP                     \n"
                 "B MemManage_Handler_CPP            \n");
}

void MemManage_Handler_CPP(uint32_t* stack_ptr)
{
    using edge::FaultHandler;
    FaultHandler::get().fault_triggered(edge::FaultType::Memory, stack_ptr);
}
}
} // namespace
