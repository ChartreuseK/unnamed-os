
#include "mem/memtools.h"






void *memcpy(void *dest, void *source, uint64_t size)
{
    while(size--)
    {
        *(uint8_t*)dest++ = *(uint8_t*)source++;
    }
    return dest;
}


