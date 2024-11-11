#include "scheduler.hpp"

#include "drivers/driver_controller.hpp"
#include "nrf52833.h"
#include "register_utils.hpp"

extern char __start_user_programs_code[];
extern char __end_user_programs_code[];
extern char __start_user_programs_data[];
extern char __end_user_programs_data[];

namespace edge {

Scheduler scheduler;

#define CSP()                  \
    do {                                                 \
        int stack_type;                                  \
        asm volatile (                                   \
            "TST lr, #4\n\t"                             \
            "ITE EQ\n\t"                                 \
            "MOVEQ %[type], #0\n\t"                      \
            "MOVNE %[type], #1\n\t"                      \
            : [type] "=r" (stack_type)                   \
        );                                               \
        if (stack_type == 0) {                           \
            printf("Main Stack Pointer (MSP) used\n");   \
        } else {                                         \
            printf("Program Stack Pointer (PSP) used\n");\
        }                                                \
    } while (0)

void Scheduler::start_scheduler()
{
    static constexpr size_t CLOCK_HZ = 64'000'000;
    static constexpr size_t SYSTICK_LOAD = (CLOCK_HZ * QUANTUM_MILLIS) / 1000;
    asm volatile("CPSID I");
    SysTick->LOAD = SYSTICK_LOAD;
    SysTick->VAL = 0;
    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

    NVIC_SetPriority(PendSV_IRQn, 0x3);
    NVIC_SetPriority(SysTick_IRQn, 0x1);
    asm volatile("CPSIE I");
    asm volatile("SVC #0");
}

void Scheduler::update_mpu_stack_region()
{
    MPU->CTRL = 0;
    // 0: kernel code
    // Privileged: read only
    // User: None
    MPU->RNR = 0;
    MPU->RBAR = (0 << MPU_RBAR_ADDR_Pos) | (0 << MPU_RBAR_VALID_Pos);
    MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (18 << MPU_RASR_SIZE_Pos) | (1 << 0);

    // 1: User code in flash
    // Privileged: read only
    // User: read only
    // MPU->RNR = 1;
    // MPU->RBAR = (((unsigned)(__start_user_programs_code)) << MPU_RBAR_ADDR_Pos)
    //             | (0 << MPU_RBAR_VALID_Pos);
    // MPU->RASR = (0b111 << MPU_RASR_AP_Pos) | (21 << MPU_RASR_SIZE_Pos) | (1 << 0);

    // 2: Flash after user code
    // Privileged: read
    // User: none
    // MPU->RNR = 2;
    // MPU->RBAR = (((unsigned)(__end_user_programs_code)) << MPU_RBAR_ADDR_Pos)
    //             | (0 << MPU_RBAR_VALID_Pos);
    // MPU->RASR = (0b101 << MPU_RASR_AP_Pos) | (21 << MPU_RASR_SIZE_Pos) | (1 << 0);

    // 3: RAM before user data
    // Privileged: read/write
    // User: none
    MPU->RNR = 1;
    MPU->RBAR =
        (((unsigned)(0x20000000)) << MPU_RBAR_ADDR_Pos) | (0 << MPU_RBAR_VALID_Pos);
    MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (16 << MPU_RASR_SIZE_Pos) | (1 << 0);


    // 4: User data in RAM
    // Privileged: read/write
    // User: read/write
    // MPU->RNR = 4;
    // MPU->RBAR = (((unsigned)(__start_user_programs_data)) << MPU_RBAR_ADDR_Pos)
    //             | (0 << MPU_RBAR_VALID_Pos);
    // MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (21 << MPU_RASR_SIZE_Pos) | (1 << 0);

    // 5: RAM after user data
    // Privileged: read/write
    // User: TODO
    // MPU->RNR = 5;
    // MPU->RBAR = (((unsigned)(__end_user_programs_data)) << MPU_RBAR_ADDR_Pos)
    //             | (0 << MPU_RBAR_VALID_Pos);
    // MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (21 << MPU_RASR_SIZE_Pos) | (1 << 0);

    static bool t = false;
    if (!t) {
        t = true;
        printf("Start program code %x\n", __start_user_programs_code);
        printf("End program code %x\n", __end_user_programs_code);
        printf("Start program data %x\n", __start_user_programs_data);
        printf("End program data %x\n", __end_user_programs_data);
        printf("Start of stack %x\n", task_stack[current_task_index].stack.begin());
    }

    //
    // MPU->RNR = 6;
    // MPU->RBAR =
    //     (((unsigned)task_stack[current_task_index].stack.begin()) <<
    //     MPU_RBAR_ADDR_Pos) | (0 << MPU_RBAR_VALID_Pos);
    // MPU->RASR = (0b011 << MPU_RASR_AP_Pos) | (14 << MPU_RASR_SIZE_Pos) | (1 << 0);
    //

    // Enable MPU with background region enabled
    MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
}

void Scheduler::add_task(void (*function)(void), uint8_t priority)
{
    task_stack.emplace_back(
        stack_registers{reinterpret_cast<unsigned>(function)}, priority
    );
}

void trigger_pendsv()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void Scheduler::handle_first_svc_hit()
{

    __set_CONTROL(0x03);
    // Unprivileged Mode

    __set_PSP((unsigned)task_stack[current_task_index].stack_ptr_loc);

    // Trigger PendSV
    trigger_pendsv();
}

void Scheduler::change_current_task_priority(uint8_t new_priority)
{
    task_stack[current_task_index].consecutive_quantums_to_run = new_priority;
    slices_remaining = etl::min(slices_remaining, new_priority);
    printf("Task %d requested new priority %d\n", current_task_index, new_priority);
}

extern "C" {

__attribute__((naked, used)) void PendSV_Handler()

{
	static int i = 0;
	if(++i < 10) {
	CSP();
	}
    asm volatile("CPSID I");
    if (--scheduler.slices_remaining == 0) {
        asm volatile("mrs r0,psp\n"
                     "sub r0,#96\n"
                     "stm r0!,{r4-r11}\n"
                     "vstm r0!, {s16-s31}");

        // This function will dirty registers. That's okay
        scheduler.task_stack[scheduler.current_task_index].stack_ptr_loc =
            reinterpret_cast<unsigned*>(__get_PSP());

        scheduler.current_task_index =
            (scheduler.current_task_index == scheduler.task_stack.size() - 1)
                ? 0
                : scheduler.current_task_index + 1;

        scheduler.slices_remaining = scheduler.task_stack[scheduler.current_task_index]
                                         .consecutive_quantums_to_run;

        __set_PSP(reinterpret_cast<unsigned>(
            scheduler.task_stack[scheduler.current_task_index].stack_ptr_loc
        ));

        asm volatile("mrs r0,psp\n"
                     "sub r0,#96\n"
                     "ldm r0!,{r4-r11}\n"
                     "vldm r0!, {s16-s31}");
    }

    asm volatile("CPSIE I");

    // Always want to call drivers on context switch
    drivers::do_async_work();

    // Return in thumb/process mode and restore using extended stack frame
    asm volatile("ldr r0,=0xffffffed\n"
                 "bx r0");
}

__attribute__((used)) void SysTick_Handler()
{
    trigger_pendsv();
}
}

// Runs in userspace after async callback has finished
// I don't think there's any way to make this cleaner lol
__attribute__((used, naked)) void restore_regs()
{
    // Load fpscr first so we can avoid dirtying r0 after its popped
    asm volatile("ldr r0, [sp, #96]\n"
                 "vmsr fpscr, r0\n");

    // Pop regs as usual
    asm volatile("pop {r0, r1, r2, r3, r12, lr}");

    // Skip SP and RETPSR. SP will be loaded last
    // RETPSR should be ignored because we already popped when returning from exception
    asm volatile("add sp, #8");

    // Pop caller saved FP registers
    asm volatile("vpop {s0-s15}");

    // Skip FPSCR and 2 reserved regs
    asm volatile("add sp, #12");

    asm volatile("ldr pc, [sp, #-84]");
}

void Scheduler::yield_current_task()
{
    auto callback_opt = drivers::get_ready_callback(current_task_index);
    if (!callback_opt) {
        // Expire turn
        slices_remaining = 1;
        trigger_pendsv();
        return;
    }
    auto [callback_address, arg1, arg2] = callback_opt.value();
    auto& t = scheduler.task_stack[scheduler.current_task_index];

    // This stack frame, originally created by the exception handler, will be popped
    // by restore()
    t.stack_ptr_loc = (unsigned*)__get_PSP();
    auto stored_registers = reinterpret_cast<stack_registers*>(t.stack_ptr_loc);

    // "Push" registers, create a fake stack frame
    // This will be popped by the exception handler
    t.stack_ptr_loc -= (sizeof(stack_registers) / sizeof(unsigned));
    auto new_registers = reinterpret_cast<stack_registers*>(t.stack_ptr_loc);
    new_registers->R0 = static_cast<unsigned>(arg1);
    new_registers->R1 = static_cast<unsigned>(arg2);

    // Return to our restore_regs function so we can pop the caller-saved registers of
    // the saved/previous execution path
    new_registers->LR = reinterpret_cast<unsigned>(&restore_regs);
    new_registers->RETURN_ADDR = reinterpret_cast<unsigned>(callback_address);

    new_registers->CTRL = stored_registers->CTRL;
    __set_PSP(reinterpret_cast<unsigned>(t.stack_ptr_loc));
}

} // namespace edge
