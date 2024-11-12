#pragma once

namespace edge {
[[noreturn]] void panic(const char* reason);

using ProcessCallbackPtr = void (*)(int, int);

} // namespace edge
