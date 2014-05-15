#ifndef _PORTS_H
#define _PORTS_H

#include <stdint.h>



static inline uint8_t inportb(uint16_t port)
{
    uint8_t retval;
    __asm__ volatile("inb %0, %1" : "=a"(retval) : "dN"(port));
    return retval;
}

static inline void outportb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %1, %0" :  :"a"(val), "dN"(port));
}







#endif
