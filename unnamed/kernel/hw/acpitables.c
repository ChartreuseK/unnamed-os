
#include "hw/acpitables.h"



extern void dprintf(const char *msg, ...);




uint64_t find_rsdp()
{
    register uint64_t i;
    uint8_t checksum = 0;
    
    // Since the first 2mb is identity mapped for us we don't
    // need to change the addresses
    for(i = 0xe0000; i < 0xfffff; i+=0x10)
    {
        if(phymem_read64(i) == 0x2052545020445352ULL) // "RSD PTR "0x5253442050545220ULL
        {                      
            // We found the RSDP
            // Just to be sure let's check the checksum
            checksum = 0;
            for(int j = 0; j < 20; j++)
            {
                checksum += *(uint8_t*)(i + j);
            }
            if(checksum != 0)
            {
                dprintf("RSDP failed checksum! Looking for another\n");
                continue;
            }
            //dprintf("Found RSDP! %x\n", i);
            // So we're now sure enough that this is the RSDP
            // Now we can return it's address
            return i;
        }
    }
    if(i >= 0xfffff)
    {
        // We didn't find the rdsp...
        dprintf("Failed to find rsdp!!!\n");
        // Probably should check in the EBDA here
    }
    return 0;
}

uint64_t get_rsdt(uint64_t rsdp)
{
    //dprintf("rsdt: %x\n", phymem_read32(rsdp + 16));
    return phymem_read32(rsdp + 16);
}


uint64_t find_apic_table(uint64_t rsdt)
{
    uint64_t apic_table = 0;
    for(unsigned int i = 0; i < phymem_read32(rsdt + 4); i+=4)
    {
        apic_table = phymem_read32(rsdt + 36 + i);
        if(phymem_read32(apic_table) == 0x43495041) // "APIC" 
        {   
            // This is the APIC table
            //dprintf("Found apic table: %x\n", apic_table);
            return apic_table;
        }
        
    }
    
    dprintf("Failed to find APIC table\n");
    
    
    return 0;
    
}


uint64_t find_first_ioapic_addr(uint64_t apic_table)
{
    uint64_t ioapic = 0;
    // Entries in the apic table are a packed list of structures, not pointers
    
    for(unsigned int i = 0; i < phymem_read32(apic_table + 4);)
    {
        if(phymem_read8(apic_table + 44 + i) == 1) // We're a I/O APIC
        {
            if(phymem_read32(apic_table + 44 + i + 8) == 0) // Looking for the I/O APIC with a base IRQ of 0
            {
                ioapic = phymem_read32(apic_table + 44 + i + 4);
                //dprintf("Found first ioapic: %x\n", ioapic);
                return ioapic;
            }
        }
        // Don't bother with local APICs just yet (will be needed for SMP)
        
        
        i += phymem_read8(apic_table + 44 + i + 1);
    }
    
    return 0;
}
