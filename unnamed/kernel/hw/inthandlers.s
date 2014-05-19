; inthandlers.s
;
; Interrupt handlers for our operating system
;
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern generic_interrupt_exception
extern generic_interrupt
global isr_table
global generic_int_handler

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; For interrupts and exceptions which do not push error codes
;;;;;;;;;
generic_int_handler:
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    rsp
    push    rbp
    
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
    
    mov     ax, ds
    push    ax
    
    mov     ax, es
    push    ax
    
    push    fs
    push    gs
    
    ;; We pushed 18 64bit registers onto our stack and 2 16 bit
    ;; So now our interrupt number is behind all those
    mov     rdi, [rsp + (18 * 8) + (2 * 2)]       ; Should load the interrupt number
    
    mov     rsi, rsp
    add     rsi, (18 * 8) + (2 * 2) + (6 * 8)
    
    call    generic_interrupt

    

    ;; Do things here

    pop     gs
    pop     fs
    
    pop     ax
    mov     es, ax
    
    pop     ax
    mov     ds, ax

    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8

    pop     rbp
    pop     rsp
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax

    add     rsp, 8              ; Pop off the interrupt number we pushed

    iretq                       ; Return from the interrupt
    
    
    

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; For when an exception passes along an error message
;;;
generic_int_handler_exception:
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    rsp
    push    rbp
    
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
    
    mov     ax, ds
    push    ax
    
    mov     ax, es
    push    ax
    
    push    fs
    push    gs
    
    ;; We pushed 18 64bit registers onto our stack and 2 16bit
    ;; So now our interrupt number is behind all those

    ;; rsp + 172   -> rip?          
    ;; rsp + 164   -> error code?  <-- return pointer appears to be here
    ;; rsp + 156   -> return pointer? <-- error code appears to be here
    ;; rsp + 148   -> interrupt number
    ;; rsp + 140   -> rax
    ;; rsp + 132   -> rbx
    ;; rsp + 124   -> rcx
    ;; rsp + 116   -> rdx
    ;; rsp + 108   -> rsi
    ;; rsp + 100   -> rdi
    ;; rsp + 92    -> rsp
    ;; rsp + 84    -> rbp
    ;; rsp + 76    -> r8
    ;; rsp + 68    -> r9
    ;; rsp + 60    -> r10
    ;; rsp + 52    -> r11
    ;; rsp + 44    -> r12
    ;; rsp + 36    -> r13
    ;; rsp + 28    -> r14
    ;; rsp + 20    -> r15
    ;; rsp + 18    -> ds
    ;; rsp + 16    -> es
    ;; rsp + 8     -> fs
    ;; rsp         -> gs

    
    mov     rdi, [rsp + (18 * 8) + (2*2)]       ; Should load the interrupt number
    
    mov     rsi, [rsp + (19 * 8) + (2*2)]       ; Grab the error code
                       
    mov     rdx, rsp
    add     rdx, (18 * 8) + (2 * 2) + (7 * 8)
    
    call    generic_interrupt_exception

    

    ;; Do things here

    pop     gs
    pop     fs
    
    pop     ax
    mov     es, ax
    
    pop     ax
    mov     ds, ax

    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8

    pop     rbp
    pop     rsp
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rbx
    
    
    
    
    pop     rax

    add     rsp, (2 * 8)        ; Pop off the interrupt number we pushed
                                ; And the error code
    


    iretq                       ; Return from the interrupt


    
    
    
    
    

int_handler_0:
    push QWORD 0
    jmp generic_int_handler    ; Div by 0
int_handler_1:
    push QWORD 1
    jmp generic_int_handler    ; 
int_handler_2:
    push QWORD 2
    jmp generic_int_handler
int_handler_3:
    push QWORD 3
    jmp generic_int_handler
int_handler_4:
    push QWORD 4
    jmp generic_int_handler
int_handler_5:
    push QWORD 5
    jmp generic_int_handler
int_handler_6:
    push QWORD 6
    jmp generic_int_handler
int_handler_7:
    push QWORD 7
    jmp generic_int_handler
int_handler_8:
    push QWORD 8
    jmp generic_int_handler_exception       ; Double fault
