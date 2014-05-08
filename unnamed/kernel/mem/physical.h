
#include <stdint.h>
#define PHY_MAP_BASE (void*)0xFFFFC00000000000ULL



#define NULL	0

void *phymem_get_page();
void phymem_mark_free(void *page);
void phymem_mark_all_free(uint64_t *regions);
void phymem_align_regions(uint64_t *regions);
