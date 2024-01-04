
		IMPORT	unloadElf


		AREA elfloader, CODE, READONLY

		CODE16

; UINT32 ldrUnloadElf( void )
		EXPORT		ldrUnloadElf
ldrUnloadElf
		MOV		R0, LR
		LDR		R1, =unloadElf
		BX		R1

		LTORG


		END
