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

	AREA |f.__call_via_r3|, CODE, READONLY
	CODE16
__call_via_r3
	BX		R3
	LTORG

	EXPORT __call_via_r3

	AREA |f.__call_via_r4|, CODE, READONLY
	CODE16
__call_via_r4
	BX		R4
	LTORG

	EXPORT __call_via_r4

	END
