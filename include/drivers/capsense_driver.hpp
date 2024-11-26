#pragma once

#include "config.hpp"
#include "drivers/driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "drivers/gpio_pin_event.hpp"
#include "microbit_v2.h"
#include "app_timer.h"
#include "nrfx_timer.h"
#include "nrf_gpio.h"
#include "util.hpp"

namespace edge::drivers {

class CapsenseController {
    GPIOPin touch_logo{TOUCH_LOGO, GPIOConfiguration::IN_NORES};

    etl::array<ProcessCallbackPtr, MAX_PROCESSES> subscriptions;

    bool touched;

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

};


} // namespace edge::drivers