int_handler_9:
    push QWORD 9
    jmp generic_int_handler
int_handler_10:
    push QWORD 10
    jmp generic_int_handler_exception       ; Invalid TSS
int_handler_11:
    push QWORD 11
    jmp generic_int_handler_exception       ; Segment not present
int_handler_12:
    push QWORD 12
    jmp generic_int_handler_exception       ; Stack fault
int_handler_13:
    push QWORD 13
    jmp generic_int_handler_exception       ; GPF
int_handler_14:
    push QWORD 14
    jmp generic_int_handler_exception       ; Page fault
int_handler_15:
    push QWORD 15
    jmp generic_int_handler
int_handler_16:
    push QWORD 16
    jmp generic_int_handler
int_handler_17:
    push QWORD 17
    jmp generic_int_handler_exception       ; Alignment check
int_handler_18:
    push QWORD 18
    jmp generic_int_handler
int_handler_19:
    push QWORD 19
    jmp generic_int_handler
int_handler_20:
    push QWORD 20
    jmp generic_int_handler
int_handler_21:
    push QWORD 21
    jmp generic_int_handler
int_handler_22:
    push QWORD 22
    jmp generic_int_handler
int_handler_23:
    push QWORD 23
    jmp generic_int_handler
int_handler_24:
    push QWORD 24
    jmp generic_int_handler
int_handler_25:
    push QWORD 25
    jmp generic_int_handler
int_handler_26:
    push QWORD 26
    jmp generic_int_handler
int_handler_27:
    push QWORD 27
    jmp generic_int_handler
int_handler_28:
    push QWORD 28
    jmp generic_int_handler
int_handler_29:
    push QWORD 29
    jmp generic_int_handler
int_handler_30:
    push QWORD 30
    jmp generic_int_handler
int_handler_31:
    push QWORD 31
    jmp generic_int_handler
int_handler_32:
    push QWORD 32
    jmp generic_int_handler
int_handler_33:
    push QWORD 33
    jmp generic_int_handler
int_handler_34:
    push QWORD 34
    jmp generic_int_handler
int_handler_35:
    push QWORD 35
    jmp generic_int_handler
int_handler_36:
    push QWORD 36
    jmp generic_int_handler
int_handler_37:
    push QWORD 37
    jmp generic_int_handler
int_handler_38:
    push QWORD 38
    jmp generic_int_handler
int_handler_39:
    push QWORD 39
    jmp generic_int_handler
int_handler_40:
    push QWORD 40
    jmp generic_int_handler
int_handler_41:
    push QWORD 41
    jmp generic_int_handler
int_handler_42:
    push QWORD 42
    jmp generic_int_handler
int_handler_43:
    push QWORD 43
    jmp generic_int_handler
int_handler_44:
    push QWORD 44
    jmp generic_int_handler
int_handler_45:
    push QWORD 45
    jmp generic_int_handler
int_handler_46:
    push QWORD 46
    jmp generic_int_handler
int_handler_47:
    push QWORD 47
    jmp generic_int_handler
int_handler_48:
    push QWORD 48
    jmp generic_int_handler
int_handler_49:
    push QWORD 49
    jmp generic_int_handler
int_handler_50:
    push QWORD 50
    jmp generic_int_handler
int_handler_51:
    push QWORD 51
    jmp generic_int_handler
int_handler_52:
    push QWORD 52
    jmp generic_int_handler
int_handler_53:
    push QWORD 53
    jmp generic_int_handler
int_handler_54:
    push QWORD 54
    jmp generic_int_handler
int_handler_55:
    push QWORD 55
    jmp generic_int_handler
int_handler_56:
    push QWORD 56
    jmp generic_int_handler
int_handler_57:
    push QWORD 57
    jmp generic_int_handler
int_handler_58:
    push QWORD 58
    jmp generic_int_handler
int_handler_59:
    push QWORD 59
    jmp generic_int_handler
int_handler_60:
    push QWORD 60
    jmp generic_int_handler
int_handler_61:
    push QWORD 61
    jmp generic_int_handler
int_handler_62:
    push QWORD 62
    jmp generic_int_handler
