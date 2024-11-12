#pragma once

#include <cstdint>

namespace edge {
enum class SystemCallType : uint8_t {
    CHANGE_PRIORITY = 0,
    YIELD = 1,
    COMMAND = 2,
    SUBSCRIBE = 3,
    IPC = 4
};
}
