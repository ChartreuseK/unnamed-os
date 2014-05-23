
#include "hw/interrupts.h"

#define APIC_REG_READ(index)        phymem_read32(0xFEE00000ULL + (0x10 * (index)))
#define APIC_REG_WRITE(index, val)  phymem_write32(0xFEE00000ULL + (0x10 * (index)), val)

#define APIC_ISR_BASE               0x10
#define APIC_EOI                    0x0B
#define APIC_ID                     0x02

#define KERNEL_SEGMENT              0x08

#define BIT(num)                    (1U << (num))



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
    
    // Set bit 8 to enable the local APIC
    sivr |= BIT(8);
    
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

    
    
    uint32_t *idt = (uint32_t *)malloc(256 * 32 );    
    
    int i;
    for(i = 0; i < 256*4; i += 4)
    {
        // First 32 bits 0-15 offset 0-15, 16-31 segment selector
        idt[i]   = (KERNEL_SEGMENT << 16)      | (isr_table[i/4] & 0xFFFF);
        
        // Next 32, 0-15 various flags, 16-31 offset 16-31
        //!! Current hard coded flags are 8 - present
        //!!                              E - Interrupt Gate
        //!!                              0
        //!!                              1 - IST 1
        idt[i+1] = (isr_table[i/4] & 0xFFFF0000) | ( 0x8E01 );
        
        // Next 32, 0-31 offset 32-63
        idt[i+2] = (isr_table[i/4] & 0xFFFFFFFF00000000ULL) >> 32;
        
        // Final 32, 0-31 blank
        idt[i+3] = 0;
        
    }

    
    uint8_t idt_pointer[10];
    
    *((uint16_t *)idt_pointer)      = (uint16_t)4095; // Or is it 4096
    *((uint64_t *)(idt_pointer+2))  = (uint64_t)idt;
    
    
    
    __asm__ volatile("lidt [%0]"::"r"(idt_pointer)); 
    
    
    
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
    uint16_t tss_limit = 28*4;
    int gdtoffset = append_gdt( (((uint64_t)tss & 0xFFFF) << 16) | (tss_limit), ((uint64_t)tss & 0xFF0000) | BIT(15) | (0x9 << 8) | ((uint64_t)tss & 0xFF000000) << 24);
    append_gdt( ((uint64_t)tss & 0xFFFFFFFF00000000ULL)>>32, 0);
    
    
    
    __asm__ volatile("ltr %%ax"::"a"(gdtoffset * 8));
 
    
    
    
    
    setioapicbase();
    
    
    
    // Setup the keyboard IRQ1 to be handled by interrupt 0x40
    ioapic_set_irq(1, APIC_REG_READ(APIC_ID), 0x40);
    
    // Set the PIC in mode 2 to a 100hz timer
    outportb(0x43, 0x68);
    outportb(0x40, 0x9b);
    outportb(0x40, 0x2e);
    
    
    // Install the PIT system timer (IRQ 2 with APIC!!!) to interrupt 0x50
    ioapic_set_irq(2, APIC_REG_READ(APIC_ID), 0x50);
    
    
    enable_interrupts();
    
    
    
    
    
    
}

