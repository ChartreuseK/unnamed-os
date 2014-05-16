
#include "heap.h"

#define HEAP_BASE 0xFFFFA00000000000



int64_t curoffset = -1;
int64_t curpage = -1;


void *malloc(int64_t size)
{
    
    if(curoffset == -1) curoffset = 0;
    
    void *ptr = NULL;
    
    // If we don't have the current page mapped and allocated
    if(curpage == -1 )
    {
        if(addmap(phymem_get_page(), HEAP_BASE , MM_READWRITE) == 0)
        {
            return NULL;
        }
        curpage = 0;
    }
    
    
    while((curoffset + size) >= (int64_t)(0x1000 * (curpage + 1)))
    {
        curpage++;
        if(addmap(phymem_get_page(), HEAP_BASE + (curpage * 0x1000) , MM_READWRITE) == 0)
        {
            return NULL;
        }
    }
    
    ptr = (void *)(HEAP_BASE + curoffset);
    curoffset += size;
    

    return ptr;
}

/*
// Simple watermark allocator
void *malloc(uint64_t size)
{
    
    if(curoffset == -1) curoffset = 0;
    
    void *ptr = NULL;
    
    // If we don't have the current page mapped and allocated
    if(curpage == -1 )
    {
        if(addmap(phymem_get_page(), HEAP_BASE , MM_READWRITE) == 0)
        {
            return NULL;
        }
        curpage = 0;
    }
    
    if ((curoffset + size) > 0x1000)
    {
        // Map as many pages as we need to the heap
        for(unsigned int i = 1; i <= ((curoffset + size) / 0x1000); i++)
        {
            if(addmap(phymem_get_page(), HEAP_BASE + ((++curpage) * 0x1000), MM_READWRITE) == 0)
            {
                return NULL;
            }
        }
    }
    
    
    
    
    ptr = (void *)(HEAP_BASE + (curpage * 0x1000) +  curoffset);
    
    curoffset = (curoffset + size) % 0x1000;
    
    return ptr;
}

*/

void free(void __attribute__((unused)) *ptr)
{
    // 
}
