#include "virtual.h"



struct VirtualAddr{
	void *PML4;				// Pointer to our PML4
	
	
	
} va;





















// Get the current address containing the PML4
uint64_t read_cr3() 
{
    uint64_t value;
    __asm__("mov rax, cr3" : "=a"(value));
    return value;
}

// Tell the processor where the new PML4 is located
void write_cr3(uint64_t value) 
{
    __asm__("mov cr3, rax" : : "a"(value));
}
