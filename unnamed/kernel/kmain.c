#include <stdint.h>

#include "mem/physical.h"
#include "mem/virtual.h"
#include "mem/heap.h"
#include "mem/gdt.h"
#include "io/virtlineterm.h"
#include "hw/interrupts.h"
#include "io/serialconsole.h"
#include "std/string.h"

#define PHY_MAP_BASE     0xFFFFC00000000000ULL
#define REFK_PHY_MAP_BASE (uint8_t *)0xFFFFC00000000000ULL

#define TEXT_VRAM_BASE              0xB8000

#define BLK_ON_WHITE                (uint8_t)0xF0
#define WHT_ON_BLUE                 (uint8_t)0x1F
#define OFFSET(x,y)                 ((y<<7)+(y<<5)+(x<<1))

void text_clrscr();
void text_putxy(char *str, int x, int y, uint8_t attr);

extern uint8_t keycode_to_ascii[];

void kmain(uint64_t  *mem) 
{
    


    // Init the serial console
    init_serial_console(115200);

    // Clear the screen
    //text_clrscr();
    
    phymem_align_regions(mem);
    phymem_mark_all_free(mem);

	
    setup_gdt();

	free(malloc(0x4010));
	
    init_ps2_keyboard();
	setup_interrupts();

	dprintf("Hello world!\n");

    initscreen();
    printf("Virtual Line Terminal:\n");
    
    
    /*
    printf("Beginning memory test\n");
    for(int d = 0; d < 64; d++)
    {
		void *test2 = malloc(4096); // 4096bytes
		printf("%p    ", (uint64_t)test2);
		
		*(char*)test2 = 'a';
		free(test2);
	}
	printf("\nDone memory test\n");
	*/


	char temp;

    while(1) 
    {
		printf("%c", keycode_to_ascii[key_buff_get_blk()]);
	}
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

