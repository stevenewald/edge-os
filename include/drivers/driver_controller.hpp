#pragma once

#include "drivers/driver_types.hpp"

#include <stdio.h>

namespace edge::drivers {

void do_work();

etl::optional<int> handle_command(DriverType type, int arg1, int arg2, int arg3);

} // namespace edge::drivers
