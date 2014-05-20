#include <stdarg.h>
#include <stdint.h>
#include "io/keyboard.h"
#include "mem/memtools.h"
#include "mem/physical.h"
#include "std/string.h"
#include "hw/ports.h"

void initscreen();
void putstr(const char *str);
void putchar(char c);
void printf(const char *msg, ...);

