
		IMPORT	dlopen_int
		; DLHANDLE dlopen_int( const WCHAR * uri, int mode, const WCHAR * cur_dir )
		
		IMPORT	loadedElfsList
		IMPORT	elfListFind
		;ldrElf * elfListFind( ELF_LIST_T * list, void * addr )


		AREA elfloader, CODE, READONLY

		CODE16


		EXPORT		dlopen
		; DLHANDLE	dlopen( const WCHAR * file, int mode );

dlopen ROUT
		PUSH	{R0-R1,LR}
		
		LDR		R0, =loadedElfsList
		MOV		R1, LR
		BL		elfListFind
		CMP		R0, #0
		BEQ		%0dlopen
		
		LDR		R2, [R0, #0x18]		; ldrElf.dir
		B		%1dlopen
		
0dlopen
		MOV		R2, #0
		
1dlopen
		POP		{R0-R1,R3}
		MOV		LR, R3
		LDR		R3, =dlopen_int
		BX		R3

		LTORG


		END
