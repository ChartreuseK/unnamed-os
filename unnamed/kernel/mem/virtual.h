#ifndef _VIRTUAL_H
#define _VIRTUAL_H


#include <stdint.h>

#include "mem/physical.h"

enum 
{
	MM_READWRITE 	= 0x2,
	MM_KERNELSPACE 	= 0x0,
	
	MM_READONLY  = 0x0,
	MM_USERSPACE = 0x4,
	MM_GLOBAL	 = 0x80
};



uint64_t read_cr3();
void write_cr3(uint64_t value);

void bootmap();

int addmap(uint64_t physical, uint64_t virtual, unsigned int flags);
void removemap(uint64_t virtual);

extern void dprintf(const char *msg, ...);



#endif
