/*
 * Goal here is to provide a simple 80x25 line terminal 
 * no scrollback, nothing fancy. Support for \r \n \t 
 * Will have a direct putchar method. Puts and printf 
 * will call the putchar method. No support for colors
 * or attributes, but will update the cursor. Will have
 * a blocking getchar
 * 
 * 
 * TODO: Need to go and write a memcpy function
 *       Scrolling - line by line copy back or have a pointer to
 *                   the current top line and roll around...
 * 
 * 
 *************************/

#include "io/virtlineterm.h"

#define PHY_MAP_BASE                0xFFFFC00000000000ULL
#define TEXT_VRAM_BASE              0xB8000
#define BLK_ON_WHITE                (uint8_t)0xF0
#define WHT_ON_BLUE                 (uint8_t)0x1F


#define COLOR                       (uint8_t)0x1F       // White on blue

#define WIDTH                       80
#define HEIGHT                      25

#define OFFSET(x,y)                 (((y)<<7)+((y)<<5)+((x)<<1))

struct Screen {
    uint16_t map[HEIGHT][WIDTH];   // We'll store both the char and the attr for a fast copy
    uint8_t curx;
    uint8_t cury;
        
    
} scr;

void raw_clrscr();
void draw_screen();
void putchar_nodraw(char c);
void move_cursor(int x, int y);;



void initscreen()
{
    //raw_clrscr();
    
    // Zero out our buffer
    for(int y = 0; y < HEIGHT; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
            scr.map[y][x] = (COLOR << 8);
        }
    }
    
    scr.curx = 0;
    scr.cury = HEIGHT - 1;
    
    draw_screen();
    
}

void draw_screen()
{
    for(int y = 0; y < 25; y++)
    {
        /*for(int x = 0; x < 80; x++)
        {
            *(uint16_t*)(PHY_MAP_BASE + TEXT_VRAM_BASE + OFFSET(x,y)) = scr.map[(scr.cury + 1 + y) % HEIGHT][x];
        }*/
        memcpy((void *)(PHY_MAP_BASE + TEXT_VRAM_BASE + OFFSET(0,y)), scr.map[ (scr.cury + 1 + y) % HEIGHT ], WIDTH * 2);
    }
    
    move_cursor(scr.curx, HEIGHT - 1);
}

void putchar(char c)
{
    putchar_nodraw(c);
    draw_screen();
}
void putchar_nodraw(char c)
{
    switch(c)
    {
    case '\r':
        scr.curx = 0;
        break;
    case '\n':
        scr.cury = (scr.cury + 1) % HEIGHT;
        scr.curx = 0;
        for(int i = 0; i < WIDTH; i++)
        {
            scr.map[scr.cury][i] = (COLOR << 8);
        }
        break;
    case '\b':
        if(scr.curx == 0)
        {
            scr.curx = WIDTH;
            scr.cury--;
        }
        scr.map[scr.cury][--scr.curx] = (COLOR << 8);
        break;
    case '\t':
        break;
    default:
        if(c == 0) break;
        
        if(scr.curx == WIDTH)
        {
            scr.cury = (scr.cury + 1) % HEIGHT;
            scr.curx = 0;
        }
        scr.map[scr.cury][scr.curx++] = (COLOR << 8) | c;
    }
}

void putstr(const char *str)
{
    while(*str)
    {
        putchar_nodraw(*str++);
    }
        
    draw_screen();
}





// A very simple printf
void printf(const char *msg, ...) 
{
    
    va_list va;
    va_start(va, msg);

    char str[32];
    char *s;
    int num;
    
    const char *fmt = msg;
    
    // Loop through our string till we hit the null terminator
    while(*fmt)
    {
        // If we encounter a % and it's not the last character
        if( *fmt == '%' && *(fmt + 1) )
        {
            switch(*(fmt + 1))
            {
            case '%':
                putchar_nodraw('%');
                break;
                
            case 'd':
                num = (int)va_arg(va, int);
                
                itoa(num, str, 10);
                s = str;
                while(*s)
                {
                    putchar_nodraw(*s++);
                }
                break;
            case 's':
                s = va_arg(va, char *);
                while(*s)
                {
                    putchar_nodraw(*s++);
                }
                break;
            case 'x':
                num = (int)va_arg(va, int);
                itoa(num, str, 16);
                s = str;
                while(*s)
                {
                    putchar_nodraw(*s++);
                }
                break;
            
            case 'c':
                // With variable arguments chars are stored as ints
                putchar_nodraw( (char)va_arg(va, int) ); 
                break;
                
            }
            fmt += 2;
        }
        else
        {
            putchar_nodraw(*fmt);
            fmt++;
        }
    }
            
            
    draw_screen();
        
    
    va_end(va);
}









void raw_clrscr()
{
    for (uint32_t i = 0; i < WIDTH * HEIGHT * 2; i+=2)
    {
        phymem_write16(TEXT_VRAM_BASE + i, (COLOR << 8));
    }

}




void move_cursor(int x, int y)
{
    // We want to access the cursor low byte port
    outportb(0x3D4, 0x0F);
    // Write to the low byte
    outportb(0x3D5, (uint8_t)((y * WIDTH) + x));
    // We want to access the cursor high byte port
    outportb(0x3D4, 0x0E);
    // Write to the high byte
    outportb(0x3D5, (uint8_t)(((y * WIDTH) + x) >> 8));
}
