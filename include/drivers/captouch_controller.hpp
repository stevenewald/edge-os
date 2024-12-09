#pragma once

#include "drivers/driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpio_pin_event.hpp"
#include "microbit_v2.h"
#include "nrf_gpio.h"
#include "scheduler/user_callback_storage.hpp"
#include "util.hpp"

namespace edge::drivers {

class CapsenseController {
    GPIOPin touch_logo{TOUCH_LOGO, GPIOConfiguration::IN_NORES};

    UserCallbackStorage subscriptions;

    bool touched;

    bool prev_touched;

    uint32_t time_test_started;

    GPIOPinEvent event;

    CapsenseController();
    ~CapsenseController() = default;

public:
    CapsenseController(const CapsenseController&) = delete;
    CapsenseController(CapsenseController&&) = delete;
    CapsenseController& operator=(const CapsenseController&) = delete;
    CapsenseController& operator=(CapsenseController&&) = delete;

    static CapsenseController& get();

    void subscribe_captouch_press(ProcessCallbackPtr callback, uint8_t process_id);

    bool get_captouch_pressed();

    void handle_gpio_interrupt(nrf_gpio_pin_sense_t sense, int pin);

    void start_capacitive_test();
};

} // namespace edge::drivers
