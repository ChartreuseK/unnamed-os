

#include "io/keyboard.h"

#define BIT(num)        (1<<(num))

#define PS2_DATA_PORT   0x60
#define PS2_STATUS_PORT 0x64
#define PS2_CMD_PORT    0x64


#define PS2_CMD_DISABLE_FIRST       0xAD
#define PS2_CMD_ENABLE_FIRST        0xAE
#define PS2_CMD_DISABLE_SECOND      0xA7
#define PS2_CMD_ENABLE_SECOND       0xA8

#define PS2_CMD_READ_CFG            0x20
#define PS2_CMD_WRITE_CFG           0x60

#define PS2_CMD_TEST_CTRLR          0xAA
#define PS2_CMD_TEST_FIRST          0xAB
#define PS2_CMD_TEST_SECOND         0xA9

#define KBD_CMD_RESET               0xFF
#define KBD_RESET_PASSED            0xAA    // Conflicting documentation either AA or FA

#define TIMEOUT_KEYBOARD_SEND       20000


uint8_t keystatus[256]; 


/* Keycodes will be defined based on the row and column of the key
 * on a standard 101-key US QWERTY keyboard. With the top left corner
 * being Row 0 Column 0.
 * 
 *  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *  |<-- Row -->|<----- Column ---->|
 * 
 * Extra keys will be as needed placed on row 5+
 * 
 * 
 * |00|  |01|02|03|04|  |05|06|07|08|  |09|0a|0b|0c|  |0d|0e|0f|
 * 
 * |20|21|22|23|24|25|26|27|28|29|2a|2b|2c|   2d   |  |2e|2f|30|   |31|32|33|34|
 * |40 |41|42|43|44|45|46|47|48|49|4a|4b|4c|  4d   |  |4e|4f|50|   |51|52|53|54|
 * | 60 |61|62|63|64|65|66|67|68|69|6a|6b|    6c   |               |6d|6e|6f|__|
 * |  80 |81|82|83|84|85|86|87|88|89|8a|    8b     |     |8c|      |8d|8e|8f|90|
 * | a0 |   | a1 |      a2         | a3 |     | a4 |  |a5|a6|a7|   | a8  |a9|__|
 * 
 *****/
 
// Bit 0 - Shift, Bit 1 - Alt, Bit 2 - Ctrl, Bit 3 - Caps lock, Bit 4 - Scroll lock
// Bit 5 - Num lock
uint8_t modifiers = 0;



uint8_t keycode_to_ascii[] = // Gives 0 if unprintable
    {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         // 0x0F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         // 0x1F
    '`','1','2','3','4','5','6','7','8','9','0','-','=','\b',0,0, // 0x2F
    0,0,'/','*','-',0,0,0,0,0,0,0,0,0,0,0,                  // 0X3F
    0,'q','w','e','r','t','y','u','i','o','p','[',']','\\',0,0, // 0x4F
    0,'7','8','9','+',0,0,0,0,0,0,0,0,0,0,0,                // 0x5F
    0,'a','s','d','f','g','h','j','k','l',';','\'','\n','4','5','6', // 0x6F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        // 0x7F
    0,'z','x','c','v','b','n','m',',','.','/',0,0,'1','2','3', //0x8F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        // 0x9F
    0,0,' ',0,0,0,0,0,'0','.',0,0,0,0,0,0                   // 0xAF
};
        
uint8_t upperkeycode_to_ascii[] = // Gives 0 if unprintable
    {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         // 0x0F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         // 0x1F
    '~','!','@','#','$','%','^','&','*','(',')','_','+','\b',0,0, // 0x2F
    0,0,'/','*','-',0,0,0,0,0,0,0,0,0,0,0,                  // 0X3F
    0,'Q','W','E','R','T','Y','U','I','O','P','{','}','|',0,0, // 0x4F
    0,'7','8','9','+',0,0,0,0,0,0,0,0,0,0,0,                // 0x5F
    0,'A','S','D','F','G','H','J','K','L',':','\"','\n','4','5','6', // 0x6F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        // 0x7F
    0,'Z','X','C','V','B','N','M','<','>','?',0,0,'1','2','3', //0x8F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        // 0x9F
    0,0,' ',0,0,0,0,0,'0','.',0,0,0,0,0,0                   // 0xAF
};



