#ifndef _ACPITABLES_H
#define _ACPITABLES_H

#include <stdint.h>
#include "mem/physical.h"





uint64_t find_first_ioapic_addr(uint64_t apic_table);
uint64_t find_apic_table(uint64_t rsdt);
uint64_t get_rsdt(uint64_t rsdp);
uint64_t find_rsdp();


#endif
