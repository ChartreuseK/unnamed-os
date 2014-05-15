
#include "hw/interrupts.h"

#define APIC_REG_READ(index)        phymem_read32(0xFEE00000 + (0x10 * index))
#define APIC_REG_WRITE(index, val)  phymem_write32(0xFEE00000 + (0x10 * index), val)

#define APIC_ISR_BASE               0x10
#define APIC_EOI                    0x0B
#define APIC_ID                     0x02

#define KERNEL_SEGMENT              0x08

#define BIT(num)                    (1U << num)



extern uint64_t *isr_table;

/* disable_pic()
 *  Disables the legacy Master-Slave 8259 PIC
 *  
 *****/
void disable_pic() 
{
    /* Set ICW1 */
    outportb(0x20, 0x11);
    outportb(0xa0, 0x11);

    /* Set ICW2 (IRQ base offsets) */
    outportb(0x21, 0xe0);
    outportb(0xa1, 0xe8);

    /* Set ICW3 */
    outportb(0x21, 4);
    outportb(0xa1, 2);

    /* Set ICW4 */
    outportb(0x21, 1);
    outportb(0xa1, 1);

    /* Set OCW1 (interrupt masks) */
    outportb(0x21, 0xff);
    outportb(0xa1, 0xff);
}




void enable_local_apic()
{
    uint32_t sivr;
    
    // Read the value of the Spurious Interrupt Vector register
    sivr = APIC_REG_READ(15);
    
    // Clear bit 8 to enable the local APIC
    sivr &= ~BIT(8);
    
    // Write back to the register
    APIC_REG_WRITE(15, sivr);
    
    
}


void enable_interrupts()
{
    __asm__ volatile("sti");
}
void disable_interrupts()
{
    __asm__ volatile("cli");
}

void conditional_acknowledge_interrupt(uint8_t interrupt)
{
    // If the ISR is set for this interrupt we have to clear 
    if( APIC_REG_READ(APIC_ISR_BASE + (interrupt / 32)) & BIT(interrupt % 32) )
    {
        APIC_REG_WRITE(APIC_EOI, 1);
        
    }
    
}



void setup_interrupts()
{
    disable_pic();
    enable_local_apic();
    
    
    uint32_t *idt = (uint32_t *)malloc(256 * 32);    
    for(int i = 0; i < 256*4; i += 4)
    {
        // First 32 bits 0-15 offset 0-15, 16-31 segment selector
        idt[i]   = (KERNEL_SEGMENT << 16)      | (isr_table[i] & 0xFFFF);
        // Next 32, 0-15 various flags, 16-31 offset 16-31
        //!! Current hard coded flags are 8 - present
        //!!                              E - Interrupt Gate
        //!!                              0
        //!!                              1 - IST 1
        idt[i+1] = (isr_table[i] & 0xFFFF0000) | ( 0x8E01 );
        // Next 32, 0-31 offset 32-63
        idt[i+2] = (isr_table[i] & 0xFFFFFFFF00000000ULL);
        // Final 32, 0-31 blank
        idt[i+3] = 0;
    }
    
    uint8_t idt_pointer[10];
    
    *((uint16_t *)idt_pointer)      = (uint16_t)4095; // Or is it 4096
    *((uint64_t *)(idt_pointer+2))  = (uint64_t)&idt;
    
    
    __asm__ volatile("lidt [%0]"::"r"(idt)); 
    
    
    
    // Setup TSS here
    uint8_t *tss = malloc(28 * 4); 
    for(int i = 0; i < 28 * 4; i++) tss[i] = 0x00; // Clear out the tss
    
    for(int i = 0; i < 7; i++)
    {
        // Allocate 1024 bytes for each IST, storing the pointer to the top
        // of the memory allocated
        *(uint64_t*)(tss + 36 + (8 * i)) = (uint64_t)((uint8_t*)malloc(1024) + 1024);
    }
    
    
    // From notes, no clue what these do
    tss[102] = 0x68;
    
    tss[104] = 0xff;
    tss[105] = 0xff;
    
    // Add the TSS to a double gdt segment
    int gdtoffset = append_gdt( ((uint64_t)tss & 0xFFFF) << 16, ((uint64_t)tss & 0xFF0000) | (0x9 << 8) | ((uint64_t)tss & 0xFF000000) << 24);
    append_gdt( ((uint64_t)tss & 0xFFFFFFFF00000000ULL), 0);
    
    __asm__ volatile("ltr %%ax"::"a"(gdtoffset));
    
    enable_interrupts();
    
    setioapicbase();
    
    // Setup the keyboard IRQ1 to be handled by interrupt 0x40
    ioapic_set_irq(1, APIC_REG_READ(APIC_ID), 0x40);
    
    
    
    
    
    
    
    
    
}





void generic_interrupt(uint64_t intnum)
{
    dprintf("interrupt!\n");
    uint8_t scancode;
    // Called on every interrupt that isn't explicitly defined
    switch(intnum)
    {
    case 0x40:          // Keyboard
    
        scancode = inportb(0x60);
        
        if(scancode & 0x80)
        {
            dprintf("Key %x pressed\n", scancode & ~0x80);
        }
        else
        {
            dprintf("Key %x depressed\n", scancode);
        }
    
        // Reset the keyboard controller
        uint8_t a = inportb(0x61);
        a |= 0x82;
        outportb(0x61, a);
        a &= 0x7f;
        outportb(0x61, a);
        break;
    default:
        break;
    }
    
    conditional_acknowledge_interrupt(intnum);
}
