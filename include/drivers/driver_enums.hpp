#pragma once

#include "hal/hal_enums.hpp"
#include "util.hpp"

namespace edge::drivers {
enum class DriverCommand {
    GET_TIME = 0,
    LED_DISPLAY = 1,
    BUTTONS = 2,
    TERMINAL_OUTPUT = 3,
    TIMER_CANCEL = 4,
    CAPTOUCH = 5,
};

enum class DriverSubscribe {
    NOTIFY_BUTTON_PRESS = 0,
    TIMER_START = 1,
    NOTIFY_CAPTOUCH = 2
};

enum class GPIOConfiguration { OUT, IN_NORES, IN_PDR, IN_PUR };

enum class ButtonType { A = 0, B = 1 };
enum class ButtonState { UP = 0, DOWN = 1 };

struct subscribe_callback {
    ProcessCallbackPtr callback;
    int arg1;
    int arg2;
};

aidan::PinPullMode to_pin_pull_mode(drivers::GPIOConfiguration config);

} // namespace edge::drivers