int_handler_63:
    push QWORD 63
    jmp generic_int_handler
int_handler_64:
    push QWORD 64
    jmp generic_int_handler
int_handler_65:
    push QWORD 65
    jmp generic_int_handler
int_handler_66:
    push QWORD 66
    jmp generic_int_handler
int_handler_67:
    push QWORD 67
    jmp generic_int_handler
int_handler_68:
    push QWORD 68
    jmp generic_int_handler
int_handler_69:
    push QWORD 69
    jmp generic_int_handler
int_handler_70:
    push QWORD 70
    jmp generic_int_handler
int_handler_71:
    push QWORD 71
    jmp generic_int_handler
int_handler_72:
    push QWORD 72
    jmp generic_int_handler
int_handler_73:
    push QWORD 73
    jmp generic_int_handler
int_handler_74:
    push QWORD 74
    jmp generic_int_handler
int_handler_75:
    push QWORD 75
    jmp generic_int_handler
int_handler_76:
    push QWORD 76
    jmp generic_int_handler
int_handler_77:
    push QWORD 77
    jmp generic_int_handler
int_handler_78:
    push QWORD 78
    jmp generic_int_handler
int_handler_79:
    push QWORD 79
    jmp generic_int_handler
int_handler_80:
    push QWORD 80
    jmp generic_int_handler
int_handler_81:
    push QWORD 81
    jmp generic_int_handler
int_handler_82:
    push QWORD 82
    jmp generic_int_handler
int_handler_83:
    push QWORD 83
    jmp generic_int_handler
int_handler_84:
    push QWORD 84
    jmp generic_int_handler
int_handler_85:
    push QWORD 85
    jmp generic_int_handler
int_handler_86:
    push QWORD 86
    jmp generic_int_handler
int_handler_87:
    push QWORD 87
    jmp generic_int_handler
int_handler_88:
    push QWORD 88
    jmp generic_int_handler
int_handler_89:
    push QWORD 89
    jmp generic_int_handler
int_handler_90:
    push QWORD 90
    jmp generic_int_handler
int_handler_91:
    push QWORD 91
    jmp generic_int_handler
int_handler_92:
    push QWORD 92
    jmp generic_int_handler
int_handler_93:
    push QWORD 93
    jmp generic_int_handler
int_handler_94:
    push QWORD 94
    jmp generic_int_handler
int_handler_95:
    push QWORD 95
    jmp generic_int_handler
int_handler_96:
    push QWORD 96
    jmp generic_int_handler
int_handler_97:
    push QWORD 97
    jmp generic_int_handler
int_handler_98:
    push QWORD 98
    jmp generic_int_handler
int_handler_99:
    push QWORD 99
    jmp generic_int_handler
int_handler_100:
    push QWORD 100
    jmp generic_int_handler
int_handler_101:
    push QWORD 101
    jmp generic_int_handler
int_handler_102:
    push QWORD 102
    jmp generic_int_handler
int_handler_103:
    push QWORD 103
    jmp generic_int_handler
int_handler_104:
    push QWORD 104
    jmp generic_int_handler
int_handler_105:
    push QWORD 105
    jmp generic_int_handler
int_handler_106:
    push QWORD 106
    jmp generic_int_handler
int_handler_107:
    push QWORD 107
    jmp generic_int_handler
int_handler_108:
    push QWORD 108
    jmp generic_int_handler
int_handler_109:
    push QWORD 109
    jmp generic_int_handler
int_handler_110:
    push QWORD 110
    jmp generic_int_handler
int_handler_111:
    push QWORD 111
    jmp generic_int_handler
int_handler_112:
    push QWORD 112
    jmp generic_int_handler
int_handler_113:
    push QWORD 113
    jmp generic_int_handler
int_handler_114:
    push QWORD 114
    jmp generic_int_handler
int_handler_115:
    push QWORD 115
    jmp generic_int_handler
int_handler_116:
    push QWORD 116
    jmp generic_int_handler
int_handler_117:
    push QWORD 117
    jmp generic_int_handler
int_handler_118:
    push QWORD 118
    jmp generic_int_handler
