#pragma once

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

enum class DriverSubscribe { NOTIFY_BUTTON_PRESS = 0, TIMER_START = 1 };

enum class GPIOConfiguration { OUT, IN_NORES, IN_PDR, IN_PUR };

enum class ButtonType { A = 0, B = 1 };
enum class ButtonState { UP = 0, DOWN = 1 };

struct subscribe_callback {
    ProcessCallbackPtr callback;
    int arg1;
    int arg2;
};

} // namespace edge::drivers
