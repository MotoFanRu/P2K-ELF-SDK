/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   A standard ELF header from the GNU C Library but truncated.
 *
 * Author:
 *   Free Software Foundation, 1995-2024
 */

/* This file defines standard ELF types, structures, and macros.
   Copyright (C) 1995-2024 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>. */

#ifndef ELF_H
#define ELF_H

#include <typedefs.h>

typedef UINT32                         Elf32_Addr;         // Unsigned program address.
typedef UINT16                         Elf32_Half;         // Unsigned medium integer.
typedef UINT32                         Elf32_Off;          // Unsigned file offset.
typedef INT32                          Elf32_Sword;        // Signed large integer.
typedef UINT32                         Elf32_Word;         // Unsigned large integer.
typedef UINT8                          Elf32_Byte;         // Unsigned char 8-bit integer.

// The ELF file header. This appears at the start of every ELF file.
#define EI_NIDENT                      (16)

typedef struct {
	UINT8                              e_ident[EI_NIDENT]; // Magic number and other info.
	Elf32_Half                         e_type;             // Object file type.
	Elf32_Half                         e_machine;          // Architecture.
	Elf32_Word                         e_version;          // Object file version.
	Elf32_Addr                         e_entry;            // Entry point virtual address.
	Elf32_Off                          e_phoff;            // Program header table file offset.
	Elf32_Off                          e_shoff;            // Section header table file offset.
	Elf32_Word                         e_flags;            // Processor-specific flags.
	Elf32_Half                         e_ehsize;           // ELF header size in bytes.
	Elf32_Half                         e_phentsize;        // Program header table entry size.
	Elf32_Half                         e_phnum;            // Program header table entry count.
	Elf32_Half                         e_shentsize;        // Section header table entry size.
	Elf32_Half                         e_shnum;            // Section header table entry count.
	Elf32_Half                         e_shstrndx;         // Section header string table index.
} Elf32_Ehdr;

#define ET_EXEC                        (2)                 // Executable file.
#define ET_DYN                         (3)                 // Shared object file.

// Program segment header.
typedef struct {
	Elf32_Word                         p_type;             // Segment type.
	Elf32_Off                          p_offset;           // Segment file offset.
	Elf32_Addr                         p_vaddr;            // Segment virtual address.
	Elf32_Addr                         p_paddr;            // Segment physical address.
	Elf32_Word                         p_filesz;           // Segment size in file.
	Elf32_Word                         p_memsz;            // Segment size in memory.
	Elf32_Word                         p_flags;            // Segment flags.
	Elf32_Word                         p_align;            // Segment alignment.
} Elf32_Phdr;

// Legal values for p_type (segment type).
#define PT_LOAD                        (1)                 // Loadable program segment.
#define PT_DYNAMIC                     (2)                 // Dynamic linking information.

// Dynamic section entry.
typedef struct {
	Elf32_Sword                        d_tag;              // Dynamic entry type.
	Elf32_Word                         d_val;              // Integer value.
} Elf32_Dyn;

// Legal values for d_tag (dynamic entry type).
#define DT_PLTRELSZ                    (2)                 // Size in bytes of PLT relocs.
#define DT_STRTAB                      (5)                 // Address of string table.
#define DT_SYMTAB                      (6)                 // Address of symbol table.
#define DT_REL                         (17)                // Address of Rel relocs.
#define DT_RELSZ                       (18)                // Total size of Rel relocs.
#define DT_JMPREL                      (23)                // Address of PLT relocs.
#define DT_BIND_NOW                    (24)                // Process relocations of object.

// Relocation table entry without addend (in section of type SHT_REL).
typedef struct {
	Elf32_Addr                         r_offset;           // Address.
	Elf32_Word                         r_info;             // Relocation type and symbol index.
} Elf32_Rel;

// How to extract and insert information held in the r_info field.
#define ELF32_R_SYM(val)               ((val) >> 8)
#define ELF32_R_TYPE(val)              ((val) & 0xFF)

// ARM relocation types.
#define R_ARM_ABS32                    (2)                 // Direct 32 bit.
#define R_ARM_RELATIVE                 (23)                // Adjust by program base.
#define R_ARM_RABS32                   (253)               // Word ?S + A For the address of a location in the target segment.

// Symbol table entry.
typedef struct {
	Elf32_Word                         st_name;            // Symbol name (string tbl index).
	Elf32_Addr                         st_value;           // Symbol value.
	Elf32_Word                         st_size;            // Symbol size.
	Elf32_Byte                         st_info;            // Symbol type and binding.
	Elf32_Byte                         st_other;           // Symbol visibility.
	Elf32_Half                         st_shndx;           // Section index.
} Elf32_Sym;

// Section header.
typedef struct {
	Elf32_Word                         sh_name;            // Section name (string tbl index).
	Elf32_Word                         sh_type;            // Section type.
	Elf32_Word                         sh_flags;           // Section flags.
	Elf32_Addr                         sh_addr;            // Section virtual addr at execution.
	Elf32_Off                          sh_offset;          // Section file offset.
	Elf32_Word                         sh_size;            // Section size in bytes.
	Elf32_Word                         sh_link;            // Link to another section.
	Elf32_Word                         sh_info;            // Additional section information.
	Elf32_Word                         sh_addralign;       // Section alignment.
	Elf32_Word                         sh_entsize;         // Entry size if section holds table.
} Elf32_Shdr;

#define SHT_SYMTAB                     (2)                 // Symbol table.
#define SHT_STRTAB                     (3)                 // String table.

// How to extract and insert information held in the st_info field.
#define ELF32_ST_BIND(val)             (((Elf32_Byte) (val)) >> 4)
#define ELF32_ST_TYPE(val)             ((val) & 0x0F)

// Legal values for ST_BIND subfield of st_info (symbol binding).
#define STB_GLOBAL                     (1)                 // Global symbol.

// Legal values for ST_TYPE subfield of st_info (symbol type).
#define STT_OBJECT                     (1)                 // Symbol is a data object.
#define STT_FUNC                       (2)                 // Symbol is a code object.

#endif /* ELF_H */