int_handler_119:
    push QWORD 119
    jmp generic_int_handler
int_handler_120:
    push QWORD 120
    jmp generic_int_handler
int_handler_121:
    push QWORD 121
    jmp generic_int_handler
int_handler_122:
    push QWORD 122
    jmp generic_int_handler
int_handler_123:
    push QWORD 123
    jmp generic_int_handler
int_handler_124:
    push QWORD 124
    jmp generic_int_handler
int_handler_125:
    push QWORD 125
    jmp generic_int_handler
int_handler_126:
    push QWORD 126
    jmp generic_int_handler
int_handler_127:
    push QWORD 127
    jmp generic_int_handler
int_handler_128:
    push QWORD 128
    jmp generic_int_handler
int_handler_129:
    push QWORD 129
    jmp generic_int_handler
int_handler_130:
    push QWORD 130
    jmp generic_int_handler
int_handler_131:
    push QWORD 131
    jmp generic_int_handler
int_handler_132:
    push QWORD 132
    jmp generic_int_handler
int_handler_133:
    push QWORD 133
    jmp generic_int_handler
int_handler_134:
    push QWORD 134
    jmp generic_int_handler
int_handler_135:
    push QWORD 135
    jmp generic_int_handler
int_handler_136:
    push QWORD 136
    jmp generic_int_handler
int_handler_137:
    push QWORD 137
    jmp generic_int_handler
int_handler_138:
    push QWORD 138
    jmp generic_int_handler
int_handler_139:
    push QWORD 139
    jmp generic_int_handler
int_handler_140:
    push QWORD 140
    jmp generic_int_handler
int_handler_141:
    push QWORD 141
    jmp generic_int_handler
int_handler_142:
    push QWORD 142
    jmp generic_int_handler
int_handler_143:
    push QWORD 143
    jmp generic_int_handler
int_handler_144:
    push QWORD 144
    jmp generic_int_handler
int_handler_145:
    push QWORD 145
    jmp generic_int_handler
int_handler_146:
    push QWORD 146
    jmp generic_int_handler
int_handler_147:
    push QWORD 147
    jmp generic_int_handler
int_handler_148:
    push QWORD 148
    jmp generic_int_handler
int_handler_149:
    push QWORD 149
    jmp generic_int_handler
int_handler_150:
    push QWORD 150
    jmp generic_int_handler
int_handler_151:
    push QWORD 151
    jmp generic_int_handler
int_handler_152:
    push QWORD 152
    jmp generic_int_handler
int_handler_153:
    push QWORD 153
    jmp generic_int_handler
int_handler_154:
    push QWORD 154
    jmp generic_int_handler
int_handler_155:
    push QWORD 155
    jmp generic_int_handler
int_handler_156:
    push QWORD 156
    jmp generic_int_handler
int_handler_157:
    push QWORD 157
    jmp generic_int_handler
int_handler_158:
    push QWORD 158
    jmp generic_int_handler
int_handler_159:
    push QWORD 159
    jmp generic_int_handler
int_handler_160:
    push QWORD 160
    jmp generic_int_handler
int_handler_161:
    push QWORD 161
    jmp generic_int_handler
int_handler_162:
    push QWORD 162
    jmp generic_int_handler
int_handler_163:
    push QWORD 163
    jmp generic_int_handler
int_handler_164:
    push QWORD 164
    jmp generic_int_handler
int_handler_165:
    push QWORD 165
    jmp generic_int_handler
int_handler_166:
    push QWORD 166
    jmp generic_int_handler
int_handler_167:
    push QWORD 167
    jmp generic_int_handler
int_handler_168:
    push QWORD 168
    jmp generic_int_handler
int_handler_169:
    push QWORD 169
    jmp generic_int_handler
int_handler_170:
    push QWORD 170
    jmp generic_int_handler
int_handler_171:
    push QWORD 171
    jmp generic_int_handler
int_handler_172:
    push QWORD 172
    jmp generic_int_handler
int_handler_173:
    push QWORD 173
    jmp generic_int_handler
int_handler_174:
    push QWORD 174
    jmp generic_int_handler
int_handler_175:
    push QWORD 175
    jmp generic_int_handler
