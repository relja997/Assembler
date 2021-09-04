.section .text
ret
sub r3, r1
iret
ldr r1, $labela
int r6
jmp 12
halt
ret
jeq %q
add r1, r2
ldr r0, r9asdc
str r3, labela
str r3, 9
push r0
ldr r7, r6
jmp *r1
ldr r4, $s
call printf
push r3
jeq *r7
str r0, [r3]
ldr r1, [r3 + labela]
pop r16
str r0, [r2 + 0x11]
ldr r6, [r0]
jne *label
jeq *r7as
not r1
.section .data
jgt *[r7 + simbol]
jmp *[r0 + 3]
mul r1, r2
xor r4, r7
ret
jne *[r5]
xchg r2, r6
halt
iret
div r2, r8
halt
test r3, r6
shl r1, r9

.end