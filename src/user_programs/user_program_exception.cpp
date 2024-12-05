#include "userlib/syscalls.hpp"

#include <cstdio>

void static callback(uint32_t id)
{
    edge::userlib::set_led(3, 3, true);
    edge::userlib::debug_print("Callback!\n");
}

void exception_task(void)
{
    auto trigger_faults = []() {
        // Usage
        asm volatile(".word 0xFFFFFFFF");

        // Memory
        volatile uint32_t* invalid_address = (uint32_t*)0xFFFFFFF0;
        [[maybe_unused]] uint32_t value = *invalid_address;
    };

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

    trigger_faults();
    auto id = start_timer(&callback, 1000000);
    // cancel_timer(id);
    id = start_timer(&callback, 2000000);

    set_led(2, 2, true);

    while (1) {
        yield();
    }
}
