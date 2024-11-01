#include "scheduler.hpp"

#include "system_call_type.hpp"

namespace edge {

Scheduler scheduler;

void Scheduler::start_scheduler()
{
    asm("CPSID I");
    SysTick->LOAD = 16000000; // period
    SysTick->VAL = 0;
    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

    NVIC_SetPriority(PendSV_IRQn, 0x3);
    NVIC_SetPriority(SysTick_IRQn, 0x1);
    asm("CPSIE I");
    asm("SVC #0");
}

void Scheduler::add_task(void (*function)(void), uint8_t priority)
{
    task_stack.emplace_back(
        process_metadata{reinterpret_cast<unsigned>(function)}, priority
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
    task_stack[current_task_index].priority = new_priority;
    slices_remaining = etl::min(slices_remaining, new_priority);
    printf("Task %d requested new priority %d\n", current_task_index, new_priority);
}

extern "C" {

__attribute__((naked, used)) void PendSV_Handler()
{
    asm("CPSID I");

    if (--scheduler.slices_remaining > 0) {
        printf(
            "Task %d has %d slices remaining\n", scheduler.current_task_index,
            scheduler.slices_remaining
        );
        goto END;
    }

    // Save context
    asm("mrs r0,psp");
    asm("sub r0,#32");
    asm("stm r0!,{r4,r5,r6,r7}");
    asm("mov r4,r8");
    asm("mov r5,r9");
    asm("mov r6,r10");
    asm("mov r7,r11");
    asm("stm r0!,{r4,r5,r6,r7}");

    // This function will dirty registers. That's okay
    scheduler.task_stack[scheduler.current_task_index].stack_ptr_loc =
        reinterpret_cast<unsigned*>(__get_PSP());

    scheduler.current_task_index =
        (scheduler.current_task_index >= scheduler.task_stack.size() - 1)
            ? 0
            : scheduler.current_task_index + 1;

    scheduler.slices_remaining =
        scheduler.task_stack[scheduler.current_task_index].priority;

    __set_PSP(reinterpret_cast<unsigned>(
        scheduler.task_stack[scheduler.current_task_index].stack_ptr_loc
    ));

    // Restore next context
    asm("mrs r0,psp");
    asm("sub r0,#16");
    asm("ldm r0!,{R4,R5,R6,R7}");
    asm("mov r8,r4");
    asm("mov r9,r5");
    asm("mov r10,r6");
    asm("mov r11,r7");
    asm("sub r0,#32");
    asm("ldm r0!,{r4,r5,r6,r7}");

END:
    asm("CPSIE I");
    // Exit interrupt mode and return to thread mode
    // We don't want to return in the normal 'C' way, this curcumvents it
    asm("ldr r0,=0xfffffffd");
    asm("bx r0");
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

void Scheduler::yield_current_task()
{
    printf("Task %d yielded\n", current_task_index);
    slices_remaining = 1;
    trigger_pendsv();
}

} // namespace edge
