#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>
#include "mem/heap.h"



void setup_gdt();
int append_gdt(uint32_t lower, uint32_t upper);



#endif
