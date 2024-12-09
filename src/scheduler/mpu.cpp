#include "scheduler/mpu.hpp"

#include "nrf52833.h"
#include "util.hpp"

extern "C" {
extern uint8_t __start_user_programs_code;
extern uint8_t __end_user_programs_code;
extern uint8_t __start_user_programs_data;
extern uint8_t __end_user_programs_data;
}

namespace edge {
MpuController& MpuController::get()
{
    static MpuController mpu_controller;
    return mpu_controller;
}

void MpuController::initialize_mpu() const
{
    // MPU disabled. Read comment in linker script to see why
    return;
    auto user_code_start = reinterpret_cast<unsigned>(&__start_user_programs_code);
    MPU->RNR = 0;
    MPU->RBAR = (user_code_start & MPU_RBAR_ADDR_Msk);
    MPU->RASR = (0b111 << MPU_RASR_AP_Pos) | (13 << MPU_RASR_SIZE_Pos)
                | (1 << MPU_RASR_ENABLE_Pos);

    auto user_data_start = reinterpret_cast<unsigned>(&__start_user_programs_data);
    MPU->RNR = 1;
    MPU->RBAR = (user_data_start & MPU_RBAR_ADDR_Msk);
    MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (13 << MPU_RASR_SIZE_Pos)
                | (1 << MPU_RASR_ENABLE_Pos);

    MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
}

void MpuController::set_program_stack_start(const unsigned* stack_ptr) const
{
    auto start_stack_ptr = reinterpret_cast<unsigned>(stack_ptr);
    if (start_stack_ptr % STACK_SIZE_BYTES != 0) [[unlikely]] {
        panic("STACK START ADDRESS IS UNALIGNED");
    }
    MPU->RNR = 2;
    MPU->RBAR = (start_stack_ptr & MPU_RBAR_ADDR_Msk);
    MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (10 << MPU_RASR_SIZE_Pos)
                | (1 << MPU_RASR_ENABLE_Pos);
}

void MpuController::disable_mpu() const
{
    MPU->CTRL = 0;
}
} // namespace edge
