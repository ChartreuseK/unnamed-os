
#include "io/serialconsole.h"


#define COM1_PORT     	0x3F8

// When DLAB bit is clear
#define DATA_OFFSET         0x0
#define IER_OFFSET          0x1     

// When DLAB bit is set
#define DIVISOR_LSB_OFFSET  0x0
#define DIVISOR_MSB_OFFSET  0x1

#define INT_AND_FIFO_OFFSET 0x2
#define LINE_CTRL_OFFSET    0x3     // Most significant bit is DLAB
#define MODEM_CTRL_OFFSET   0x4
#define LINE_STATUS_OFFSET  0x5
#define MODEM_STATUS_OFFSET 0x6
#define SCRATCH_OFFSET      0x7

#define BIT(n)              (1<<(n))

// 115200 = divisor 1
//  57600 = divisor 2
//  38400 = divisor 3 


void init_serial_console(int baudrate)
{
    uint16_t divisor = (uint16_t)(115200 / baudrate);
    
    // Disable interrupts (we only intend to output)
    outportb(COM1_PORT + IER_OFFSET, 0);
    
    
    // Swap to DLAB so we can set the baud rate
    outportb(COM1_PORT + LINE_CTRL_OFFSET, BIT(7));
    
    // Now set the baud rate
    outportb(COM1_PORT + DIVISOR_LSB_OFFSET, (uint8_t)(divisor & 0xFF));
    outportb(COM1_PORT + DIVISOR_MSB_OFFSET, (uint8_t)(divisor >> 8));
    
    // Now disable DLAB, and set us to 8 data bits, no parity, 1 stop bit (8N1)
    outportb(COM1_PORT + LINE_CTRL_OFFSET, BIT(1) | BIT(0));
    
    // Clear the transmit and recieve FIFOs
    outportb(COM1_PORT + INT_AND_FIFO_OFFSET, BIT(1) | BIT(2));
    
    // Should be good now
    
    
}


uint8_t readbyte()
{
    return inportb(COM1_PORT + DATA_OFFSET);
}
void putbyte(uint8_t byte)
{
    // Wait until the UART can accept a byte
    while( !(inportb(COM1_PORT + LINE_STATUS_OFFSET) & BIT(5)) );
    
    outportb(COM1_PORT + DATA_OFFSET, byte);
}



void dprintf(const char *msg, ...) 
{
    
    va_list va;
    va_start(va, msg);

    char str[32];
    char *s;
    int num;
    uint32_t unum;
    uint64_t unum64;
    int64_t num64;
    
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
                putbyte('%');
                break;
            case 'p':       // Pointer fixed length %llx
                unum64 = (uint64_t)va_arg(va, uint64_t);
                uitoa(unum64, str, 16);
                s = str;
                for(int i = strlen(str); i <= 16; i++)
                {
                    putbyte('0');
                }
                while(*s)
                {
                    putbyte(*s++);
                }
                break;
            case 'b':       // Not standard but binary is nice
                unum = (uint32_t)va_arg(va, uint32_t);
            
                uitoa(unum, str, 2);
                s = str;
                while(*s)
                {
                    putbyte(*s++);
                }
                break;
            case 'o':
                unum = (uint32_t)va_arg(va, uint32_t);
            
                uitoa(unum, str, 8);
                s = str;
                while(*s)
                {
                    putbyte(*s++);
                }
                break;
                
            case 'u':
                unum = (uint32_t)va_arg(va, uint32_t);
                
                uitoa(unum, str, 10);
                s = str;
                while(*s)
                {
                    putbyte(*s++);
                }
                break;   
            case 'i':
            case 'd':
                num = (int)va_arg(va, int);
                
                itoa(num, str, 10);
                s = str;
                while(*s)
                {
                    putbyte(*s++);
                }
                break;
            case 's':
                s = va_arg(va, char *);
                while(*s)
                {
                    putbyte(*s++);
                }
                break;
            case 'x':
                num = (int)va_arg(va, int);
                itoa(num, str, 16);
                s = str;
                while(*s)
                {
                    putbyte(*s++);
                }
                break;
            
            case 'c':
                // With variable arguments chars are stored as ints
                putbyte( (char)va_arg(va, int) ); 
                break;
             
            case 'l':   // 64bit number expected
                if(*(fmt+2) == 'l')
                {
                    switch(*(fmt+3))
                    {
                    case 'b': // Not standard but binary is nice
                        unum64 = (uint64_t)va_arg(va, uint64_t);
                    
                        uitoa(unum64, str, 2);
                        s = str;
                        while(*s)
                        {
                            putbyte(*s++);
                        }
                        break;
                    case 'o':
                        unum64 = (uint64_t)va_arg(va, uint64_t);
                    
                        uitoa(unum64, str, 8);
                        s = str;
                        while(*s)
                        {
                            putbyte(*s++);
                        }
                        break;
                    case 'i':
                    case 'd':
                        num64 = (int64_t)va_arg(va, int64_t);
                    
                        itoa(num64, str, 10);
                        s = str;
                        while(*s)
                        {
                            putbyte(*s++);
                        }
                        break;
                    case 'u':
                        unum64 = (uint64_t)va_arg(va, uint64_t);
                    
                        uitoa(unum64, str, 10);
                        s = str;
                        while(*s)
                        {
                            putbyte(*s++);
                        }
                        break;
                    case 'x':
                        unum64 = (uint64_t)va_arg(va, uint64_t);
                        uitoa(unum64, str, 16);
                        s = str;
                        while(*s)
                        {
                            putbyte(*s++);
                        }
                        break;
                        
                    }
                }
                fmt+=2;
                break;
                
            }
            fmt += 2;
        }
        else
        {
            switch(*fmt)
            {
            case '\n':
                putbyte('\r');
                putbyte('\n');
                break;
            default:
                putbyte(*fmt);
                
            }
            fmt++;
        }
    }
    va_end(va);
}
