;
; Project:
;   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
;
; About:
;   Auto-generated ARM stub library for ARM ADS.
;
; Author:
;   EXL, 04-Dec-2025
;

	AREA Lib, CODE, READONLY
	ALIGN 4

	AREA |f.__call_via_r2|, CODE, READONLY
	CODE16
__call_via_r2
	BX		R2
	LTORG

	EXPORT __call_via_r2

	END
