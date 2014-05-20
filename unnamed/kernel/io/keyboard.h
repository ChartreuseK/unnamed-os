
#include <stdint.h>
#include "hw/ports.h"
#include "io/serialconsole.h"

uint8_t init_ps2_keyboard();
void key_event();
uint8_t key_buff_get_blk();
extern  uint8_t keycode_to_ascii[];