int_handler_176:
    push QWORD 176
    jmp generic_int_handler
int_handler_177:
    push QWORD 177
    jmp generic_int_handler
int_handler_178:
    push QWORD 178
    jmp generic_int_handler
int_handler_179:
    push QWORD 179
    jmp generic_int_handler
int_handler_180:
    push QWORD 180
    jmp generic_int_handler
int_handler_181:
    push QWORD 181
    jmp generic_int_handler
int_handler_182:
    push QWORD 182
    jmp generic_int_handler
int_handler_183:
    push QWORD 183
    jmp generic_int_handler
int_handler_184:
    push QWORD 184
    jmp generic_int_handler
int_handler_185:
    push QWORD 185
    jmp generic_int_handler
int_handler_186:
    push QWORD 186
    jmp generic_int_handler
int_handler_187:
    push QWORD 187
    jmp generic_int_handler
int_handler_188:
    push QWORD 188
    jmp generic_int_handler
int_handler_189:
    push QWORD 189
    jmp generic_int_handler
int_handler_190:
    push QWORD 190
    jmp generic_int_handler
int_handler_191:
    push QWORD 191
    jmp generic_int_handler
int_handler_192:
    push QWORD 192
    jmp generic_int_handler
int_handler_193:
    push QWORD 193
    jmp generic_int_handler
int_handler_194:
    push QWORD 194
    jmp generic_int_handler
int_handler_195:
    push QWORD 195
    jmp generic_int_handler
int_handler_196:
    push QWORD 196
    jmp generic_int_handler
int_handler_197:
    push QWORD 197
    jmp generic_int_handler
int_handler_198:
    push QWORD 198
    jmp generic_int_handler
int_handler_199:
    push QWORD 199
    jmp generic_int_handler
int_handler_200:
    push QWORD 200
    jmp generic_int_handler
int_handler_201:
    push QWORD 201
    jmp generic_int_handler
int_handler_202:
    push QWORD 202
    jmp generic_int_handler
int_handler_203:
    push QWORD 203
    jmp generic_int_handler
int_handler_204:
    push QWORD 204
    jmp generic_int_handler
int_handler_205:
    push QWORD 205
    jmp generic_int_handler
int_handler_206:
    push QWORD 206
    jmp generic_int_handler
int_handler_207:
    push QWORD 207
    jmp generic_int_handler
int_handler_208:
    push QWORD 208
    jmp generic_int_handler
int_handler_209:
    push QWORD 209
    jmp generic_int_handler
int_handler_210:
    push QWORD 210
    jmp generic_int_handler
int_handler_211:
    push QWORD 211
    jmp generic_int_handler
int_handler_212:
    push QWORD 212
    jmp generic_int_handler
int_handler_213:
    push QWORD 213
    jmp generic_int_handler
int_handler_214:
    push QWORD 214
    jmp generic_int_handler
int_handler_215:
    push QWORD 215
    jmp generic_int_handler
int_handler_216:
    push QWORD 216
    jmp generic_int_handler
int_handler_217:
    push QWORD 217
    jmp generic_int_handler
int_handler_218:
    push QWORD 218
    jmp generic_int_handler
int_handler_219:
    push QWORD 219
    jmp generic_int_handler
int_handler_220:
    push QWORD 220
    jmp generic_int_handler
int_handler_221:
    push QWORD 221
    jmp generic_int_handler
int_handler_222:
    push QWORD 222
    jmp generic_int_handler
int_handler_223:
    push QWORD 223
    jmp generic_int_handler
int_handler_224:
    push QWORD 224
    jmp generic_int_handler
int_handler_225:
    push QWORD 225
    jmp generic_int_handler
int_handler_226:
    push QWORD 226
    jmp generic_int_handler
int_handler_227:
    push QWORD 227
    jmp generic_int_handler
int_handler_228:
    push QWORD 228
    jmp generic_int_handler
int_handler_229:
    push QWORD 229
    jmp generic_int_handler
int_handler_230:
    push QWORD 230
    jmp generic_int_handler
int_handler_231:
    push QWORD 231
    jmp generic_int_handler
