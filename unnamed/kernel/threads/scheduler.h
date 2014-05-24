#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include "hw/interrupts.h"
#include "mem/heap.h"


void init_threads();
void sleep_thread_self();
void scheduler_event(uint64_t savedregs);
uint32_t new_kthread( void (*function)(void), uint64_t stack_size );

#endif
