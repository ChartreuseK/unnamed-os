#include "hw/ioapic.h"

uint64_t ioapic_base = NULL;


void setioapicbase()
{
    ioapic_base = 0x00000000FEC00000ULL;
}


uint32_t ioapic_read(uint32_t index)
{
    phymem_write32(ioapic_base, index);
    return phymem_read32(ioapic_base + 0x10);
}

void ioapic_write(uint32_t index, uint32_t value)
{
    phymem_write32(ioapic_base, index);
    phymem_write32(ioapic_base + 0x10, value);
}


// Not my code
void ioapic_set_irq(uint8_t irq, uint64_t apic_id, uint8_t vector) {
    uint32_t low_index = 0x10 + irq*2;
    uint32_t high_index = 0x10 + irq*2 + 1;


    uint32_t high = ioapic_read(high_index);
    // set APIC ID
    high &= ~0xff000000;
    high |= apic_id << 24;
    ioapic_write(high_index, high);

    uint32_t low = ioapic_read(low_index);

    // unmask the IRQ
    low &= ~(1<<16);

    // set to physical delivery mode
    low &= ~(1<<11);

    // set to fixed delivery mode
    low &= ~0x700;

    // set delivery vector
    low &= ~0xff;
    low |= vector;

    ioapic_write(low_index, low);
}
