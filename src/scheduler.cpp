#include "scheduler.hpp"

namespace edge {

void
Scheduler::start_scheduler()
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

void
Scheduler::handle_priority_change()
{
    uint8_t new_prio = 0;
    uint16_t* PC_reg;

    uint32_t* SP_reg;
    asm("MRS %0,PSP" : "=r"(SP_reg));

    // this is bad. fix
    PC_reg = reinterpret_cast<uint16_t*>(SP_reg[6]);
    new_prio = PC_reg[-1] & 0xFF;

    task_stack[current_task_index].priority = new_prio;
    printf("Task %d requested new priority %d\n", current_task_index, new_prio);
}

void
Scheduler::handle_first_svc_hit()
{
    // Unprivileged Mode
    __set_CONTROL(0x03);

    __set_PSP((unsigned)task_stack[current_task_index].stack_ptr_loc);

    // Trigger PendSV
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void
Scheduler::advance_turn()
{
    // This function will dirty registers. That's okay

    task_stack[current_task_index].stack_ptr_loc = (unsigned*)__get_PSP();

    current_task_index =
        (current_task_index >= task_stack.size() - 1) ? 0 : current_task_index + 1;

    slices_remaining = task_stack[current_task_index].priority;

    __set_PSP((unsigned)task_stack[current_task_index].stack_ptr_loc);
}

bool
Scheduler::continue_current_task()
{
    return --slices_remaining > 0;
}

void
Scheduler::print_info() const
{
    printf("Task %d has %d slices remaining\n", current_task_index, slices_remaining);
}

void
Scheduler::add_task(void (*function)(void), uint8_t priority)
{
    task_stack.emplace_back(
        process_metadata{reinterpret_cast<unsigned>(function)}, priority
    );
}

extern "C" {
void
print_task_info(void)
{
    scheduler.print_info();
}

__attribute__((naked)) void
PendSV_Handler()
{
    asm("CPSID I");

    if (scheduler.continue_current_task()) {
        asm("bl print_task_info");
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

    scheduler.advance_turn();

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

void
SysTick_Handler()
{
    // Trigger PENDSV
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

// Triggered on program start, as well as when process wants to change priority
void
SVC_Handler()
{
    static bool first_svc_hit = true;
    if (first_svc_hit) {
        scheduler.handle_first_svc_hit();
        first_svc_hit = false;
    }
    else {
        scheduler.handle_priority_change();
    }
}
}
edge::Scheduler scheduler;
} // namespace edge
