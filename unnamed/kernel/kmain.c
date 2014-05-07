#include <stdint.h>

#define REFK_PHY_MAP_BASE (volatile uint8_t *)0xFFFFC00000000000ULL

#define TEXT_VRAM_BASE              0xB8000

#define BLK_ON_WHITE                (uint8_t)0xF0

#define OFFSET(x,y)                 ((y<<7)+(y<<5)+(x<<1))

void text_clrscr();
void text_putxy(char *str, int x, int y, uint8_t attr);

void kmain(uint64_t __attribute__((unused)) *mem) 
{
    char *hello = "Hello world\0";

    // Clear the screen
    text_clrscr();

    text_putxy(hello, 2, 2, BLK_ON_WHITE);


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
        *(scr + i + 1) = BLK_ON_WHITE;

    }

}
