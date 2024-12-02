#include "userlib/syscalls.hpp"
#include <cstdio>


void static callback(void)
{
    edge::userlib::set_led(3, 3, true);
}
void exception_task(void)
{
    /* auto trigger_faults = []() { */
    /*     // Usage */
    /*     asm volatile(".word 0xFFFFFFFF"); */
    /**/
    /*     // Memory */
    /*     volatile uint32_t* invalid_address = (uint32_t*)0xFFFFFFF0; */
    /*     [[maybe_unused]] uint32_t value = *invalid_address; */
    /* }; */
    /**/
    using namespace edge::userlib;
    static void (*fault_handler)(edge::FaultType) = [](edge::FaultType type) {
        switch (type) {
            case edge::FaultType::Usage:
                debug_print("USAGE FAULT TRIGGERED\n");
                break;
            case edge::FaultType::Bus:
                debug_print("BUS FAULT TRIGGERED\n");
                break;
            case edge::FaultType::Memory:
                debug_print("MEMORY FAULT TRIGGERED\n");
                break;
        }
    };

    set_fault_handler(fault_handler);

    /* trigger_faults(); */
    start_vtimer(&callback);

    set_led(2, 2, true);

    while (1) {
        yield();
    }
}