int_handler_232:
    push QWORD 232
    jmp generic_int_handler
int_handler_233:
    push QWORD 233
    jmp generic_int_handler
int_handler_234:
    push QWORD 234
    jmp generic_int_handler
int_handler_235:
    push QWORD 235
    jmp generic_int_handler
int_handler_236:
    push QWORD 236
    jmp generic_int_handler
int_handler_237:
    push QWORD 237
    jmp generic_int_handler
int_handler_238:
    push QWORD 238
    jmp generic_int_handler
int_handler_239:
    push QWORD 239
    jmp generic_int_handler
int_handler_240:
    push QWORD 240
    jmp generic_int_handler
int_handler_241:
    push QWORD 241
    jmp generic_int_handler
int_handler_242:
    push QWORD 242
    jmp generic_int_handler
int_handler_243:
    push QWORD 243
    jmp generic_int_handler
int_handler_244:
    push QWORD 244
    jmp generic_int_handler
int_handler_245:
    push QWORD 245
    jmp generic_int_handler
int_handler_246:
    push QWORD 246
    jmp generic_int_handler
int_handler_247:
    push QWORD 247
    jmp generic_int_handler
int_handler_248:
    push QWORD 248
    jmp generic_int_handler
int_handler_249:
    push QWORD 249
    jmp generic_int_handler
int_handler_250:
    push QWORD 250
    jmp generic_int_handler
int_handler_251:
    push QWORD 251
    jmp generic_int_handler
int_handler_252:
    push QWORD 252
    jmp generic_int_handler
int_handler_253:
    push QWORD 253
    jmp generic_int_handler
int_handler_254:
    push QWORD 254
    jmp generic_int_handler
int_handler_255:
    push QWORD 255
    jmp generic_int_handler
    
    
SECTION .data
  
ALIGN 16

