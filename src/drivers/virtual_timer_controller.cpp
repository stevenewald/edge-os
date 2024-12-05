#include "drivers/virtual_timer_controller.hpp"
#include "drivers/virtual_timer_linked_list.hpp"
#include "nrf52833.h"
#include "scheduler/pending_process_callbacks.hpp"

namespace edge::drivers
{

extern "C" {
void TIMER3_IRQHandler(void)
{
    auto vtc = VirtualTimerController::get();
    vtc.TIMER->EVENTS_COMPARE[2] = 0;
    printf("Starting interrupt\n");
    vtc.update_ll();
    printf("Interrupt end!\n");
    return;
}
}

VirtualTimerController::VirtualTimerController()
{
    TIMER->BITMODE = 0x3;
    TIMER->MODE = 0x0;
    TIMER->PRESCALER = 0x4;
    TIMER->TASKS_START = 1;
    TIMER->INTENSET = 0x40000;
    NVIC_EnableIRQ(TIMER3_IRQn);

}

void VirtualTimerController::update_ll()
{
    __disable_irq();
    node_t* ptr = list_get_first();
    if (!ptr)
    {
        return;
    }
    uint32_t val = ptr->timer_value;
    uint32_t current_time = read_timer();
    if (val < current_time)
    {
        printf("Adding ready callback.\n");
        PendingProcessCallbacks::get().add_ready_callback(0, reinterpret_cast<void(*)(int, int)>(ptr->callback));
    }
    /* while (val < current_time) */
    /* { */
    /*     list_remove_first(); */
    /*     ptr->callback(); */
    /*     free(ptr); */
    /**/
    /*     ptr = list_get_first(); */
    /*     if (!ptr) */
    /*     { */
    /*         break; */
    /*     } */
    /*     val = ptr->timer_value; */
    /*     current_time = read_timer(); */
    /* } */
    __enable_irq();
    return;
}

VirtualTimerController& VirtualTimerController::get()
{
    static VirtualTimerController controller;
    return controller;
}

uint32_t VirtualTimerController::read_timer()
{
    TIMER->TASKS_CAPTURE[1] = 1;
    return TIMER->CC[1];
}

uint32_t VirtualTimerController::timer_start(uint32_t microseconds, void* cb)
{
    __disable_irq();
    node_t* node_ptr = new node_t;
    uint32_t curr_time = read_timer();
    uint32_t time = curr_time + microseconds;
    node_ptr->timer_value = time;
    node_ptr->callback = cb;
    uint32_t node_id = (uint32_t) node_ptr;
    node_ptr->id = node_id;
    node_ptr->freq = microseconds;
    list_insert_sorted(node_ptr);
    update_ll();
    TIMER->CC[2] = list_get_first()->timer_value;
    __enable_irq();
    return node_id;
}

uint32_t VirtualTimerController::virtual_timer_start(uint32_t microseconds, void* cb)
{
    return timer_start(microseconds, cb);
}

void VirtualTimerController::virtual_timer_cancel(uint32_t timer_id)
{
    __disable_irq();
    node_t*ptr = list_get_first();
    if (!ptr)
    {
        __enable_irq();
        return;
    }
    if (ptr->id == timer_id)
    {
        list_remove(ptr);
        delete ptr;
        update_ll();
        ptr = list_get_first();
        if (ptr)
        {
            TIMER->CC[2] = list_get_first()->timer_value;
        }
        else
        {
            TIMER->CC[2] = 0;
        }
        __enable_irq();
        return;
    }

    while ((ptr->next != nullptr) && (ptr->id != timer_id))
    {
        ptr = ptr->next;
    }

    list_remove(ptr);
    delete ptr;
    update_ll();
    __enable_irq();
    ptr = list_get_first();
    if (ptr)
    {
        TIMER->CC[2] = list_get_first()->timer_value;
    }
    else
    {
        TIMER->CC[2] = 0;
    }

}





} // namespace edge::drivers
