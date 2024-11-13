#include "drivers/driver_enums.hpp"
#include "error_handler.hpp"
#include "scheduler.hpp"
#include "userlib/syscalls.hpp"
#include "util.hpp"

#include <stdbool.h>
#include <stdio.h>

void task1(void)
{
    auto trigger_faults = []() { asm volatile(".word 0xFFFFFFFF"); };

    using namespace edge::userlib;
    using namespace edge::drivers;

    static void (*fault_handler)(edge::FaultType) = [](edge::FaultType type) {
        switch (type) {
            case edge::FaultType::Usage:
                debug_println(etl::string<25>{"USAGE FAULT TRIGGERED"});
        }
    };

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

    set_fault_handler(fault_handler);

    trigger_faults();

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

    edge::ErrorHandler::get();

    edge::scheduler.add_task(task0);
    edge::scheduler.add_task(task1);

    edge::scheduler.start_scheduler();
}
