.global a, b, main # scmkdsvnuebv
.extern printf # ndnvhsvbh

# ovaj slucaj
.section proba # jdsvnrjsknv
push r3
pr: 
zh:
    add r3, r6 
.word 0x11, -21, a
call printf
.skip 3

.section .rodata
msg: 
    .word    0x97, -122, 4097, -6000, 39, 16, main, pr
.skip    4
ew: # sanfj
.equ    b, 3

.section   .text
main:
    push    r0
    xchg     r0, r3
    call printf
simb:   cmp r4, r5 # 10
    jne    *[r2]
    jeq     *[ r0 + b]
    jgt *[  r3  +   14]
a: # 25
    jmp     -4097
    call printf
    jne main
    jeq simb
    .skip 2
    .word ew, 16, 0x16, -128
    jgt  *r3
t:
    jgt   *simb
    jmp         %main
q: 
    ldr         r0, $14



    
    str             r7, $simb
    str     r1, %ew
    ldr         r0, r1
    ldr r4, %a
    str r6, %q
    ldr r0, %main
    str r6, 0x15
    ldr r2, ew
    str     r4,     r6
    str r3, [r5]
    ldr r1, [r4 + 0x12]
    str r3, [r0 + simb]
lab: 
    add r1, r2
    not r1
    halt
    mul r3, r4
    int r7
    shl r0, r1
    iret
    test r7, r6
    xor r0, r4
    push r1
    pop r0
brm:
.end
