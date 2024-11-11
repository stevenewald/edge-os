#include "nrf52833.h"
#include "userlib/syscalls.hpp"

template <int N>
void task(void)
{
    using namespace edge::userlib;
    using namespace edge::drivers;
    change_priority(1);

    static bool flipped = false;
    static void (*on_button_press)(ButtonType, ButtonState) = [](ButtonType type,
                                                                 ButtonState state) {
        if (state == ButtonState::DOWN) {
            if (type == ButtonType::A)
                flipped = true;
            else
                flipped = false;
        }
        // SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    };

    get_button_pressed(ButtonType::A, on_button_press);
    get_button_pressed(ButtonType::B, on_button_press);

    while (1) {
        if (flipped) {
            set_led(N, 0, true);
            yield();
            set_led(N, 0, false);
        }
        else {
            set_led(0, N, true);
            yield();
            set_led(0, N, false);
        }
    }
}

template void task<0>(void);
template void task<1>(void);
template void task<2>(void);
template void task<3>(void);
template void task<4>(void);
