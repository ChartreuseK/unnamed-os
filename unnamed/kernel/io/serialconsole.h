
#include <stdint.h>
#include <stdarg.h>

#include "hw/ports.h"
#include "std/string.h"

void init_serial_console(int baudrate);
void dprintf(const char *msg, ...);
