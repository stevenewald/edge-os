#include "userlib/syscalls.hpp"
#include "nrf_delay.h"

void ending_task(void)
{
	nrf_delay_ms(1000);
	edge::userlib::debug_print("ENDING PROGRAM: Done Running\n");
}