isr_table:
    dq int_handler_0
    dq int_handler_1
    dq int_handler_2
    dq int_handler_3
    dq int_handler_4
    dq int_handler_5
    dq int_handler_6
    dq int_handler_7
    dq int_handler_8
    dq int_handler_9
    dq int_handler_10
    dq int_handler_11
    dq int_handler_12
    dq int_handler_13
    dq int_handler_14
    dq int_handler_15
    dq int_handler_16
    dq int_handler_17
    dq int_handler_18
    dq int_handler_19
    dq int_handler_20
    dq int_handler_21
    dq int_handler_22
    dq int_handler_23
    dq int_handler_24
    dq int_handler_25
    dq int_handler_26
    dq int_handler_27
    dq int_handler_28
    dq int_handler_29
    dq int_handler_30
    dq int_handler_31
    dq int_handler_32
    dq int_handler_33
    dq int_handler_34
    dq int_handler_35
    dq int_handler_36
    dq int_handler_37
    dq int_handler_38
    dq int_handler_39
    dq int_handler_40
    dq int_handler_41
    dq int_handler_42
    dq int_handler_43
    dq int_handler_44
    dq int_handler_45
    dq int_handler_46
    dq int_handler_47
    dq int_handler_48
    dq int_handler_49
    dq int_handler_50
    dq int_handler_51
    dq int_handler_52
    dq int_handler_53
    dq int_handler_54
    dq int_handler_55
    dq int_handler_56
    dq int_handler_57
    dq int_handler_58
    dq int_handler_59
    dq int_handler_60
    dq int_handler_61
    dq int_handler_62
    dq int_handler_63
    dq int_handler_64
    dq int_handler_65
    dq int_handler_66
    dq int_handler_67
    dq int_handler_68
    dq int_handler_69
    dq int_handler_70
    dq int_handler_71
    dq int_handler_72
    dq int_handler_73
    dq int_handler_74
    dq int_handler_75
    dq int_handler_76
    dq int_handler_77
    dq int_handler_78
    dq int_handler_79
    dq int_handler_80
    dq int_handler_81
    dq int_handler_82
    dq int_handler_83
    dq int_handler_84
    dq int_handler_85
    dq int_handler_86
    dq int_handler_87
    dq int_handler_88
    dq int_handler_89
    dq int_handler_90
    dq int_handler_91
    dq int_handler_92
    dq int_handler_93
    dq int_handler_94
    dq int_handler_95
    dq int_handler_96
    dq int_handler_97
    dq int_handler_98
    dq int_handler_99
    dq int_handler_100
    dq int_handler_101
    dq int_handler_102
    dq int_handler_103
    dq int_handler_104
    dq int_handler_105
    dq int_handler_106
    dq int_handler_107
    dq int_handler_108
    dq int_handler_109
    dq int_handler_110
    dq int_handler_111
    dq int_handler_112
    dq int_handler_113
    dq int_handler_114
    dq int_handler_115
    dq int_handler_116
    dq int_handler_117
    dq int_handler_118
    dq int_handler_119
    dq int_handler_120
    dq int_handler_121
    dq int_handler_122
    dq int_handler_123
    dq int_handler_124
    dq int_handler_125
    dq int_handler_126
    dq int_handler_127
    dq int_handler_128
    dq int_handler_129
    dq int_handler_130
    dq int_handler_131
    dq int_handler_132
    dq int_handler_133
    dq int_handler_134
    dq int_handler_135
    dq int_handler_136
    dq int_handler_137
    dq int_handler_138
    dq int_handler_139
    dq int_handler_140
    dq int_handler_141
    dq int_handler_142
    dq int_handler_143
    dq int_handler_144
    dq int_handler_145
    dq int_handler_146
    dq int_handler_147
    dq int_handler_148
    dq int_handler_149
    dq int_handler_150
    dq int_handler_151
    dq int_handler_152
    dq int_handler_153
    dq int_handler_154
    dq int_handler_155
    dq int_handler_156
    dq int_handler_157
    dq int_handler_158
    dq int_handler_159
    dq int_handler_160
    dq int_handler_161
    dq int_handler_162
    dq int_handler_163
    dq int_handler_164
    dq int_handler_165
    dq int_handler_166
    dq int_handler_167
    dq int_handler_168
    dq int_handler_169
    dq int_handler_170
    dq int_handler_171
    dq int_handler_172
    dq int_handler_173
    dq int_handler_174
    dq int_handler_175
    dq int_handler_176
    dq int_handler_177
    dq int_handler_178
    dq int_handler_179
    dq int_handler_180
    dq int_handler_181
    dq int_handler_182
    dq int_handler_183
    dq int_handler_184
    dq int_handler_185
    dq int_handler_186
    dq int_handler_187
    dq int_handler_188
    dq int_handler_189
    dq int_handler_190
    dq int_handler_191
    dq int_handler_192
    dq int_handler_193
    dq int_handler_194
    dq int_handler_195
    dq int_handler_196
    dq int_handler_197
    dq int_handler_198
    dq int_handler_199
    dq int_handler_200
    dq int_handler_201
    dq int_handler_202
    dq int_handler_203
    dq int_handler_204
    dq int_handler_205
    dq int_handler_206
    dq int_handler_207
    dq int_handler_208
    dq int_handler_209
    dq int_handler_210
    dq int_handler_211
    dq int_handler_212
    dq int_handler_213
    dq int_handler_214
    dq int_handler_215
    dq int_handler_216
    dq int_handler_217
    dq int_handler_218
    dq int_handler_219
    dq int_handler_220
    dq int_handler_221
    dq int_handler_222
    dq int_handler_223
    dq int_handler_224
    dq int_handler_225
    dq int_handler_226
    dq int_handler_227
    dq int_handler_228
    dq int_handler_229
    dq int_handler_230
    dq int_handler_231
    dq int_handler_232
    dq int_handler_233
    dq int_handler_234
    dq int_handler_235
    dq int_handler_236
    dq int_handler_237
    dq int_handler_238
    dq int_handler_239
    dq int_handler_240
    dq int_handler_241
    dq int_handler_242
    dq int_handler_243
    dq int_handler_244
    dq int_handler_245
    dq int_handler_246
    dq int_handler_247
    dq int_handler_248
    dq int_handler_249
    dq int_handler_250
    dq int_handler_251
    dq int_handler_252
    dq int_handler_253
    dq int_handler_254
    dq int_handler_255



