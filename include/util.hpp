#pragma once

namespace edge {
[[noreturn]] void panic(const char* reason);

using ProcessCallbackPtr = void (*)(int, int);
using ProcessName = etl::string<20>;
enum class FaultType { Usage };

} // namespace edge
