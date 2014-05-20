
#include "heap.h"

#define HEAP_BASE  0xFFFFA00000000000
//                        ^-- Each of these is 1TB
//                        v-- We're giving each pool 1TB of address space
#define POOL_64B   0xFFFFA10000000000
#define POOL_512B  0xFFFFA20000000000
#define POOL_4K    0xFFFFA30000000000
#define POOL_64K   0xFFFFA40000000000
#define POOL_1M    0xFFFFA50000000000
#define POOL_16M   0xFFFFA60000000000
#define POOL_64M   0xFFFFA70000000000
#define POOL_128M  0xFFFFA80000000000
#define POOL_512M  0xFFFFA90000000000   
#define POOL_1G    0xFFFFAA0000000000   // Going to make the biggest ammount a single 
                                        // malloc can be as 1GB for now.
#define POOL_MAX   0xFFFFAB0000000000

uint64_t pool_64b_firstfree     = 0;
uint64_t pool_512b_firstfree    = 0; 

uint64_t pool_64b_top     = 0;
uint64_t pool_512b_top    = 0;

uint64_t pool_4k_top      = 0;  // Points to the free part of the virt address space
                                   // Since we have 1TB of virtual address space
                                   // we're not going to bother reusing it.
uint64_t pool_64k_top     = 0;
uint64_t pool_1M_top      = 0;
uint64_t pool_16M_top     = 0;
uint64_t pool_64M_top     = 0;
uint64_t pool_128M_top    = 0;
uint64_t pool_512M_top    = 0;
uint64_t pool_1G_top      = 0;

extern void dprintf(const char *str, ...);

void *malloc(int64_t size)
{
    void *retval = NULL;
    
    if(size <= 64)
    {
        if(pool_64b_firstfree == NULL)
        {
            if(addmap(phymem_get_page(), POOL_64B + pool_64b_top , MM_READWRITE) == 0)
            {
                return NULL;
            }
            for(int i = (4096/64)-1; i > 0; i--)
            {
                *(uint64_t*)(POOL_64B + pool_64b_top + (i * 64)) = pool_64b_firstfree;
                pool_64b_firstfree = POOL_64B + pool_64b_top + (i * 4096);
            }
            retval = (void*)(POOL_64B + pool_64b_top);
            pool_64b_top += 4096;
        }
        else
        {
            retval = (void*)pool_64b_firstfree;
            // The first free should now be the next element in the linked list
            pool_64b_firstfree = *(uint64_t*)pool_64b_firstfree;
        }
    }
    else if(size <= 512)
    {
        if(pool_512b_firstfree == NULL)
        {
            
            if(addmap(phymem_get_page(), POOL_512B + pool_512b_top , MM_READWRITE) == 0)
            {
                return NULL;
            }
            
            for(int i = (4096/512)-1; i > 0; i--)
            {
                *(uint64_t*)(POOL_512B + pool_512b_top + (i * 512)) = pool_512b_firstfree;
                pool_512b_firstfree = POOL_512B + pool_512b_top + (i * 4096);
            }
            
            retval = (void*)(POOL_512B + pool_512b_top);
            
            pool_512b_top += 4096;
        }
        else
        {
            retval = (void*)pool_512b_firstfree;
            // The first free should now be the next element in the linked list
            pool_512b_firstfree = *(uint64_t*)pool_512b_firstfree;
        }
    }
    // If we're bigger than 512b well allocate a full 4k page we 
    // don't check a linked list we just allocate the space and 
    // adjust the top address
    else if(size <= 4 * 1024)
    {
        if(addmap(phymem_get_page(), POOL_4K + pool_4k_top , MM_READWRITE) == 0)
        {
            return NULL;
        }
        retval = (void*)(POOL_4K + pool_4k_top);
        pool_4k_top += 4 * 1024;
        
    }
    else if(size <= 64 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < 64/4; i++)
        {
            uint64_t pagetemp = phymem_get_page();
            if(addmap(pagetemp, POOL_64K + pool_64k_top + (i * 4096), MM_READWRITE) == 0)
            {
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_64K + pool_64k_top + (i * 4096) ));
                    removemap( POOL_64K + pool_64k_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_64K + pool_64k_top);
        pool_64k_top += 64 * 1024;
    }
    else if(size <= 1 * 1024 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < (1*1024)/4; i++)
        {
            if(addmap(phymem_get_page(), POOL_1M + pool_1M_top + (i * 4096), MM_READWRITE) == 0)
            {
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_1M + pool_1M_top + (i * 4096) ));
                    removemap( POOL_1M + pool_1M_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_1M + pool_1M_top);
        pool_1M_top += (1*1024) * 1024;
    }
    else if(size <= 16 * 1024 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < (16*1024)/4; i++)
        {
            uint64_t temppage = phymem_get_page();
            //dprintf("%x\n",temppage);
            if(addmap(temppage, POOL_16M + pool_16M_top + (i * 4096), MM_READWRITE) == 0)
            {
                
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_16M + pool_16M_top + (i * 4096) ));
                    removemap( POOL_16M + pool_16M_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_16M + pool_16M_top);
        pool_16M_top += (16*1024) * 1024;
    }
    else if(size <= 64 * 1024 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < (64*1024)/4; i++)
        {
            if(addmap(phymem_get_page(), POOL_64M + pool_64M_top + (i * 4096), MM_READWRITE) == 0)
            {
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_64M + pool_64M_top + (i * 4096) ));
                    removemap( POOL_64M + pool_64M_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_64M + pool_64M_top);
        pool_64M_top += (64*1024) * 1024;
    }
    else if(size <= 128 * 1024 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < (128*1024)/4; i++)
        {
            if(addmap(phymem_get_page(), POOL_128M + pool_128M_top + (i * 4096), MM_READWRITE) == 0)
            {
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_128M + pool_128M_top + (i * 4096) ));
                    removemap( POOL_128M + pool_128M_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_128M + pool_128M_top);
        pool_128M_top += (128*1024) * 1024;
    }
    else if(size <= 512 * 1024 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < (512*1024)/4; i++)
        {
            if(addmap(phymem_get_page(), POOL_512M + pool_512M_top + (i * 4096), MM_READWRITE) == 0)
            {
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_512M + pool_512M_top + (i * 4096) ));
                    removemap( POOL_512M + pool_512M_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_512M + pool_512M_top);
        pool_512M_top += (512*1024) * 1024;
    }
    else if(size <= 1 * 1024 * 1024 * 1024)
    {
        // Map enough pages for the region
        for(int i = 0; i < (1024*1024)/4; i++)
        {
            if(addmap(phymem_get_page(), POOL_1G + pool_1G_top + (i * 4096), MM_READWRITE) == 0)
            {
                // If we can't map then free up the pages we succedded in mapping
                i--;
                for(;i > 0;i--)
                {
                    phymem_mark_free( getmap( POOL_1G + pool_1G_top + (i * 4096) ));
                    removemap( POOL_1G + pool_1G_top + (i * 4096) );
                }
                //  Then return a null pointer
                return NULL;
            }
        }
        retval = (void*)(POOL_1G + pool_1G_top);
        pool_1G_top += (1024*1024) * 1024;
    }
    return retval;
}