uint8_t single_scan_to_key[] = 
    {
    0xFF, // Not used
    0x00, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,        // 0x07
    0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x40,  // 0x0F
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,  // 0x17
    0x49, 0x4a, 0x4b, 0x4c, 0x6c, 0xa0, 0x61, 0x62,  // 0x1F
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,  // 0x27
    0x6b, 0x20, 0x80, 0x4d, 0x81, 0x82, 0x83, 0x84,  // 0x2F
    0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x33,  // 0x37
    0xa1, 0xa2, 0x60, 0x01, 0x02, 0x03, 0x04, 0x05,  // 0x3F
    0x06, 0x07, 0x08, 0x09, 0x0a, 0x31, 0x0e, 0x51,   // 0x47
    0x52, 0x53, 0x34, 0x6d, 0x6e, 0x6f, 0x54, 0x8d,   // 0x4F
    0x8e, 0x8f, 0xa8, 0xa9, 0xFF, 0xFF, 0xFF, 0x0b,   // 0x57
    0x0e // 0x58, 
    };
    
    
// These scancodes are prefixed with a 0xE0 byte
uint8_t double_scan_to_key[] =
    {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x07
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x0F
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x17
    0xFF, 0xFF, 0xFF, 0xFF, 0x90, 0xa4, 0xFF, 0xFF, // 0x1F
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x27
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x2F
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x32, 0xFF, 0xFF, // 0x37
    0xa3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x3f
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2f, // 0x47
    0x8c, 0x30, 0xFF, 0xa5, 0xFF, 0xa7, 0xFF, 0x4f, // 0x4F
    0xa6, 0x50, 0x2e, 0x4e // 0x53
    };
    
    
// Finally there's 2 special ones
// Printscr pressed - 0xE0, 0x2A, 0xE0, 0x3f
//          released- 0xE0, 0xB7, 0xE0, 0xAA
// Pause pressed    - 0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5
// No pause released
        
uint8_t prev_scan_buf[6];
uint8_t prev_count = 0;


#define KEY_BUFF_SIZE       256

volatile uint16_t key_buff[KEY_BUFF_SIZE];
volatile int key_buff_count = 0;

void key_buff_add(uint8_t keycode)
{
    switch(keycode)
    {
    case 0x60: // Capslock
        modifiers ^= BIT(3);
        break;    
    case 0x80: // Shift
    case 0x8b:
        // Toggle shift untill we implement key release
        modifiers |=  BIT(0);
        break;
    case 0xa0: // Ctrl
    case 0xa4:
        modifiers |=  BIT(2);
        break;    
    case 0xa1: // Alt
    case 0xa3:
        modifiers |=  BIT(1);
        break;    
    case 0x0e: // Scroll lock
        modifiers ^= BIT(4);
        break;    
    case 0x31: // Num lock
        modifiers ^= BIT(5);
        break;

    default:
        if(key_buff_count < (KEY_BUFF_SIZE - 1))
        {
            key_buff[key_buff_count++] = (modifiers << 8) + keycode;
        }
    }
}

uint16_t key_buff_get_blk()
{
    // Wait till we get something in our buffer
    while(key_buff_count == 0){}
  
    return key_buff[--key_buff_count];
    
    
}

uint16_t getchar() // Lower byte is the ascii char, upper byte is modifier
{
    
    uint16_t keycode = key_buff_get_blk();
    
    
    if( keycode & BIT(8) ) // Shift bit
    {
        return upperkeycode_to_ascii[(uint8_t)keycode];
        
    }
    else
    {
        return keycode_to_ascii[(uint8_t)keycode];
    }
    
}
uint16_t getchare() // Lower byte is the ascii char, upper byte is modifier
{
    uint16_t keycode = key_buff_get_blk();
    if( keycode & BIT(8) ) // Shift bit
    {
        putchar(upperkeycode_to_ascii[(uint8_t)keycode]);
        return upperkeycode_to_ascii[(uint8_t)keycode];
        
    }
    else
    {
        putchar(keycode_to_ascii[(uint8_t)keycode]);
        return keycode_to_ascii[(uint8_t)keycode];
    }
    
}

void ngets(char *str, int len)
{
    for(int i = 0; i < len;)
    {
        
        str[i] = getchar();
        
        
        
        if(str[i] == '\n')
        {
            putchar('\n');
            str[i] = '\0';
            break;
        }
        else if(str[i] == '\b')
        {
            if(i > 0) 
            {
                putchar('\b');
                i--;
            }
        }
        else
        {
            putchar(str[i]);
            i++;
        }
        
    }
    str[len] = '\0';
}

