;
; Project:
;   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
;
; About:
;   Auto-generated stub library for ARM ADS.
;
; Author:
;   EXL, 04-Dec-2025
;

	AREA |f.funcTS|, CODE, READONLY
	CODE16
funcTS
	BX    PC
	CODE32
funcTS32
	LDR   R12, =0x10867509
	BX    R12
	LTORG

	AREA |f.funcAS|, CODE, READONLY
	CODE16
funcAS
	BX    PC
	CODE32
funcAS32
	LDR   R12, =0x10867500
	BX    R12
	LTORG

	AREA |f.funcTL|, CODE, READONLY
	CODE16
funcTL
	BX    PC
	CODE32
funcTL32
	LDR   R12, =0x00000101
	BX    R12
	LTORG

	AREA |f.funcAL|, CODE, READONLY
	CODE16
funcAL
	BX    PC
	CODE32
funcAL32
	LDR   R12, =0x00000200
	BX    R12
	LTORG

	EXPORT funcTS
	EXPORT funcTS32
	EXPORT funcAS
	EXPORT funcAS32
	EXPORT funcTL
	EXPORT funcTL32
	EXPORT funcAL
	EXPORT funcAL32

	END
