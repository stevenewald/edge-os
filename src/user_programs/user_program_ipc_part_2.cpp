#include "userlib/syscalls.hpp"

void ipc_part2(void)
{
    using namespace edge::userlib;
    using namespace edge::drivers;

    static void (*on_button_press)(ButtonType, ButtonState) = [](ButtonType type,
                                                                 ButtonState state) {
        if (state == ButtonState::DOWN) {
            if (type == ButtonType::A) {
                edge::userlib::debug_print("IPC RECEIVER PROGRAM: Button A pressed\n");
                send_ipc("LED_DISPLAY", true);
            }
            else {
                send_ipc("LED_DISPLAY", false);
                edge::userlib::debug_print("IPC RECEIVER PROGRAM: Button B pressed\n");
            }
        }
    };

    get_button_pressed(ButtonType::A, on_button_press);
    get_button_pressed(ButtonType::B, on_button_press);

    while (1) {
        yield();
    }
}
