
#include "hw/interrupts.h"

#define APIC_REG_READ(index)        phymem_read32(0xFEE00000 + (0x10 * index))
#define APIC_REG_WRITE(index, val)  phymem_write32(0xFEE00000 + (0x10 * index), val)

#define APIC_ISR_BASE               0x10
#define APIC_EOI                    0x0B
#define APIC_ID                     0x02

#define KERNEL_SEGMENT              0x08

#define BIT(num)                    (1U << num)





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
    
    uint64_t *test_isr = malloc(256*8);
    
    
    // Quick test code, would prefer this work in the assembly
    test_isr[0] = (uint64_t)&int_handler_0;
    test_isr[1] = (uint64_t)&int_handler_1;
    test_isr[2] = (uint64_t)&int_handler_2;
    test_isr[3] = (uint64_t)&int_handler_3;
    test_isr[4] = (uint64_t)&int_handler_4;
    test_isr[5] = (uint64_t)&int_handler_5;
    test_isr[6] = (uint64_t)&int_handler_6;
    test_isr[7] = (uint64_t)&int_handler_7;
    test_isr[8] = (uint64_t)&int_handler_8;
    test_isr[9] = (uint64_t)&int_handler_9;
    test_isr[10] = (uint64_t)&int_handler_10;
    test_isr[11] = (uint64_t)&int_handler_11;
    test_isr[12] = (uint64_t)&int_handler_12;
    test_isr[13] = (uint64_t)&int_handler_13;
    test_isr[14] = (uint64_t)&int_handler_14;
    test_isr[15] = (uint64_t)&int_handler_15;
    test_isr[16] = (uint64_t)&int_handler_16;
    test_isr[17] = (uint64_t)&int_handler_17;
    test_isr[18] = (uint64_t)&int_handler_18;
    test_isr[19] = (uint64_t)&int_handler_19;
    test_isr[20] = (uint64_t)&int_handler_20;
    test_isr[21] = (uint64_t)&int_handler_21;
    test_isr[22] = (uint64_t)&int_handler_22;
    test_isr[23] = (uint64_t)&int_handler_23;
    test_isr[24] = (uint64_t)&int_handler_24;
    test_isr[25] = (uint64_t)&int_handler_25;
    test_isr[26] = (uint64_t)&int_handler_26;
    test_isr[27] = (uint64_t)&int_handler_27;
    test_isr[28] = (uint64_t)&int_handler_28;
    test_isr[29] = (uint64_t)&int_handler_29;
    test_isr[30] = (uint64_t)&int_handler_30;
    test_isr[31] = (uint64_t)&int_handler_31;
    test_isr[32] = (uint64_t)&int_handler_32;
    test_isr[33] = (uint64_t)&int_handler_33;
    test_isr[34] = (uint64_t)&int_handler_34;
    test_isr[35] = (uint64_t)&int_handler_35;
    test_isr[36] = (uint64_t)&int_handler_36;
    test_isr[37] = (uint64_t)&int_handler_37;
    test_isr[38] = (uint64_t)&int_handler_38;
    test_isr[39] = (uint64_t)&int_handler_39;
    test_isr[40] = (uint64_t)&int_handler_40;
    test_isr[41] = (uint64_t)&int_handler_41;
    test_isr[42] = (uint64_t)&int_handler_42;
    test_isr[43] = (uint64_t)&int_handler_43;
    test_isr[44] = (uint64_t)&int_handler_44;
    test_isr[45] = (uint64_t)&int_handler_45;
    test_isr[46] = (uint64_t)&int_handler_46;
    test_isr[47] = (uint64_t)&int_handler_47;
    test_isr[48] = (uint64_t)&int_handler_48;
    test_isr[49] = (uint64_t)&int_handler_49;
    test_isr[50] = (uint64_t)&int_handler_50;
    test_isr[51] = (uint64_t)&int_handler_51;
    test_isr[52] = (uint64_t)&int_handler_52;
    test_isr[53] = (uint64_t)&int_handler_53;
    test_isr[54] = (uint64_t)&int_handler_54;
    test_isr[55] = (uint64_t)&int_handler_55;
    test_isr[56] = (uint64_t)&int_handler_56;
    test_isr[57] = (uint64_t)&int_handler_57;
    test_isr[58] = (uint64_t)&int_handler_58;
    test_isr[59] = (uint64_t)&int_handler_59;
    test_isr[60] = (uint64_t)&int_handler_60;
    test_isr[61] = (uint64_t)&int_handler_61;
    test_isr[62] = (uint64_t)&int_handler_62;
    test_isr[63] = (uint64_t)&int_handler_63;
    test_isr[64] = (uint64_t)&int_handler_64;
    test_isr[65] = (uint64_t)&int_handler_65;
    test_isr[66] = (uint64_t)&int_handler_66;
    test_isr[67] = (uint64_t)&int_handler_67;
    test_isr[68] = (uint64_t)&int_handler_68;
    test_isr[69] = (uint64_t)&int_handler_69;
    test_isr[70] = (uint64_t)&int_handler_70;
    test_isr[71] = (uint64_t)&int_handler_71;
    test_isr[72] = (uint64_t)&int_handler_72;
    test_isr[73] = (uint64_t)&int_handler_73;
    test_isr[74] = (uint64_t)&int_handler_74;
    test_isr[75] = (uint64_t)&int_handler_75;
    test_isr[76] = (uint64_t)&int_handler_76;
    test_isr[77] = (uint64_t)&int_handler_77;
    test_isr[78] = (uint64_t)&int_handler_78;
    test_isr[79] = (uint64_t)&int_handler_79;
    test_isr[80] = (uint64_t)&int_handler_80;
    test_isr[81] = (uint64_t)&int_handler_81;
    test_isr[82] = (uint64_t)&int_handler_82;
    test_isr[83] = (uint64_t)&int_handler_83;
    test_isr[84] = (uint64_t)&int_handler_84;
    test_isr[85] = (uint64_t)&int_handler_85;
    test_isr[86] = (uint64_t)&int_handler_86;
    test_isr[87] = (uint64_t)&int_handler_87;
    test_isr[88] = (uint64_t)&int_handler_88;
    test_isr[89] = (uint64_t)&int_handler_89;
    test_isr[90] = (uint64_t)&int_handler_90;
    test_isr[91] = (uint64_t)&int_handler_91;
    test_isr[92] = (uint64_t)&int_handler_92;
    test_isr[93] = (uint64_t)&int_handler_93;
    test_isr[94] = (uint64_t)&int_handler_94;
    test_isr[95] = (uint64_t)&int_handler_95;
    test_isr[96] = (uint64_t)&int_handler_96;
    test_isr[97] = (uint64_t)&int_handler_97;
    test_isr[98] = (uint64_t)&int_handler_98;
    test_isr[99] = (uint64_t)&int_handler_99;
    test_isr[100] = (uint64_t)&int_handler_100;
    test_isr[101] = (uint64_t)&int_handler_101;
    test_isr[102] = (uint64_t)&int_handler_102;
    test_isr[103] = (uint64_t)&int_handler_103;
    test_isr[104] = (uint64_t)&int_handler_104;
    test_isr[105] = (uint64_t)&int_handler_105;
    test_isr[106] = (uint64_t)&int_handler_106;
    test_isr[107] = (uint64_t)&int_handler_107;
    test_isr[108] = (uint64_t)&int_handler_108;
    test_isr[109] = (uint64_t)&int_handler_109;
    test_isr[110] = (uint64_t)&int_handler_110;
    test_isr[111] = (uint64_t)&int_handler_111;
    test_isr[112] = (uint64_t)&int_handler_112;
    test_isr[113] = (uint64_t)&int_handler_113;
    test_isr[114] = (uint64_t)&int_handler_114;
    test_isr[115] = (uint64_t)&int_handler_115;
    test_isr[116] = (uint64_t)&int_handler_116;
    test_isr[117] = (uint64_t)&int_handler_117;
    test_isr[118] = (uint64_t)&int_handler_118;
    test_isr[119] = (uint64_t)&int_handler_119;
    test_isr[120] = (uint64_t)&int_handler_120;
    test_isr[121] = (uint64_t)&int_handler_121;
    test_isr[122] = (uint64_t)&int_handler_122;
    test_isr[123] = (uint64_t)&int_handler_123;
    test_isr[124] = (uint64_t)&int_handler_124;
    test_isr[125] = (uint64_t)&int_handler_125;
    test_isr[126] = (uint64_t)&int_handler_126;
    test_isr[127] = (uint64_t)&int_handler_127;
    test_isr[128] = (uint64_t)&int_handler_128;
    test_isr[129] = (uint64_t)&int_handler_129;
    test_isr[130] = (uint64_t)&int_handler_130;
    test_isr[131] = (uint64_t)&int_handler_131;
    test_isr[132] = (uint64_t)&int_handler_132;
    test_isr[133] = (uint64_t)&int_handler_133;
    test_isr[134] = (uint64_t)&int_handler_134;
    test_isr[135] = (uint64_t)&int_handler_135;
    test_isr[136] = (uint64_t)&int_handler_136;
    test_isr[137] = (uint64_t)&int_handler_137;
    test_isr[138] = (uint64_t)&int_handler_138;
    test_isr[139] = (uint64_t)&int_handler_139;
    test_isr[140] = (uint64_t)&int_handler_140;
    test_isr[141] = (uint64_t)&int_handler_141;
    test_isr[142] = (uint64_t)&int_handler_142;
    test_isr[143] = (uint64_t)&int_handler_143;
    test_isr[144] = (uint64_t)&int_handler_144;
    test_isr[145] = (uint64_t)&int_handler_145;
    test_isr[146] = (uint64_t)&int_handler_146;
    test_isr[147] = (uint64_t)&int_handler_147;
    test_isr[148] = (uint64_t)&int_handler_148;
    test_isr[149] = (uint64_t)&int_handler_149;
    test_isr[150] = (uint64_t)&int_handler_150;
    test_isr[151] = (uint64_t)&int_handler_151;
    test_isr[152] = (uint64_t)&int_handler_152;
    test_isr[153] = (uint64_t)&int_handler_153;
    test_isr[154] = (uint64_t)&int_handler_154;
    test_isr[155] = (uint64_t)&int_handler_155;
    test_isr[156] = (uint64_t)&int_handler_156;
    test_isr[157] = (uint64_t)&int_handler_157;
    test_isr[158] = (uint64_t)&int_handler_158;
    test_isr[159] = (uint64_t)&int_handler_159;
    test_isr[160] = (uint64_t)&int_handler_160;
    test_isr[161] = (uint64_t)&int_handler_161;
    test_isr[162] = (uint64_t)&int_handler_162;
    test_isr[163] = (uint64_t)&int_handler_163;
    test_isr[164] = (uint64_t)&int_handler_164;
    test_isr[165] = (uint64_t)&int_handler_165;
    test_isr[166] = (uint64_t)&int_handler_166;
    test_isr[167] = (uint64_t)&int_handler_167;
    test_isr[168] = (uint64_t)&int_handler_168;
    test_isr[169] = (uint64_t)&int_handler_169;
    test_isr[170] = (uint64_t)&int_handler_170;
    test_isr[171] = (uint64_t)&int_handler_171;
    test_isr[172] = (uint64_t)&int_handler_172;
    test_isr[173] = (uint64_t)&int_handler_173;
    test_isr[174] = (uint64_t)&int_handler_174;
    test_isr[175] = (uint64_t)&int_handler_175;
    test_isr[176] = (uint64_t)&int_handler_176;
    test_isr[177] = (uint64_t)&int_handler_177;
    test_isr[178] = (uint64_t)&int_handler_178;
    test_isr[179] = (uint64_t)&int_handler_179;
    test_isr[180] = (uint64_t)&int_handler_180;
    test_isr[181] = (uint64_t)&int_handler_181;
    test_isr[182] = (uint64_t)&int_handler_182;
    test_isr[183] = (uint64_t)&int_handler_183;
    test_isr[184] = (uint64_t)&int_handler_184;
    test_isr[185] = (uint64_t)&int_handler_185;
    test_isr[186] = (uint64_t)&int_handler_186;
    test_isr[187] = (uint64_t)&int_handler_187;
    test_isr[188] = (uint64_t)&int_handler_188;
    test_isr[189] = (uint64_t)&int_handler_189;
    test_isr[190] = (uint64_t)&int_handler_190;
    test_isr[191] = (uint64_t)&int_handler_191;
    test_isr[192] = (uint64_t)&int_handler_192;
    test_isr[193] = (uint64_t)&int_handler_193;
    test_isr[194] = (uint64_t)&int_handler_194;
    test_isr[195] = (uint64_t)&int_handler_195;
    test_isr[196] = (uint64_t)&int_handler_196;
    test_isr[197] = (uint64_t)&int_handler_197;
    test_isr[198] = (uint64_t)&int_handler_198;
    test_isr[199] = (uint64_t)&int_handler_199;
    test_isr[200] = (uint64_t)&int_handler_200;
    test_isr[201] = (uint64_t)&int_handler_201;
    test_isr[202] = (uint64_t)&int_handler_202;
    test_isr[203] = (uint64_t)&int_handler_203;
    test_isr[204] = (uint64_t)&int_handler_204;
    test_isr[205] = (uint64_t)&int_handler_205;
    test_isr[206] = (uint64_t)&int_handler_206;
    test_isr[207] = (uint64_t)&int_handler_207;
    test_isr[208] = (uint64_t)&int_handler_208;
    test_isr[209] = (uint64_t)&int_handler_209;
    test_isr[210] = (uint64_t)&int_handler_210;
    test_isr[211] = (uint64_t)&int_handler_211;
    test_isr[212] = (uint64_t)&int_handler_212;
    test_isr[213] = (uint64_t)&int_handler_213;
    test_isr[214] = (uint64_t)&int_handler_214;
    test_isr[215] = (uint64_t)&int_handler_215;
    test_isr[216] = (uint64_t)&int_handler_216;
    test_isr[217] = (uint64_t)&int_handler_217;
    test_isr[218] = (uint64_t)&int_handler_218;
    test_isr[219] = (uint64_t)&int_handler_219;
    test_isr[220] = (uint64_t)&int_handler_220;
    test_isr[221] = (uint64_t)&int_handler_221;
    test_isr[222] = (uint64_t)&int_handler_222;
    test_isr[223] = (uint64_t)&int_handler_223;
    test_isr[224] = (uint64_t)&int_handler_224;
    test_isr[225] = (uint64_t)&int_handler_225;
    test_isr[226] = (uint64_t)&int_handler_226;
    test_isr[227] = (uint64_t)&int_handler_227;
    test_isr[228] = (uint64_t)&int_handler_228;
    test_isr[229] = (uint64_t)&int_handler_229;
    test_isr[230] = (uint64_t)&int_handler_230;
    test_isr[231] = (uint64_t)&int_handler_231;
    test_isr[232] = (uint64_t)&int_handler_232;
    test_isr[233] = (uint64_t)&int_handler_233;
    test_isr[234] = (uint64_t)&int_handler_234;
    test_isr[235] = (uint64_t)&int_handler_235;
    test_isr[236] = (uint64_t)&int_handler_236;
    test_isr[237] = (uint64_t)&int_handler_237;
    test_isr[238] = (uint64_t)&int_handler_238;
    test_isr[239] = (uint64_t)&int_handler_239;
    test_isr[240] = (uint64_t)&int_handler_240;
    test_isr[241] = (uint64_t)&int_handler_241;
    test_isr[242] = (uint64_t)&int_handler_242;
    test_isr[243] = (uint64_t)&int_handler_243;
    test_isr[244] = (uint64_t)&int_handler_244;
    test_isr[245] = (uint64_t)&int_handler_245;
    test_isr[246] = (uint64_t)&int_handler_246;
    test_isr[247] = (uint64_t)&int_handler_247;
    test_isr[248] = (uint64_t)&int_handler_248;
    test_isr[249] = (uint64_t)&int_handler_249;
    test_isr[250] = (uint64_t)&int_handler_250;
    test_isr[251] = (uint64_t)&int_handler_251;
    test_isr[252] = (uint64_t)&int_handler_252;
    test_isr[253] = (uint64_t)&int_handler_253;
    test_isr[254] = (uint64_t)&int_handler_254;
    test_isr[255] = (uint64_t)&int_handler_255;
    // End test code
    
    
    
    uint32_t *idt = (uint32_t *)malloc(256 * 32 );    
    
    int i;
    for(i = 0; i < 256*4; i += 4)
    {
        // First 32 bits 0-15 offset 0-15, 16-31 segment selector
        idt[i]   = (KERNEL_SEGMENT << 16)      | (test_isr[i/4] & 0xFFFF);
        
        // Next 32, 0-15 various flags, 16-31 offset 16-31
        //!! Current hard coded flags are 8 - present
        //!!                              E - Interrupt Gate
        //!!                              0
        //!!                              1 - IST 1
        idt[i+1] = (test_isr[i/4] & 0xFFFF0000) | ( 0x8E01 );
        
        // Next 32, 0-31 offset 32-63
        idt[i+2] = (test_isr[i/4] & 0xFFFFFFFF00000000ULL) >> 32;
        
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
    
    ioapic_set_irq(0, APIC_REG_READ(APIC_ID), 0x50);
    
    dprintf("%x\n", APIC_REG_READ(0x2));
    
    enable_interrupts();
    
    
    
    
    
    
}


void generic_interrupt_exception(uint64_t intnum, uint64_t err_code)
{
    dprintf("Exception 0x%x, code: 0x%x\n", intnum, err_code);
    conditional_acknowledge_interrupt(intnum);
    
}


void generic_interrupt(uint64_t intnum)
{
    uint8_t scancode;
    // Called on every interrupt that is not listed as an exception
    
    

    dprintf("interrupt 0x%x\n", intnum);
    
    switch(intnum)
    {
    case 0x40:          // Keyboard
    
        dprintf("keyboard!");
        scancode = inportb(0x60);
        
        if(scancode & 0x80)
        {
            dprintf("\nKey %x pressed\n", scancode & ~0x80);
        }
        else
        {
            dprintf("\nKey %x depressed\n", scancode);
        }
    
        // Reset the keyboard controller
        uint8_t a = inportb(0x61);
        a |= 0x82;
        outportb(0x61, a);
        a &= 0x7f;
        outportb(0x61, a);
        break;
    default:
        dprintf("int %x\n", intnum);
        break;
    }
    
    conditional_acknowledge_interrupt(intnum);
}
