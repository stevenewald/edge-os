#include "drivers/driver_enums.hpp"
#include "faults/fault_handler.hpp"
#include "scheduler/scheduler.hpp"
#include "util.hpp"

#include <stdbool.h>
#include <stdio.h>

extern void ipc_part1(void);
extern void ipc_part2(void);
extern void exception_task(void);

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::FaultHandler::get();

    edge::scheduler.add_task(exception_task);

    edge::scheduler.add_task(ipc_part1);
    edge::scheduler.add_task(ipc_part2);

    edge::scheduler.start_scheduler();
}
