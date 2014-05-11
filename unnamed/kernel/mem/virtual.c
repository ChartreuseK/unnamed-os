#include "virtual.h"



struct VirtualAddr
{
    void *PML4;                // Pointer to our PML4
    
    
    
} va;




typedef enum 
{
	P			 = 0,
	PT,
	PD,
	PDPT,
	
	PTE			 = 0,
	PDE,
	PDPTE,
	PML4E
} pageLevel;

typedef enum
{
	true = 0,
	false
} bool;

#define FAILURE 	0
#define SUCCESS 	1


void bootmap()
{
	//uint64_t oldpml4 = read_cr3();
	uint64_t newpml4 = phymem_get_page();
	
	
	for(int i = 0 ; i < 4096; i+=8)
	{
		phymem_write64( newpml4 + i, 0 );
	}
	
	// Map 512 1GB pages from 0x0 onward at 0xffffc00000000000+
	
	uint64_t temppage;
	
	for(int i = 0; i < 512; i++)
	{
		temppage = phymem_get_page();
		if(i == 0)
		{
			phymem_write64( newpml4 + (384 * 8), temppage );
		}
		
		phymem_write64(temppage + (i * 8), 
					0x40000000 * i | 1);
	}
	
	write_cr3(newpml4);
	
	/*for(int i = 0; i < 512; i++)
	{
		phymem_write64( newpml4 + i*8, phymem_read64(oldpml4 + i*8));
	}
	
	dprintf("Before cr3 write\n");
	
	write_cr3(newpml4);
	
	dprintf("Wrote new cr3: %x\n", read_cr3());*/
	
}




void removemap(uint64_t virtual)
{
	uint64_t curlevel, page;
	dprintf("PML4: %x\n",read_cr3());
	curlevel = phymem_read64( read_cr3() + (((virtual >> 39) % 512) * 8 ));	// PML4E
	dprintf("PML4E: %x\n",curlevel);
	curlevel = phymem_read64( curlevel   + (((virtual >> 30) % 512) * 8));  // PDPTE
	dprintf("PDPTE: %x\n",curlevel);
	curlevel = phymem_read64( curlevel   + (((virtual >> 21) % 512) * 8));	// PDE
	dprintf("PDE: %x\n",curlevel);
	page 	 = phymem_read64( curlevel   + (((virtual >> 12) % 512) * 8));	// PTE
	dprintf("PTE: %x\n",page);
	phymem_mark_free(page & ~0xFFF);
	
	// And now we zero out the PTE 
	phymem_write64( curlevel   + (((virtual >> 12) % 512) * 8), 0 ); 	
}


int addmap(uint64_t physical, uint64_t virtual, unsigned int flags)
{
	uint64_t curlevel, parent; int new = 0;
	
	
	uint64_t indicies[] = {
		(virtual >> 12) % 512,
		(virtual >> 21) % 512,
		(virtual >> 30) % 512,
		(virtual >> 39) % 512
	};
	
	
	// Check that our address's we're mapping are page aligned
	if( ( physical != (physical & ~0xFFF) ) ||
		( virtual  != (virtual  & ~0xFFF) ) )
	{
		return FAILURE;
	}
	
	
	
	parent = read_cr3();
	
	dprintf("CR3: %x\n", read_cr3());
	/* PML4       read_cr3()
	 * PDPT[512]    
	 * PD[512]
	 * PT[512]
	 * P
	 */
	
	for(int l = 3; l > 0; l--)
	{
		dprintf("In loop %x\n", l);
		curlevel = phymem_read64( (parent + indicies[l] * 8));
		dprintf("curlevel: %x\n", curlevel);
		if((curlevel & 0x1 ) == 0 || new)
		{
			dprintf("Current level not defined: %x\n", curlevel);
			dprintf("Parent: %x\n", parent);
			// Not present so create a new table
			curlevel = phymem_get_page();
			
			
			
			// Zero all entries
			for(register int i = 0; i < 4096; i+=8)
			{
				phymem_write64(curlevel + i, 0);
			}
			
			
			// Write our new entry into the parent, carry page flags all the way up 
			// non-existant branches on the tree			
			phymem_write64( (parent + indicies[l] * 8), 
						curlevel | flags | 1 );
					
			dprintf("Value written: %x\n", phymem_read64( (parent + indicies[l] * 8)));
					
			new = 1;  // We just created one so we need to create all the way down
		}
		parent = curlevel & ~0xFFF;
	}
	
	// Finally write our physical address to the Page Table Entry
	phymem_write64( (curlevel + indicies[0] * 8) , 
					physical | flags | 1 );
	
	return SUCCESS;
}







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
