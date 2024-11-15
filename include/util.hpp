#pragma once

#define USER_CODE __attribute__((section(".user_code")))

namespace edge {
[[noreturn]] void panic(const char* reason);

using ProcessCallbackPtr = void (*)(int, int);
enum class FaultType { Usage, Bus, Memory };

struct exception_stack_registers {
    unsigned R0{};
    unsigned R1{};
    unsigned R2{};
    unsigned R3{};
    unsigned R12{};
    unsigned LR{};
    unsigned RETURN_ADDR{};
    unsigned CTRL{0x01000000};
    unsigned FP_REGS[16]{};
    unsigned FPSCR{};

    // diagram: https://shorturl.at/85lyY
    unsigned RESERVED_FOR_STACK_ALIGNMENT[2]{};

    exception_stack_registers(unsigned return_addr) : RETURN_ADDR(return_addr) {}
};

} // namespace edge
