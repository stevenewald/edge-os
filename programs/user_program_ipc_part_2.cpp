#include "userlib/syscalls.hpp"

void ipc_part2(void)
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
