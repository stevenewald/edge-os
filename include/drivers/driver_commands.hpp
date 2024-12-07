#pragma once

#include "drivers/driver_enums.hpp"

#include <stdio.h>

namespace edge::drivers {

// SHOULD BE DEPRECATED SOON
void do_async_work();

etl::optional<int> handle_command(DriverCommand type, int arg1, int arg2, int arg3);

etl::optional<int> handle_subscribe(
    DriverSubscribe type, ProcessCallbackPtr callback, int arg1, int arg2,
    ProcessId process_id
);

} // namespace edge::drivers
