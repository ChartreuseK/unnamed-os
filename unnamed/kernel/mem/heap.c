
#include "heap.h"

#define HEAP_BASE 0xFFFFA00000000000

uint64_t curoffset = 0;
int64_t curpage = -1;


// Simple watermark allocator
void *malloc(uint64_t size)
{
    void *ptr = NULL;
    
    // If we don't have the current page mapped and allocated
    if(curpage == -1 || (curoffset + size) > 0x1000)
    {
        // Map as many pages as we need to the heap
        for(unsigned int i = 1; i <= (curoffset + size) / 0x1000; i++)
        {
            addmap(phymem_get_page(), HEAP_BASE + ((curpage + i) * 0x1000), 0);
        }
    
        
        
    }
    
    
    ptr = (void *)(HEAP_BASE + curoffset);
    
    curoffset = (curoffset + size) % 0x1000;
    
    return ptr;
}



void free(void __attribute__((unused)) *ptr)
{
    // 
}
