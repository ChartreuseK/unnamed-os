#include "physical.h"

// Don't allow the use of ram below 2MB to be treated as free 
#define PROTECTED_RAM    0x20000

// Local prototypes
void set_next_page(void *page, void *next);
void *get_next_page(void *page);


extern void dprintf(const char *msg, ...);
extern void dinit();


/* Sidenote remember not to allow the first page so that NULL doesn't become valid */
void *next_free_page = NULL;


/* The addresses of these symbols are the start of where the 
 * kernel is loaded and the end of where it is loaded.
 */
extern char kernel_pbase;
extern char _data_end; 


/* phymem_align_regions
 *  Aligns (rounds) the regions of physical memory reported by multiboot
 *  to be multiples of 4kB (page aligned)
 *
 *  regions contains pairs of numbers representing the lower bound and size
 *  ie:
 *   regions[0] = lower_bound
 *   regions[1] = size
 *  We want to round up the lower_bound to be page aligned
 *  and round down the size to be page aligned.
 */
void phymem_align_regions(uint64_t *regions)
{
    for(; *regions != NULL || *(regions + 1) != NULL; regions = regions + 2)
    {
        // If any of the bottom 12 bits are set the add will overflow those up to the next
        // page and the AND will bitclear the bottom 12 bits, rounding it up to the next page.
        *regions = (*regions + 0xFFF) & ~(0xFFF);

        // For the size simply round it down by clearing the lower 12 bits
        *(regions + 1) = *(regions + 1) & ~(0xFFF);
    }
}





/*
 * For now we will keep track of free pages by storing in their first 8 bytes 
 * the address of the next available free page. We do this by first going through
 * all the pages in RAM (besides our kernel) and marking them as free.
 */
void phymem_mark_all_free(uint64_t *regions)
{
	uint64_t i;
	dprintf("Starting to mark free\n");
    for(; *regions != NULL || *(regions + 1) != NULL; regions = regions + 2)
    {
		// Go through the region in steps of 4KB
		for(i = 0; i < *(regions + 1); i += 0x1000) 
		{
			// If we're looking at RAM below 2MB 
			if(( *regions + i )  <=  PROTECTED_RAM) continue;
			// If we're looking at memory in use by the kernel (where we are loaded)
			if(( *regions + i )  >=  (uint64_t)&kernel_pbase &&
			   ( *regions + i )  <=  (uint64_t)&_data_end) continue;
			 
			// If not then we can mark the page as free and add it to the linked list
			
			phymem_mark_free( regions + i );
			dprintf("Marked: %x\r", (uint64_t)regions + i);
		}
    }

}


void phymem_mark_free(void *page)
{
	set_next_page(page, next_free_page);
	
	next_free_page = page;
}

/* 
 * Uses the linked list we have of the next free page to return the first page available.
 */
void *phymem_get_page()
{
	void *page;
	
	page = next_free_page;
	
	next_free_page = get_next_page(next_free_page);
	
	return page;
}


void *get_next_page(void *page)
{
	return (void*)*((uint64_t*)PHY_MAP_BASE + (uint64_t)page);
}


void set_next_page(void *page, void *next)
{
	*((uint64_t*)PHY_MAP_BASE + (uint64_t)page) = (uint64_t)next;	
}