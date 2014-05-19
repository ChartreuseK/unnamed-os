#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>
#include "hw/ports.h"
#include "mem/physical.h"
#include "mem/heap.h"
#include "mem/gdt.h"
#include "hw/ioapic.h"
#include "io/keyboard.h"


void disable_pic();
void enable_local_apic();
void enable_interrupts();
void setup_interrupts();

extern void dprintf(const char *msg, ...);

extern void text_putxy(char *str, int x, int y, uint8_t attr);


extern uint64_t isr_table[];



#endif