void key_event()
{
    uint8_t scancode = inportb(PS2_DATA_PORT);

    // Ignore depresses for our simple buffer


    if(prev_count != 0)
    {
        // If we've only got one stored scancode
        if(prev_count == 1)
        {
            // If this is not part of a printscr keystroke (NOT WORKING)
            if(scancode != 0x2A && scancode != 0xB7 && scancode != 0x1D)
            {
                // Then this is the second part of a two byte scan code
                //dprintf("2 part keycode: %x\n", double_scan_to_key[scancode]);
                
                switch(scancode)
                {
                case 0x9D: // Right ctrl released
                    modifiers &= ~BIT(2);
                    break;
                case 0xB8: // Right alt released
                    modifiers &= ~BIT(1);
                    break;
                default:
                    if (!(scancode & 0x80))
                    {
                        key_buff_add(double_scan_to_key[scancode]);
                    }
                }
            }
            else
            {
                // Second byte of printscr
                prev_scan_buf[prev_count++] = scancode;
                return;
            }
        }
        else if (prev_count == 4)
        {
            // We're either done a printscr or in the middle of a pause
            if(prev_scan_buf[0] == 0xE0)
            {
                // We just finished recieving a printscr
                dprintf("PrintScr!\n");
                
            }
            else
            {
                prev_scan_buf[prev_count++] = scancode;
                return;
            }
        }
        else if (prev_count == 6)
        {
            // We must have recieved a pause keystroke
            dprintf("Pause key!\n");
            
            
            
        }
    }
    else 
    {
        // Beginning of a multi-byte scancode
        if(scancode == 0xE0 || scancode == 0xE1)
        {
            prev_scan_buf[prev_count++] = scancode;
            return;
        }
        else
        {
            // Single byte scancode
            switch(scancode)
            {
            case 0x9D: // Left ctrl released
                modifiers &= ~BIT(2);
                break;
            case 0xB8: // Left alt
                modifiers &= ~BIT(1);
                break;
            case 0xB6: // Right shift released
            case 0xAA: // Left
                modifiers &= ~BIT(0);
                break;
            default:
                if (!(scancode & 0x80))
                {
                    key_buff_add(single_scan_to_key[scancode]);
                }
            }
            //dprintf("1 part keycode: %x\n", single_scan_to_key[scancode]);
            
        }
        
    }
    // If we make it here we must have completed handling a keyevent
    
    prev_count = 0;
}

uint8_t send_keyboard_cmd(uint8_t command)
{
    int i;
    // We need to wait until the input buffer clear bit is set
    // Though also add a timeout so we don't wait forever
    for(i = 0; i < TIMEOUT_KEYBOARD_SEND; i++)
    {
        if(!(inportb(PS2_STATUS_PORT) & BIT(1)))
        {
            break;
        }
    }
    if(i >= TIMEOUT_KEYBOARD_SEND)
    {
        // Input buffer didn't clear in time
        dprintf("Timedout trying to send command to keyboard\n");
        return 0;
    }
    
    outportb(PS2_DATA_PORT, command);
    
    
    return 1;
}


uint8_t init_ps2_keyboard()
{
    // Probably should check we have a ps/2 controller first
    
    // Disable both ps/2 ports (assuming we have 2)
    outportb(PS2_CMD_PORT, PS2_CMD_DISABLE_FIRST);
    outportb(PS2_CMD_PORT, PS2_CMD_DISABLE_SECOND);
    
    // Flush output buffer
    inportb(PS2_DATA_PORT);
    
    outportb(PS2_CMD_PORT, PS2_CMD_READ_CFG);
    uint8_t conf_byte = inportb(PS2_DATA_PORT);
    
    // We want to disable the irq's and translation
    conf_byte &= ~(BIT(0) | BIT(1) | BIT(6));
    
    outportb(PS2_CMD_PORT, PS2_CMD_WRITE_CFG);
    
    // Test controller
    outportb(PS2_CMD_PORT, PS2_CMD_TEST_CTRLR);
    
    if(inportb(PS2_DATA_PORT) != 0x55)
    {
        dprintf("Error with keyboard controller \n");
        return 0;
    }
    
    // We'll skip testing for a second controller
    
    // Test first PS/2 port
    outportb(PS2_CMD_PORT, PS2_CMD_TEST_FIRST);
    if(inportb(PS2_DATA_PORT) != 0x00)
    {
        dprintf("Error on first PS/2 port!\n");
        return 0;
    }
    
    // We'll skip testing the second
    
    // Enable first PS/2 port
    outportb(PS2_CMD_PORT, PS2_CMD_ENABLE_FIRST);
    
    // Enable interrupts
    conf_byte = inportb(PS2_DATA_PORT);
    
    // We want to enable the first irq and translation
    conf_byte |= (BIT(0)| BIT(6));
    
    outportb(PS2_CMD_PORT, PS2_CMD_WRITE_CFG);
    
    // Finally send a reset command to the keyboard
    /*send_keyboard_cmd(KBD_CMD_RESET);
    
    if(inportb(PS2_DATA_PORT) != KBD_RESET_PASSED)
    {
        dprintf("Keyboard self-test failed!\n");
        return 0;
    }*/
    
    return 1;
}

