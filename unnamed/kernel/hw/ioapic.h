
#include <stdint.h>

#include "mem/physical.h"

void ioapic_write(uint32_t index, uint32_t value);
uint32_t ioapic_read(uint32_t index);
void setioapicbase();
void ioapic_set_irq(uint8_t irq, uint64_t apic_id, uint8_t vector);
