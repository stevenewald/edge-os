#include "drivers/driver_enums.hpp"
#include "fault_handler.hpp"
#include "scheduler.hpp"
#include "userlib/syscalls.hpp"
#include "util.hpp"

#include <stdbool.h>
#include <stdio.h>

void exception_task(void)
{
    auto trigger_faults = []() {
        // Usage
        asm volatile(".word 0xFFFFFFFF");

        // Bus
        volatile uint32_t* invalid_address = (uint32_t*)0xFFFFFFF0;
        [[maybe_unused]] uint32_t value = *invalid_address;
    };

    using namespace edge::userlib;
    static void (*fault_handler)(edge::FaultType) = [](edge::FaultType type) {
        switch (type) {
            case edge::FaultType::Usage:
                debug_println(etl::string<25>{"USAGE FAULT TRIGGERED"});
                break;
            case edge::FaultType::Bus:
                debug_println(etl::string<25>{"BUS FAULT TRIGGERED"});
                break;
            case edge::FaultType::Memory:
                debug_println(etl::string<25>{"MEMORY FAULT TRIGGERED"});
                break;
        }
    };

    set_fault_handler(fault_handler);

    trigger_faults();

    set_led(2, 2, true);

    while (1) {
        yield();
    }
}

void task1(void)
{
    using namespace edge::userlib;
    using namespace edge::drivers;

    static void (*on_button_press)(ButtonType, ButtonState) = [](ButtonType type,
                                                                 ButtonState state) {
        if (state == ButtonState::DOWN) {
            if (type == ButtonType::A)
                send_ipc("LED_DISPLAY", true);
            else
                send_ipc("LED_DISPLAY", false);
        }
    };

    get_button_pressed(ButtonType::A, on_button_press);
    get_button_pressed(ButtonType::B, on_button_press);

    while (1) {
        yield();
    }
}

void task0(void)
{
    using namespace edge::userlib;

    static bool flipped = false;
    static void (*ipc_callback)(int) = [](int value) { flipped = value; };

    subscribe_ipc("LED_DISPLAY", ipc_callback);

    while (1) {
        if (flipped) {
            set_led(4, 0, true);
            yield();
            set_led(4, 0, false);
        }
        else {
            set_led(0, 4, true);
            yield();
            set_led(0, 4, false);
        }
    }
}

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::FaultHandler::get();

    edge::scheduler.add_task(exception_task);
    edge::scheduler.add_task(task0);
    edge::scheduler.add_task(task1);

    edge::scheduler.start_scheduler();
}
