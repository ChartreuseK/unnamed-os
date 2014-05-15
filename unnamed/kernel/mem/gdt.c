
#include "mem/gdt.h"

#define MAX_GDT_ENTRIES   128

uint32_t *gdt;

int curgdt;

void add_gdt_entry(int entry, uint32_t lower, uint32_t upper)
{
    if(entry > MAX_GDT_ENTRIES)
    {
        return;
    }
    
    gdt[entry*2] = lower;
    gdt[(entry*2)+1] = upper;
}

int append_gdt(uint32_t lower, uint32_t upper)
{
    add_gdt_entry(curgdt++, lower, upper);
    
    uint8_t gdtptr[10];
    
    *(uint16_t *)gdtptr = (MAX_GDT_ENTRIES * 8) - 1;
    *(uint64_t *)(gdtptr+2) =(uint64_t) gdt;
    
    __asm__ volatile("lgdt [%0]"::"r"(gdtptr));
    
    return curgdt - 1;
    
}


void setup_gdt()
{
    gdt = malloc(MAX_GDT_ENTRIES * 8); // Allocate a very large amount of entries

    add_gdt_entry(0, 0, 0); // NULL Segment 0x0
    
    add_gdt_entry(1, 0, (1<<15) | (1<<21)); // Code segment 0x8
    
    add_gdt_entry(2, 0, (1<<15) | (1<<9) | (1<<12)); // Code segment 0x10

    curgdt = 3;
    
    
    
    uint8_t gdtptr[10];
    
    *(uint16_t *)gdtptr = (curgdt * 8) - 1;
    *(uint64_t *)(gdtptr+2) =(uint64_t) gdt;
    
    __asm__ volatile("lgdt [%0]"::"r"(gdtptr));
    
}


