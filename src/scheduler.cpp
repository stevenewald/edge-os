#include "scheduler.hpp"

#include "drivers/driver_controller.hpp"
#include "nrf52833.h"

namespace edge {

Scheduler scheduler;

void Scheduler::start_scheduler()
{
    asm volatile("CPSID I");
    SysTick->LOAD = 1600000; // period, change later
    SysTick->VAL = 0;
    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

    NVIC_SetPriority(PendSV_IRQn, 0x3);
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
    task_stack[current_task_index].priority = new_priority;
    slices_remaining = etl::min(slices_remaining, new_priority);
    printf("Task %d requested new priority %d\n", current_task_index, new_priority);
}

extern "C" {

__attribute__((naked, used)) void PendSV_Handler()

{
    asm volatile("CPSID I");
    if (--scheduler.slices_remaining > 0) {
        goto END;
    }

    asm volatile("mrs r0,psp\n"
                 "sub r0,#96\n"
                 "stm r0!,{r4-r11}");

    // This function will dirty registers. That's okay
    scheduler.task_stack[scheduler.current_task_index].stack_ptr_loc =
        reinterpret_cast<unsigned*>(__get_PSP());

    scheduler.current_task_index =
        (scheduler.current_task_index == scheduler.task_stack.size() - 1)
            ? 0
            : scheduler.current_task_index + 1;

    scheduler.slices_remaining =
        scheduler.task_stack[scheduler.current_task_index].priority;

    __set_PSP(reinterpret_cast<unsigned>(
        scheduler.task_stack[scheduler.current_task_index].stack_ptr_loc
    ));

    asm volatile("mrs r0,psp\n"
                 "sub r0,#96\n"
                 "ldm r0!,{r4-r11}\n");
END:
    // Always want to call drivers on context switch
    // Note: this is probably not what will call callbacks
    drivers::do_work();

    // printf(
    //     "Task %d has %d slices remaining\n", scheduler.current_task_index,
    //     scheduler.slices_remaining
    // );

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

void Scheduler::yield_current_task()
{
    printf("Task %d yielded\n", current_task_index);
    slices_remaining = 1;
    trigger_pendsv();
}

} // namespace edge
