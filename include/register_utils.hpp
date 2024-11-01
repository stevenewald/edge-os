#pragma once

namespace edge {
#define TRIGGER_SVC(call_type)   asm volatile("svc %0" ::"I"(call_type))
#define SET_REGISTER(reg, value) asm volatile("mov " #reg ", %0" ::"r"(value) : #reg)
#define READ_REGISTER(reg, var)  asm volatile("mov %0, " #reg : "=r"(var))
} // namespace edge
