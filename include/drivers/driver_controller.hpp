#pragma once

#include "drivers/driver_enums.hpp"

#include <stdio.h>

namespace edge::drivers {

void do_async_work();

etl::optional<int> handle_command(DriverCommand type, int arg1, int arg2, int arg3);

void handle_subscribe(
    DriverSubscribe type, ButtonCallbackPtr callback, int arg1, int arg2,
    uint8_t process_id
);

etl::optional<button_subscribe_callback> get_ready_callback(uint8_t process_id);

} // namespace edge::drivers
