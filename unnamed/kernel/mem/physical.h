
#include <stdint.h>




#define NULL	0

uint64_t phymem_get_page();
void phymem_mark_free(uint64_t page);
void phymem_mark_all_free(uint64_t *regions);
void phymem_align_regions(uint64_t *regions);
void phymem_write64(uint64_t addr, uint64_t val);
void phymem_write8(uint64_t addr, uint8_t val);
uint64_t phymem_read64(uint64_t addr);
uint8_t phymem_read8(uint64_t addr);
