#ifndef _PHYSICAL_H
#define _PHYSICAL_H


#include <stdint.h>
#include "io/serialconsole.h"

#define PHY_MAP_BASE     0xFFFFC00000000000ULL

#define NULL    0

uint64_t phymem_get_page();
void phymem_mark_free(uint64_t page);
void phymem_mark_all_free(uint64_t *regions);
void phymem_align_regions(uint64_t *regions);




static inline void phymem_write64(uint64_t addr, uint64_t val)
{
    *((uint64_t*)(PHY_MAP_BASE + addr)) = val;    
}
static inline void phymem_write32(uint64_t addr, uint32_t val)
{
    *((uint32_t*)(PHY_MAP_BASE + addr)) = val;    
}
static inline void phymem_write16(uint64_t addr, uint16_t val)
{
    *((uint16_t*)(PHY_MAP_BASE + addr)) = val;    
}
static inline void phymem_write8(uint64_t addr, uint8_t val)
{
    *((uint8_t*)PHY_MAP_BASE + addr) = val;    
}

static inline uint64_t phymem_read64(uint64_t addr)
{
    return *((uint64_t*)(PHY_MAP_BASE + addr));
}
static inline uint32_t phymem_read32(uint64_t addr)
{
    return *((uint32_t*)(PHY_MAP_BASE + addr));
}
static inline uint16_t phymem_read16(uint64_t addr)
{
    return *((uint16_t*)(PHY_MAP_BASE + addr));
}
static inline uint8_t phymem_read8(uint64_t addr)
{
    return *((uint8_t*)PHY_MAP_BASE + addr);
}


#endif
