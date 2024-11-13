#include "error_handler.hpp"

namespace {
extern "C" {
void __attribute__((used, naked)) UsageFault_Handler(void)
{
    asm volatile("TST LR, #4                        \n"
                 "ITE EQ                            \n"
                 "MRSEQ R0, MSP                     \n"
                 "MRSNE R0, PSP                     \n"
                 "B UsageFault_Handler_C            \n");
}

void UsageFault_Handler_C(uint32_t* stack_ptr)
{
    using edge::ErrorHandler;
    ErrorHandler::get().fault_triggered(edge::FaultType::Usage, stack_ptr);
}
}
} // namespace
