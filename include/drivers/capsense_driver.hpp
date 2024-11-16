# pragma once

#include "config.hpp"
#include "drivers/driver_enums.hpp"
#include "drivers/gpio_pin.hpp"
#include "microbit_v2.h"
#include "util.hpp"

namespace edge::drivers {

class CapsenseController {
    GPIOPin touch_logo{TOUCH_LOGO, GPIOConfiguration::IN_NORES};

    etl::array<ProcessCallbackPtr, MAX_PROCESSES> subscriptions;

    CapsenseController();
    ~CapsenseController() = default;

public:
    CapsenseController(const CapsenseController&) = delete;
    CapsenseController(CapsenseController&&) = delete;
    CapsenseController& operator=(const CapsenseController&) = delete;
    CapsenseController& operator=(CapsenseController&&) = delete;

    static CapsenseController& get();

    void subscribe_captouch_press(ProcessCallbackPtr callback, uint8_t process_id);

};


} // namespace edge::drivers
