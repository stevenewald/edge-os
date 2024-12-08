#include "userlib/syscalls.hpp"

#include <cstdio>

void callback(uint32_t id)
{
    static int b = 0;
    edge::userlib::set_led(3, 3, ++b % 2 == 0);
}

void callback2(uint32_t id)
{
    static int b = 0;
    edge::userlib::set_led(1, 1, ++b % 2 == 0);
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
    // start_timer(callback, 100000, true);
    // auto id2 = start_timer(callback2, 750000, true);
    // cancel_timer(id2);

    set_led(2, 2, true);

    while (1) {
        yield();
    }
}
