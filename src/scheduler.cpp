#include "scheduler.hpp"

#include "drivers/driver_controller.hpp"
#include "nrf52833.h"

namespace edge {

Scheduler scheduler;

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
    // TODO: move to bootloader?
    NVIC_SetPriority(GPIOTE_IRQn, 0x2);
    NVIC_SetPriority(SysTick_IRQn, 0x1);
    asm volatile("CPSIE I");
    asm volatile("SVC #0");
}

void Scheduler::add_task(void (*function)(void), uint8_t priority)
{
    task_stack.emplace_back(
        saved_registers{reinterpret_cast<unsigned>(function)}, priority
    );
}

void Scheduler::handle_first_svc_hit()
{
    // Unprivileged Mode
    __set_CONTROL(0x03);

    __set_PSP((unsigned)task_stack[current_task_index].stack_ptr_loc);

    // Trigger PendSV
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
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
    asm volatile("CPSID I");
    if (--scheduler.slices_remaining == 0) {
        asm volatile("mrs r0,psp\n"
                     "sub r0,#32\n"
                     "stm r0!,{r4-r11}");

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
                     "sub r0,#32\n"
                     "ldm r0!,{r4-r11}\n");
    }

    // Always want to call drivers on context switch
    drivers::do_async_work();

    asm volatile("CPSIE I\n"
                 "ldr r0,=0xfffffffd\n"
                 "bx r0");
}

void trigger_pendsv()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

__attribute__((used)) void SysTick_Handler()
{
    trigger_pendsv();
}
}

__attribute__((used, naked)) void restore()
{
    asm volatile("mrs r0, psp\n"
                 "add r0, #32\n"
                 "msr psp, r0\n");
    asm volatile("ldr     r0, [sp, #0]\n"
                 "ldr     r1, [sp, #4]\n"
                 "ldr     r2, [sp, #8]\n"
                 "ldr     r3, [sp, #12]\n"
                 "ldr     r12, [sp, #16]\n"
                 "ldr     lr, [sp, #20]\n"
                 "ldr     pc, [sp, #24]\n");
}

void Scheduler::yield_current_task()
{
    auto callback_opt = drivers::get_ready_callback(current_task_index);
    if (callback_opt) {
        printf("Yield1\n");
        auto [address, arg1] = callback_opt.value();
        auto& t = scheduler.task_stack[scheduler.current_task_index];
        t.stack_ptr_loc = (unsigned*)__get_PSP();
        (*(t.stack_ptr_loc + 6))++;

        // "Push" registers
        t.stack_ptr_loc -= 8;
        *(t.stack_ptr_loc + 0) = *(t.stack_ptr_loc + 8 + 0);
        *(t.stack_ptr_loc + 1) = *(t.stack_ptr_loc + 8 + 1);
        *(t.stack_ptr_loc + 2) = *(t.stack_ptr_loc + 8 + 2);
        *(t.stack_ptr_loc + 3) = *(t.stack_ptr_loc + 8 + 3);
        *(t.stack_ptr_loc + 4) = *(t.stack_ptr_loc + 8 + 4);
        *(t.stack_ptr_loc + 5) = (unsigned)&restore;
        *(t.stack_ptr_loc + 6) = (unsigned)address;
        *(t.stack_ptr_loc + 7) = *(t.stack_ptr_loc + 8 + 7);
        __set_PSP((unsigned)t.stack_ptr_loc);
    }
    else {
        slices_remaining = 1;
        trigger_pendsv();
    }
}

} // namespace edge
