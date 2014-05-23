#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>
#include "hw/ports.h"
#include "io/serialconsole.h"
#include "io/virtlineterm.h"
uint8_t init_ps2_keyboard();
void key_event();
uint16_t key_buff_get_blk();
extern  uint8_t keycode_to_ascii[];
uint16_t getchar();
void ngets(char *str, int len);
uint16_t getchare();


#endif