void register_trace(uint64_t savedregs)
{
    dprintf("Register Trace\n");
    dprintf("ss:     0x%p    rsp:   0x%p\nrflags: 0x%p    cs:    0x%p\n", *(uint64_t*)(savedregs),*(uint64_t*)(savedregs-1*8),*(uint64_t*)((savedregs-2*8)),*(uint64_t*)((savedregs-3*8)));
    dprintf("rip:    0x%p    ecode: 0x%p\nret:    0x%p    \n", *(uint64_t*)((savedregs-4*8)), *(uint64_t*)((savedregs-6*8)),*(uint64_t*)((savedregs-5*8))); 
    dprintf("rax:    0x%p    rbx:   0x%p\nrcx:    0x%p    rdx:   0x%p\n",*(uint64_t*)((savedregs-7*8)), *(uint64_t*)((savedregs-8*8)), *(uint64_t*)((savedregs-9*8)),*(uint64_t*)((savedregs-10*8)));
    dprintf("rsi:    0x%p    rdi:   0x%p\nrsp:    0x%p    rbp:   0x%p\n",*(uint64_t*)((savedregs-11*8)), *(uint64_t*)((savedregs-12*8)), *(uint64_t*)((savedregs-13*8)),*(uint64_t*)((savedregs-14*8)));
    dprintf("r8:     0x%p    r9:    0x%p\nr10:    0x%p    r11:   0x%p\n",*(uint64_t*)((savedregs-15*8)), *(uint64_t*)((savedregs-16*8)), *(uint64_t*)((savedregs-17*8)),*(uint64_t*)((savedregs-18*8)));
    dprintf("r12:    0x%p    r13:   0x%p\nr14:    0x%p    r15:   0x%p\n",*(uint64_t*)((savedregs-19)), *(uint64_t*)((savedregs-20)), *(uint64_t*)((savedregs-21)),*(uint64_t*)((savedregs-22)));
    dprintf("ds:     0x%p    es:    0x%p\n", *(uint16_t*)((savedregs - (23 * 8))), *(uint16_t*)((savedregs - (23 * 8) - 2)));
    dprintf("fs:     0x%p    gs:    0x%p\n", *(uint64_t*)((savedregs - (23 * 8) - 4)), *(uint64_t*)(savedregs - (23 * 8) - 12));
    
    printf("Register Trace\n");
    printf("ss:     0x%p    rsp:   0x%p\nrflags: 0x%p    cs:    0x%p\n", *(uint64_t*)(savedregs),*(uint64_t*)(savedregs-1*8),*(uint64_t*)((savedregs-2*8)),*(uint64_t*)((savedregs-3*8)));
    printf("rip:    0x%p    ecode: 0x%p\nret:    0x%p    \n", *(uint64_t*)((savedregs-4*8)), *(uint64_t*)((savedregs-6*8)),*(uint64_t*)((savedregs-5*8))); 
    printf("rax:    0x%p    rbx:   0x%p\nrcx:    0x%p    rdx:   0x%p\n",*(uint64_t*)((savedregs-7*8)), *(uint64_t*)((savedregs-8*8)), *(uint64_t*)((savedregs-9*8)),*(uint64_t*)((savedregs-10*8)));
    printf("rsi:    0x%p    rdi:   0x%p\nrsp:    0x%p    rbp:   0x%p\n",*(uint64_t*)((savedregs-11*8)), *(uint64_t*)((savedregs-12*8)), *(uint64_t*)((savedregs-13*8)),*(uint64_t*)((savedregs-14*8)));
    printf("r8:     0x%p    r9:    0x%p\nr10:    0x%p    r11:   0x%p\n",*(uint64_t*)((savedregs-15*8)), *(uint64_t*)((savedregs-16*8)), *(uint64_t*)((savedregs-17*8)),*(uint64_t*)((savedregs-18*8)));
    printf("r12:    0x%p    r13:   0x%p\nr14:    0x%p    r15:   0x%p\n",*(uint64_t*)((savedregs-19)), *(uint64_t*)((savedregs-20)), *(uint64_t*)((savedregs-21)),*(uint64_t*)((savedregs-22)));
    printf("ds:     0x%p    es:    0x%p\n", *(uint16_t*)((savedregs - (23 * 8))), *(uint16_t*)((savedregs - (23 * 8) - 2)));
    printf("fs:     0x%p    gs:    0x%p\n", *(uint64_t*)((savedregs - (23 * 8) - 4)), *(uint64_t*)(savedregs - (23 * 8) - 12));
    
    
}

