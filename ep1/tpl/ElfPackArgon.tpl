/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   Linker script for Motorola P2K phones on the Argon LV SoC including a memory map.
 *   Supports models: Motorola RAZR V3xx, Motorola KRZR K3, Motorola RAZR V6 maxx, etc.
 *
 * Author:
 *   EXL, 03-Jan-2025
 */

MEMORY {
	IROM1 (RX)  : ORIGIN = 0x00000000, LENGTH = 0x00004000 /* 16 KiB     */
	IROM2 (RX)  : ORIGIN = 0x00404000, LENGTH = 0x00004000 /* 16 KiB     */
	IRAM  (RWX) : ORIGIN = 0x00000000, LENGTH = 0x00000000 /* ?  KiB     */ /* Unknown for ArgonLV. */
	ROM   (RX)  : ORIGIN = 0xA0000000, LENGTH = 0x04000000 /* 64 MiB     */
	RAM   (RWX) : ORIGIN = 0x80000000, LENGTH = 0x02000000 /* 32 MiB     */

	/* Entry point origin address. */
	PATCH (RX)  : ORIGIN = %addr_entry%, LENGTH = ORIGIN(ROM) + LENGTH(ROM) - ORIGIN(PATCH)
}

/* Generate Big-Endian ARM ELFs. */
OUTPUT_FORMAT("elf32-bigarm")

/* Entry point function. */
ENTRY(AutorunMain)

/* Combine all relevant section to one. */

SECTIONS {
	.text : {
		*(.text .text.*)
		*(.rodata .rodata.*)
		*(.data .data.*)
		*(.bss .bss.*)
		*(COMMON COMMON.*)
	} > PATCH
}
