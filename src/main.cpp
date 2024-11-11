#include "drivers/driver_enums.hpp"
#include "nrf52833.h"
#include "scheduler.hpp"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Pin configurations

template <int N>
void task(void);

extern char __start_user_programs_code[];
extern char __end_user_programs_code[];
extern char __start_user_programs_data[];
extern char __end_user_programs_data[];

int main(void)
{
    printf("Starting EdgeOS\n");

    edge::scheduler.add_task(task<4>);
    // edge::scheduler.add_task(task<3>);
    // edge::scheduler.add_task(task<2>);
    // edge::scheduler.add_task(task<1>);
    // edge::scheduler.add_task(task<0>);

    edge::scheduler.start_scheduler();
}