void register_trace_noerror(uint64_t savedregs)
{
    dprintf("Register Trace\n");
    dprintf("ss:     0x%p    rsp:   0x%p\nrflags: 0x%p    cs:    0x%p\n", *(uint64_t*)(savedregs),*(uint64_t*)(savedregs-1*8),*(uint64_t*)((savedregs-2*8)),*(uint64_t*)((savedregs-3*8)));
    dprintf("rip:    0x%p               \nret:    0x%p    \n", *(uint64_t*)((savedregs-4*8)),*(uint64_t*)((savedregs-5*8))); 
    dprintf("rax:    0x%p    rbx:   0x%p\nrcx:    0x%p    rdx:   0x%p\n",*(uint64_t*)((savedregs-6*8)), *(uint64_t*)((savedregs-7*8)), *(uint64_t*)((savedregs-8*8)),*(uint64_t*)((savedregs-9*8)));
    dprintf("rsi:    0x%p    rdi:   0x%p\nrsp:    0x%p    rbp:   0x%p\n",*(uint64_t*)((savedregs-10*8)), *(uint64_t*)((savedregs-11*8)), *(uint64_t*)((savedregs-12*8)),*(uint64_t*)((savedregs-13*8)));
    dprintf("r8:     0x%p    r9:    0x%p\nr10:    0x%p    r11:   0x%p\n",*(uint64_t*)((savedregs-14*8)), *(uint64_t*)((savedregs-15*8)), *(uint64_t*)((savedregs-16*8)),*(uint64_t*)((savedregs-17*8)));
    dprintf("r12:    0x%p    r13:   0x%p\nr14:    0x%p    r15:   0x%p\n",*(uint64_t*)((savedregs-18*8)), *(uint64_t*)((savedregs-19*8)), *(uint64_t*)((savedregs-20*8)),*(uint64_t*)((savedregs-21*8)));
    dprintf("ds:     0x%p    es:    0x%p\n", *(uint16_t*)((savedregs - (22 * 8))), *(uint16_t*)((savedregs - (22 * 8) - 2)));
    dprintf("fs:     0x%p    gs:    0x%p\n", *(uint64_t*)((savedregs - (22 * 8) - 4)), *(uint64_t*)(savedregs - (22 * 8) - 12));
    
    printf("Register Trace\n");
    printf("ss:     0x%p    rsp:   0x%p\nrflags: 0x%p    cs:    0x%p\n", *(uint64_t*)(savedregs),*(uint64_t*)(savedregs-1*8),*(uint64_t*)((savedregs-2*8)),*(uint64_t*)((savedregs-3*8)));
    printf("rip:    0x%p               \nret:    0x%p    \n", *(uint64_t*)((savedregs-4*8)),*(uint64_t*)((savedregs-5*8))); 
    printf("rax:    0x%p    rbx:   0x%p\nrcx:    0x%p    rdx:   0x%p\n",*(uint64_t*)((savedregs-6*8)), *(uint64_t*)((savedregs-7*8)), *(uint64_t*)((savedregs-8*8)),*(uint64_t*)((savedregs-9*8)));
    printf("rsi:    0x%p    rdi:   0x%p\nrsp:    0x%p    rbp:   0x%p\n",*(uint64_t*)((savedregs-10*8)), *(uint64_t*)((savedregs-11*8)), *(uint64_t*)((savedregs-12*8)),*(uint64_t*)((savedregs-13*8)));
    printf("r8:     0x%p    r9:    0x%p\nr10:    0x%p    r11:   0x%p\n",*(uint64_t*)((savedregs-14*8)), *(uint64_t*)((savedregs-15*8)), *(uint64_t*)((savedregs-16*8)),*(uint64_t*)((savedregs-17*8)));
    printf("r12:    0x%p    r13:   0x%p\nr14:    0x%p    r15:   0x%p\n",*(uint64_t*)((savedregs-18*8)), *(uint64_t*)((savedregs-19*8)), *(uint64_t*)((savedregs-20*8)),*(uint64_t*)((savedregs-21*8)));
    printf("ds:     0x%p    es:    0x%p\n", *(uint16_t*)((savedregs - (22 * 8))), *(uint16_t*)((savedregs - (22 * 8) - 2)));
    printf("fs:     0x%p    gs:    0x%p\n", *(uint64_t*)((savedregs - (22 * 8) - 4)), *(uint64_t*)(savedregs - (22 * 8) - 12));
    
}









void generic_interrupt_exception(uint64_t intnum, uint64_t err_code, uint64_t savedregs)
{
    conditional_acknowledge_interrupt(intnum);
 
    
 
 
    printf("\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!!!!!! Exception encountered. Halting !!!!!!!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("Exception 0x%x, code: 0x%x\n", intnum, err_code);
    
    dprintf("\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    dprintf("!!!!!!! Exception encountered. Halting !!!!!!!\n");
    dprintf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    dprintf("Exception 0x%x, code: 0x%x\n", intnum, err_code);
    
    
    register_trace(savedregs);
    
    __asm__("hlt");
    while(1){}
}


void generic_interrupt(uint64_t intnum, uint64_t savedregs)
{
    uint8_t scancode;
    static uint32_t counter = 1;
    // Called on every interrupt that is not listed as having an error code
    
    switch(intnum)
    {
    case 0x0:       // Division by 0 exception - no error code
        dprintf("Divide by zero, halting!\n");
        register_trace_noerror(savedregs);
        __asm__("hlt");
        while(1){}
        break;
        
        
    case 0x40:          // Keyboard
        key_event();
        break;
        dprintf("keyboard!");
        scancode = inportb(0x60);
        
        if(!(scancode & 0x80))
        {
            dprintf("\nKey %x pressed\n", scancode & ~0x80);
        }
        else
        {
            dprintf("\nKey %x depressed\n", scancode);
        }
    /*
        // Reset the keyboard controller
        uint8_t a = inportb(0x64);
        a |= 0x82;
        outportb(0x64, a);
        a &= 0x7f;
        outportb(0x64m, a);*/
        break;
    case 0x50:          // Timer
        //dprintf("%x\r", counter++);
        //scheduler_event(savedregs);
        break;
    
    default:
        dprintf("unhandled int %x\n", intnum);
        register_trace(savedregs);
        break;
    }
    
    conditional_acknowledge_interrupt(intnum);
}
