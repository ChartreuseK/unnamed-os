#include <stdint.h>

#include "mem/physical.h"
#include "mem/virtual.h"
#include "mem/heap.h"
#include "mem/gdt.h"
#include "io/virtlineterm.h"
#include "hw/interrupts.h"
#include "io/serialconsole.h"
#include "std/string.h"
#include "threads/scheduler.h"

#define PHY_MAP_BASE     0xFFFFC00000000000ULL
#define REFK_PHY_MAP_BASE (uint8_t *)0xFFFFC00000000000ULL

#define TEXT_VRAM_BASE              0xB8000

#define BLK_ON_WHITE                (uint8_t)0xF0
#define WHT_ON_BLUE                 (uint8_t)0x1F
#define OFFSET(x,y)                 ((y<<7)+(y<<5)+(x<<1))

#define NULL						0

void text_clrscr();
void text_putxy(char *str, int x, int y, uint8_t attr);

extern uint8_t keycode_to_ascii[];

void kernel_main_thread();
void testthread();
void testthread2();
void inputthread();

void kmain(uint64_t  *mem) 
{
    


    // Init the serial console
    init_serial_console(115200);

    // Clear the screen
    //text_clrscr();
    
    phymem_align_regions(mem);
    phymem_mark_all_free(mem);

	// Write a \0 to address 0, so that a null pointer string is empty
	phymem_write8(NULL, 0);
	
    setup_gdt();

	free(malloc(0x4010));
	
    init_ps2_keyboard();
    
	

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

	init_threads();
    
	new_kthread(&kernel_main_thread, 4096);

	setup_interrupts();	
	while(1);
	
}


void kernel_main_thread()
{
	new_kthread(&testthread, 64);
	new_kthread(&testthread2, 64);
	new_kthread(&inputthread, 1024);
	
	// Now we put ourself to sleep since we have nothing left to do.
	sleep_thread_self();
	// Just incase we  somehow get out of sleep
	while(1){__asm__("hlt");}
}


void testthread()
{
	
	while(1);dprintf("I'm the first thread\n");
}
void testthread2()
{
	while(1);dprintf("I'm thread two\n");
}
void echothread()
{
	for(int i = 100; i > 0; i--) printf("I'm a thread %d\n",i);
	
}
void inputthread()
{
	char input[512];
    while(1) 
    {
		printf("> ");
		
		ngets(input, 512);
		
		
		char *command = strtok(input, " ");
		if(strcmp(command, "exit") == 0)
		{
			printf("Disabling interrupts and halting\n");
			__asm__("cli");
			__asm__("hlt");
		}
		else if(strcmp(command, "echo") == 0)
		{
			printf("%s\n", strtok(NULL, ""));
		}
		else if(strcmp(command, "thread") == 0)
		{
			printf("Creating new test thread\n");
			new_kthread(&echothread, 512);
		}
		
		
		
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

