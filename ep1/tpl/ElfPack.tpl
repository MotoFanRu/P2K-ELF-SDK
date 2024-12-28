/* Motorola P2K phones on Neptune LTE1 SoC memory map like Motorola E398 or Motorola ROKR E1. */
MEMORY {
	IROM  (RX)  : ORIGIN = 0x00000000, LENGTH = 0x001C0000 /* 1.75 MiB     */
	IRAM  (RWX) : ORIGIN = 0x03FC0000, LENGTH = 0x00040000 /* 256  KiB     */
	ROM   (RX)  : ORIGIN = 0x10000000, LENGTH = 0x02000000 /* 32   MiB     */
	RAM   (RWX) : ORIGIN = 0x12000000, LENGTH = 0x00800000 /* 8    MiB     */

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
