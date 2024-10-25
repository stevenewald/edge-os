#pragma once

namespace edge {
#define TRIGGER_SVC(call_type)      asm("svc %0" ::"I"(static_cast<uint8_t>(call_type)))
#define SET_REGISTER(reg, value) asm("mov " #reg ", %0" ::"r"(value) : #reg)
#define READ_REGISTER(reg, var)  asm("mov %0, " #reg : "=r"(var))
} // namespace edge
