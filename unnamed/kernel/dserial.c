#include <stdarg.h>
#include <stdint.h>

#define PORT 0x3f8

uint8_t dio_in(uint16_t port) {
    uint8_t ret;
    __asm__("in %%al, (%%dx)" : "=a"(ret) : "d"(port));
    return ret;
}

void dio_out(uint16_t port, uint8_t value) {
    __asm__("outb (%%dx), %%al" : : "a"(value), "d"(port));
}

void dinit() {
    dio_out(PORT + 1, 0x00);
    dio_out(PORT + 3, 0x80);
    dio_out(PORT + 0, 0x03);
    dio_out(PORT + 1, 0x00);
    dio_out(PORT + 3, 0x03);
    dio_out(PORT + 2, 0xc7);
    dio_out(PORT + 4, 0x0b);
}

void dputchar(char c) {
    while((dio_in(PORT + 5) & 0x20) == 0) ;

    dio_out(PORT, c);
}

void dprintf(const char *msg, ...) {
    va_list va;
    va_start(va, msg);

    const char *p = msg;
    while(*p) {
        if(*p == '\n') {
            dputchar('\n');
            dputchar('\r');
            p ++;
        }
        else if(*p == '%' && *(p+1)) {
            p ++;
            if(*p == 's') {
                const char *s = va_arg(va, const char *);
                while(*s) {
                    dputchar(*s);
                    s ++;
                }
                break;
            }
            else if(*p == 'x') {
                uint64_t v = va_arg(va, uint64_t);
                for(int shift = 60; shift >= 0; shift -= 4) {
                    uint8_t vv = (v >> shift) & 0xf;
                    dputchar("0123456789abcdef"[vv]);
                }
            }

            p ++;
        }
        else {
            dputchar(*p);
            p ++;
        }
    }

    va_end(va);
}
