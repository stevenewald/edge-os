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

void trigger_pendsv()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void Scheduler::handle_first_svc_hit()
{
    // Unprivileged Mode
    __set_CONTROL(0x03);

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

    asm volatile("CPSIE I");

    // Always want to call drivers on context switch
    drivers::do_async_work();

    asm volatile("ldr r0,=0xfffffffd\n"
                 "bx r0");
}

__attribute__((used)) void SysTick_Handler()
{
    trigger_pendsv();
}
}

// Runs in userspace after async callback has finished
__attribute__((used, naked)) void restore_regs()
{
    asm volatile("pop {r0, r1, r2, r3, r12, lr}\npop {pc}");
}

// Returns old flag
unsigned move_registers_up(saved_registers* registers)
{
    auto old_flag = registers->FLAG;
    registers->FLAG = registers->RETURN_ADDR;
    registers->RETURN_ADDR = registers->LR;
    registers->LR = registers->R12;
    registers->R12 = registers->R3;
    registers->R3 = registers->R2;
    registers->R2 = registers->R1;
    registers->R1 = registers->R0;
    registers->R0 = 0;
    return old_flag;
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
    auto [callback_address, arg1] = callback_opt.value();
    auto& t = scheduler.task_stack[scheduler.current_task_index];

    // This stack frame, originally created by the exception handler, will be popped
    // by restore()
    t.stack_ptr_loc = reinterpret_cast<unsigned*>(__get_PSP());
    auto stored_registers = reinterpret_cast<saved_registers*>(t.stack_ptr_loc);
    stored_registers->RETURN_ADDR++;

    unsigned old_flag = move_registers_up(stored_registers);

    // "Push" registers, create a fake stack frame
    // This will be popped by the exception handler
    t.stack_ptr_loc -= 7;
    auto new_registers = reinterpret_cast<saved_registers*>(t.stack_ptr_loc);
    new_registers->R0 = static_cast<unsigned>(arg1);
    new_registers->LR = reinterpret_cast<unsigned>(&restore_regs);
    new_registers->RETURN_ADDR = reinterpret_cast<unsigned>(callback_address);
    new_registers->FLAG = old_flag;
    __set_PSP(reinterpret_cast<unsigned>(t.stack_ptr_loc));
}

} // namespace edge
