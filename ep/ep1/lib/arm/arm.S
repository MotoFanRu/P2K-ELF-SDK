;			AREA |asm.my_ll_cmpu|, CODE, READONLY
;			ALIGN 4
;			CODE32
;			
;			EXPORT _ll_cmpu
;_ll_cmpu
;			CMP		R0, R2
;			CMPEQ	R1, R3
;			BX		LR
; End of function my_ll_cmpu
;			DCD		0x00000000	; чтобы работало с лоадером независимо от наличия ф-ции в либе
;			LTORG

			


; ----------------------------------------------------------------------------
; __call_via_r*
			AREA |asm.__call_via_r0|, CODE, READONLY
			ALIGN 4
			CODE16
			EXPORT __call_via_r0
__call_via_r0
			BX		R0
; End of function __call_via_r0
			LTORG
			
			AREA |asm.__call_via_r1|, CODE, READONLY
			CODE16
			EXPORT __call_via_r1
__call_via_r1
			BX		R1
; End of function __call_via_r1
			LTORG
			
			AREA |asm.__call_via_r2|, CODE, READONLY
			CODE16
			EXPORT __call_via_r2
__call_via_r2
			BX		R2
; End of function __call_via_r2
			LTORG

			AREA |asm.__call_via_r3|, CODE, READONLY
			CODE16
			EXPORT __call_via_r3
__call_via_r3
			BX		R3
; End of function __call_via_r2
			LTORG
			
			AREA |asm.__call_via_r4|, CODE, READONLY
			CODE16
			EXPORT __call_via_r4
__call_via_r4
			BX		R4
; End of function __call_via_r4
			LTORG
			
			
			AREA |asm.__call_via_r5|, CODE, READONLY
			CODE16
			EXPORT __call_via_r5
__call_via_r5
			BX		R5
; End of function __call_via_r5
			LTORG
			
			AREA |asm.__call_via_r6|, CODE, READONLY
			CODE16
			EXPORT __call_via_r6
__call_via_r6
			BX		R6
; End of function __call_via_r6
			LTORG
			
			AREA |asm.__call_via_r7|, CODE, READONLY
			CODE16
			EXPORT __call_via_r7
__call_via_r7
			BX		R7
; End of function __call_via_r7
			LTORG
			
; --------------------------------------------------------------------
; C++ new & delete operators
			AREA |asm.cpp_operators|, CODE, READONLY
			CODE16
			
; new
__nw__FUi
			PUSH	{R3,LR}
			MOV	R1, #0
			STR	R1, [SP]
			MOV	R1, SP
			BL	suAllocMem
			LDR	R1, [SP]
			CMP	R1, #0
			BEQ	loc_14
			MOV	R0, #0
loc_14
			ADD	SP, SP,	#4
			POP	{R3}
			BX	R3
; End of function __nw(uint)
			
; delete
__dl__FPv
			PUSH	{R7,LR}
			CMP	R0, #0
			BEQ	loc_24
			BL	suFreeMem
loc_24
			POP	{R7}
			POP	{R3}
			BX	R3
; End of function __dl(void *)

; new []
__na__FUi
			PUSH	{R3,LR}
			MOV	R1, #0
			STR	R1, [SP]
			MOV	R1, SP
			BL	suAllocMem
			LDR	R1, [SP]
			CMP	R1, #0
			BEQ	loc_3E
			MOV	R0, #0
loc_3E
			ADD	SP, SP,	#4
			POP	{R3}
			BX	R3
; End of function __na(uint)


; delete []
__da__FPv
			PUSH	{R7,LR}
			CMP	R0, #0
			BEQ	loc_4E
			BL	suFreeMem
loc_4E
			POP	{R7}
			POP	{R3}
			BX	R3
; End of function __da(void *)

			EXPORT __nw__FUi
			EXPORT __dl__FPv
			EXPORT __na__FUi
			EXPORT __da__FPv
			
			IMPORT suAllocMem
			IMPORT suFreeMem


			END
			