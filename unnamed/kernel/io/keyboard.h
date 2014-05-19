
#include <stdint.h>
#include "hw/ports.h"


uint8_t init_ps2_keyboard();
void key_event();
uint8_t key_buff_get_blk();
