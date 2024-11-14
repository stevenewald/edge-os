#include "drivers/driver_enums.hpp"
#include "fault_handler.hpp"
#include "scheduler.hpp"
#include "util.hpp"

#include <stdbool.h>
#include <stdio.h>

extern void task0(void);

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::FaultHandler::get();

    // edge::scheduler.add_task(exception_task);
    edge::scheduler.add_task(task0);
    // edge::scheduler.add_task(task1);

    edge::scheduler.start_scheduler();
}
