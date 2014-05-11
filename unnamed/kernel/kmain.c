#include <stdint.h>

#include "mem/physical.h"
#include "mem/virtual.h"
#include "mem/heap.h"

#define PHY_MAP_BASE     0xFFFFC00000000000ULL
#define REFK_PHY_MAP_BASE (uint8_t *)0xFFFFC00000000000ULL

#define TEXT_VRAM_BASE              0xB8000

#define BLK_ON_WHITE                (uint8_t)0xF0
#define WHT_ON_BLUE                 (uint8_t)0x1F
#define OFFSET(x,y)                 ((y<<7)+(y<<5)+(x<<1))

void text_clrscr();
void text_putxy(char *str, int x, int y, uint8_t attr);


extern void dprintf(const char *msg, ...);
extern void dinit();

void kmain(uint64_t  *mem) 
{
    char *hello = "Hello world\0";


    // Init the serial console
    dinit();

    // Clear the screen
    text_clrscr();

    text_putxy(hello, 0, 0, WHT_ON_BLUE);
    
    phymem_align_regions(mem);
    phymem_mark_all_free(mem);


	//bootmap();

	uint64_t page = phymem_get_page();


	
	addmap(page, 0x000000000d000000, MM_READWRITE);
    char *test2 = (char*)0x000000000d000000;
    
    dprintf("Before mem write\n");
    
    test2[0] = 'H';
    test2[1] = 'e';
    test2[2] = 'l';
    test2[3] = 'l'; 
    test2[4] = 'o';
    test2[5] = '\0';
    
    dprintf("After mem write\n");
    
    text_putxy(test2, 0,2, WHT_ON_BLUE);
    
    text_putxy((char*)PHY_MAP_BASE + page, 0, 3, WHT_ON_BLUE);
    
    while(1) {}
}

void text_putxy(char *str, int x, int y, uint8_t attr)
{
    volatile uint8_t *scr;
    char *cur;

    scr = (REFK_PHY_MAP_BASE + TEXT_VRAM_BASE + OFFSET(x,y));

    for(cur = str; *cur != 0; cur++)
    {
        *(scr++) = *cur;
        *(scr++) = attr;
    }
}

void text_clrscr()
{
    volatile uint8_t *scr;
    uint32_t i;

    scr = (REFK_PHY_MAP_BASE + TEXT_VRAM_BASE);

    for (i = 0; i < 80 * 25 * 2; i+=2)
    {
        *(scr + i) = 0;
        *(scr + i + 1) = WHT_ON_BLUE;

    }

}

