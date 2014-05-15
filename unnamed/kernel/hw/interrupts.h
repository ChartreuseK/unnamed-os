#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>
#include "hw/ports.h"
#include "mem/physical.h"
#include "mem/heap.h"
#include "mem/gdt.h"
#include "hw/ioapic.h"


void disable_pic();
void enable_local_apic();
void enable_interrupts();
void setup_interrupts();

#endif