void free(void *ptr)
{
    if((uint64_t)ptr < POOL_64B || (uint64_t)ptr > POOL_MAX)
    {
        // Invalid  pointer to free
        
    }
    /* 64b and 512b pools will be used a lot,
     * we won't try and free physical pages for these
     * since there will be multiple pool elements in a page
     ************/
    else if((uint64_t)ptr < POOL_512B) // 64 byte ptr to free
    {
        // Use the freed memory to point to the next free memory
        *(uint64_t*)ptr = pool_64b_firstfree;
        pool_64b_firstfree = (uint64_t)ptr;
    }
    else if((uint64_t)ptr < POOL_4K) // 512 byte
    {
        // Use the freed memory to point to the next free memory
        *(uint64_t*)ptr = pool_512b_firstfree;
        pool_512b_firstfree = (uint64_t)ptr;
    }
    /* Below 4k we weren't page aligned, now we can free the physical pages */
    else if((uint64_t)ptr < POOL_64K) // 4K
    {
        
        phymem_mark_free( getmap( (uint64_t)ptr ) );
        removemap( (uint64_t) ptr );
    }
    else if((uint64_t)ptr < POOL_1M) // 64k
    {
        for(int i = 0; i < (64/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
    else if((uint64_t)ptr < POOL_16M) // 1M
    {
        for(int i = 0; i < (1024/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
    else if((uint64_t)ptr < POOL_64M) // 16M
    {
        for(int i = 0; i < ((16*1024)/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
    else if((uint64_t)ptr < POOL_128M) // 64M
    {
        for(int i = 0; i < ((64*1024)/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
    else if((uint64_t)ptr < POOL_512M) // 128M
    {
        for(int i = 0; i < ((128*1024)/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
    else if((uint64_t)ptr < POOL_1G)  // 512M
    {
        for(int i = 0; i < ((512*1024)/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
    else                    // 1G
    {
        for(int i = 0; i < ((1024*1024)/4); i++)
        {
            phymem_mark_free( getmap( (uint64_t)(ptr + (4096 * i) ) ));
            removemap( (uint64_t) ptr + (4096 * i) );
        }
    }
}





/*
int64_t curoffset = 0;
int64_t curpage = -1;

void *malloc(int64_t size)
{
    
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



void free(void __attribute__((unused)) *ptr)
{
    // 
}




*/



