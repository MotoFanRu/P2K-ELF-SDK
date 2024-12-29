.syntax unified

.section .text.funcT
.thumb
.thumb_func
.type funcT, %function
funcT:
	bx pc

.arm
.type funcT32, %function
funcT32:
	ldr   r12, =0x10867509
	bx    r12
	.ltorg

.section .text.funcA
.thumb
.thumb_func
.type funcA, %function
funcA:
	bx pc

.arm
.type funcA32, %function
funcA32:
	ldr   r12, =0x10867500
	bx    r12
	.ltorg


.section .text.funcTL
.thumb
.thumb_func
.type funcTL, %function
funcTL:
	bx pc

.arm
.type funcTL32, %function
funcTL32:
	ldr   r12, =0x00000101
	bx    r12
	.ltorg

.section .text.funcAL
.thumb
.thumb_func
.type funcAL, %function
funcAL:
	bx pc

.arm
.type funcAL32, %function
funcAL32:
	ldr   r12, =0x00000200
	bx    r12
	.ltorg


.global funcT
.global funcT32
.global funcA
.global funcA32
.global funcTL
.global funcTL32
.global funcAL
.global funcAL32

@END
