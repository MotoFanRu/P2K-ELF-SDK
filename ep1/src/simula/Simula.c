/*
 * Project:
 *   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
 *
 * About:
 *   The Simula project is designed to debug the ElfLoader part of ElfPack, which is used to launch ELFs.
 *
 * Author:
 *   EXL, 06-Jan-2025
 */

#include "hacks.h"

#include "ElfLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <byteswap.h>

// EXL, 23-Dec-2024: Compare two ANSI strings.
UINT32 namecmp(const char *ansi_str_1, const char *ansi_str_2) {
	UINT32 i;

	i = 0;

	while (ansi_str_1[i] == ansi_str_2[i]) {
		if (ansi_str_1[i++] == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

UINT32 loadELF(char *file_uri, char *params, void *Library, UINT32 reserve, IRAM_ELF_T *iram_elf) {
	UINT32          i;
	UINT32          j;

	BOOL            is_ads_elf;
#if !defined(SIMULA)
	FS_HANDLE_T     file;
	FS_COUNT_T      read;
#else
	FILE            *file;
#endif
	Elf32_Ehdr      elfHeader;
	// EXL, 25-Dec-2024: It's set to elfProgramHeaders[8], but there are only 2 in ELFs compiled with ADS.
	// EXL, 07-Jan-2025: Drop hardcoded MAX_PROG_HEADERS=8 array on stack.
	Elf32_Phdr      *elfProgramHeaders;

	Elf32_Addr      virtBase;           // = 0;
	Elf32_Addr      physBase;           // = 0;
	Elf32_Addr      upperAddr;          // = 0;
	Elf32_Addr      dynSegment;         // = 0;

	UINT32          sumMem;             // = 0;
	UINT32          sumSize;            // = 0;

	Elf32_Word      dynTags[DT_BIND_NOW + 1];
	Elf32_Dyn       tag;
	Elf32_Word      relType;
	Elf32_Rel       *relTable;

	UINT32          ldrNumSymbols;
	UINT32          elfNumSymbols;      // = 0;
	UINT32          elfStrTableSize;    // = 0;
	Ldr_Sym         *ldrSymTable;
	Elf32_Sym       *elfSymTable;       // = NULL;
	char            *ldrStrTable;
	char            *elfStrTable;       // = NULL;
	Elf32_Shdr      elfSectionHeader;
	WCHAR           wstr[WCHAR_PARAMS_MAX];

	virtBase        = 0;
	physBase        = 0;
	upperAddr       = 0;
	dynSegment      = 0;

	sumMem          = 0;
	sumSize         = 0;

	elfNumSymbols   = 0;
	elfStrTableSize = 0;

	elfSymTable     = NULL;
	elfStrTable     = NULL;

	UtilLogStringData("ElfLdr Load Request %s", file_uri);

#if !defined(SIMULA)
	// EXL, 23-Dec-2024: Copy ELF uri to WCHAR string then open ELF and read ELF header.
	u_atou((const char *) file_uri, wstr);
	file = DL_FsOpenFile((WCHAR *) wstr, FILE_READ_MODE, 0);
	if (file == FS_HANDLE_INVALID) {
		return ELDR_OPEN_FAILED;
	}
#else
	file = fopen(file_uri, "rb");
	if (!file) {
		UtilLogStringData("Cannot open ELF '%s' file\n", file_uri);
		return ELDR_OPEN_FAILED;
	}
#endif

	if (DL_FsReadFile((void *) &elfHeader, sizeof(Elf32_Ehdr), 1, file, &read) != RESULT_OK) {
		fprintf(stderr, "LOL");
		return ELDR_READ_HEADER_FAILED;
	}

#if defined(SIMULA)
	elfHeader.e_type               = B16(elfHeader.e_type);
	elfHeader.e_machine            = B16(elfHeader.e_machine);
	elfHeader.e_version            = B32(elfHeader.e_version);
	elfHeader.e_entry              = B32(elfHeader.e_entry);
	elfHeader.e_phoff              = B32(elfHeader.e_phoff);
	elfHeader.e_shoff              = B32(elfHeader.e_shoff);
	elfHeader.e_flags              = B32(elfHeader.e_flags);
	elfHeader.e_ehsize             = B16(elfHeader.e_ehsize);
	elfHeader.e_phentsize          = B16(elfHeader.e_phentsize);
	elfHeader.e_phnum              = B16(elfHeader.e_phnum);
	elfHeader.e_shentsize          = B16(elfHeader.e_shentsize);
	elfHeader.e_shnum              = B16(elfHeader.e_shnum);
	elfHeader.e_shstrndx           = B16(elfHeader.e_shstrndx);
#endif

	is_ads_elf = (elfHeader.e_type == ET_EXEC); // ET_DYN is GCC elf.

	UtilLogStringData(
		"Elf is loading...\n\nELF header:\n  e_entry  0x%X\n  e_phoff  0x%X\n  e_phnum  %d\n  e_type  %d\n\n",
		elfHeader.e_entry, elfHeader.e_phoff, elfHeader.e_phnum, elfHeader.e_type
	);

	// EXL, 07-Jan-2025: Allocate memory for program headers.
	elfProgramHeaders = suAllocMem(sizeof(Elf32_Phdr) * elfHeader.e_phnum, NULL);
	UtilLogStringData("Allocated %d bytes for elfProgramHeaders.\n\n", sizeof(Elf32_Phdr) * elfHeader.e_phnum);

	// EXL, 24-Dec-2024: ~0 is 0xFFFFFFFF.
	upperAddr =  0;
	virtBase  = ~0;

	// EXL, 24-Dec-2024: Read the program header tables and determine how many RAM they eats.
	for (i = 0; i < elfHeader.e_phnum; i++) {
		// EXL, 24-Dec-2024: Seek to the program header tables and read them.
		if (DL_FsFSeekFile(file, elfHeader.e_phoff + (i * elfHeader.e_phentsize), SEEK_WHENCE_SET) != RESULT_OK) {
			suFreeMem(elfProgramHeaders);
			return ELDR_SEEK_FAILED;
		}
		if (DL_FsReadFile((void *) &elfProgramHeaders[i], sizeof(Elf32_Phdr), 1, file, &read) != RESULT_OK) {
			suFreeMem(elfProgramHeaders);
			return ELDR_READ_FAILED;
		}

#if defined(SIMULA)
		elfProgramHeaders[i].p_type    = B32(elfProgramHeaders[i].p_type);
		elfProgramHeaders[i].p_offset  = B32(elfProgramHeaders[i].p_offset);
		elfProgramHeaders[i].p_vaddr   = B32(elfProgramHeaders[i].p_vaddr);
		elfProgramHeaders[i].p_paddr   = B32(elfProgramHeaders[i].p_paddr);
		elfProgramHeaders[i].p_filesz  = B32(elfProgramHeaders[i].p_filesz);
		elfProgramHeaders[i].p_memsz   = B32(elfProgramHeaders[i].p_memsz);
		elfProgramHeaders[i].p_flags   = B32(elfProgramHeaders[i].p_flags);
		elfProgramHeaders[i].p_align   = B32(elfProgramHeaders[i].p_align);
#endif

		UtilLogStringData(
			"Segment #%d:\n  p_type  %d\n  p_vaddr  0x%X\n  p_memsz  0x%X\n",
			i, elfProgramHeaders[i].p_type, elfProgramHeaders[i].p_vaddr, elfProgramHeaders[i].p_memsz
		);

		// EXL, 24-Dec-2024: Set a lower [<======>] upper memory addresses for program segments.
		if (elfProgramHeaders[i].p_type == PT_LOAD) {
			if (elfProgramHeaders[i].p_vaddr < virtBase) {
				virtBase = elfProgramHeaders[i].p_vaddr;
			}
			if ((elfProgramHeaders[i].p_vaddr + elfProgramHeaders[i].p_memsz) > upperAddr) {
				upperAddr = elfProgramHeaders[i].p_vaddr + elfProgramHeaders[i].p_memsz;
			}
		}

		sumMem  += elfProgramHeaders[i].p_memsz;
		sumSize += elfProgramHeaders[i].p_filesz;
	}

	if (B32(*((Elf32_Addr *) &elfHeader.e_ident[0x0C]))) {
		// EXL, 07-May-2025: Enable loading small ELFs to IRAM.
		Elf32_Addr iram = B32(*((Elf32_Addr *) &elfHeader.e_ident[0x0C]));
		Elf32_Addr eram = (Elf32_Addr) suAllocMem(upperAddr - virtBase, NULL);

		UtilLogStringData("Using IRAM for ELF! iram=0x%08X, eram=0x%08X, size=%d\n", iram, eram, upperAddr - virtBase);

#if defined(SIMULA)
		iram = (Elf32_Addr) suAllocMem(upperAddr - virtBase, NULL);
#endif

		iram_elf->iram_mem = (Elf32_Addr *) iram;
		iram_elf->eram_mem = (Elf32_Addr *) eram;
		iram_elf->size_mem = upperAddr - virtBase;

		memcpy((Elf32_Addr *) eram, (Elf32_Addr *) iram, upperAddr - virtBase);

		physBase = iram;
	} else {
		// EXL, 24-Dec-2024: Allocate RAM memory for program segments then clear its.
		physBase = (Elf32_Addr) suAllocMem(upperAddr - virtBase, NULL);
	}
	memclr((void *) physBase, upperAddr - virtBase);
	// EXL, 13-Apr-2025: Fix bug with loading a small GCC ELFs.
	memclr(dynTags, (DT_BIND_NOW + 1) * sizeof(Elf32_Word));

	UtilLogStringData(
		"\nFinished analysis:\n  Virtual Base  0x%X\n  Mem Needed  0x%X\n  Upper Address  0x%X\n  "
		"Summary Mem  0x%X\n  Summary Size  0x%X\n  Physical Base  0x%X\n\n",
		virtBase, upperAddr - virtBase, upperAddr, sumMem, sumSize, physBase
	);

	// Andy51, 01-Nov-2007: editing API calls.
	// EXL, 24-Dec-2024: Library format is following:
	//   4-byte integer: record count
	//   record array[] elements { offset_function_name_in_strings_array, function_address }
	//   record array[] elements { function_name, '\0' }

	ldrNumSymbols = B32(((UINT32 *) Library)[0]);
	ldrSymTable   = (Ldr_Sym *) &((UINT32 *) Library)[1];
	ldrStrTable   = (char *) &ldrSymTable[ldrNumSymbols]; // EXL, 24-Dec-2024: Empty for now.

	// EXL, 24-Dec-2024: Read and deploy the program segments from ELF to RAM.
	//   #define PT_LOAD                        (1)                 // Loadable program segment.
	//   #define PT_DYNAMIC                     (2)                 // Dynamic linking information.
	for (i = 0; i < elfHeader.e_phnum; i++) {
		switch (elfProgramHeaders[i].p_type) {
			case PT_LOAD:
				// EXL, 24-Dec-2024: Seek and read program segments with executable code to RAM.
				//   We interested in PT_LOAD and PT_DYNAMIC program sections.
				if (DL_FsFSeekFile(file, elfProgramHeaders[i].p_offset, SEEK_WHENCE_SET) != RESULT_OK) {
					suFreeMem((void *) physBase);
					suFreeMem(elfProgramHeaders);
					return ELDR_SEEK_FAILED;
				}

				UtilLogStringData(
					"  Segment #%d loading  0x%X  0x%d\n\n",
					i, physBase + elfProgramHeaders[i].p_vaddr - virtBase, elfProgramHeaders[i].p_filesz
				);

				if ((elfProgramHeaders[i].p_filesz > 0) &&
					(DL_FsReadFile(
						(void *) (physBase + elfProgramHeaders[i].p_vaddr - virtBase),
						elfProgramHeaders[i].p_filesz, 1, file, &read
					) != RESULT_OK)
				) {
					suFreeMem((void *) physBase);
					suFreeMem(elfProgramHeaders);
					return ELDR_READ_FAILED;
				}
				break;
			case PT_DYNAMIC:
				// EXL, 24-Dec-2024: Seek and read dynamic section with tag functions and addresses.
				if (is_ads_elf) {
					dynSegment = (Elf32_Addr) suAllocMem(elfProgramHeaders[i].p_filesz, NULL);
					if (DL_FsFSeekFile(file, elfProgramHeaders[i].p_offset, SEEK_WHENCE_SET) != RESULT_OK) {
						suFreeMem((void *) physBase);
						suFreeMem((void *) dynSegment);
						suFreeMem(elfProgramHeaders);
						return ELDR_SEEK_FAILED;
					}
					if (DL_FsReadFile((void *) dynSegment, elfProgramHeaders[i].p_filesz, 1, file, &read) != RESULT_OK) {
						suFreeMem((void *) physBase);
						suFreeMem((void *) dynSegment);
						suFreeMem(elfProgramHeaders);
						return ELDR_READ_FAILED;
					}
				} else {
					dynSegment = physBase + elfProgramHeaders[i].p_vaddr - virtBase;
				}

				j = 0;

				// Andy51, 01-Nov-2007: Load tags from dynamic segment.
				// EXL, 24-Dec-2024: Only part of table until 00 00 00 00 as d_tag. 00 00 00 00 00 00 00 00 is divider.
				do {
					tag = ((Elf32_Dyn *) dynSegment)[j++];
					tag.d_tag = B32(tag.d_tag);
					tag.d_val = B32(tag.d_val);
					if (tag.d_tag <= DT_BIND_NOW) {
						dynTags[tag.d_tag] = tag.d_val;
					}
				} while (tag.d_tag);

				UtilLogStringData(
					"Relocation start\n DT_REL  0x%X\n DT_RELSZ  %d\n", dynTags[DT_REL], dynTags[DT_RELSZ]
				);

				if (is_ads_elf) {
					relTable = (Elf32_Rel *) (dynSegment + dynTags[DT_REL] - elfProgramHeaders[i].p_vaddr);
				} else {
					relTable = (Elf32_Rel *) (dynTags[DT_REL] + physBase - virtBase);
					elfSymTable = (Elf32_Sym *) (physBase + dynTags[DT_SYMTAB] - virtBase);
				}

				// EXL, 24-Dec-2024: Translate dynamic section with tag functions to a real memory addresses.
				for (j = 0; j * sizeof(Elf32_Rel) < dynTags[DT_RELSZ]; j++) {
					relTable[j].r_info = B32(relTable[j].r_info);
					relTable[j].r_offset = B32(relTable[j].r_offset);
					// EXL, 24-Dec-2024: Only last byte of UINT32 r_info is relType.
					relType = ELF32_R_TYPE(relTable[j].r_info);

					UtilLogStringData(" Reloc #%d\n  Type  %d\n  Off  0x%X\n", j, relType, relTable[j].r_offset);

					if (relType == R_ARM_RABS32 || relType == R_ARM_RELATIVE) {
						UtilLogStringData(
							" R_ARM_RABS32\n  Old  0x%X\n  New  0x%X\n",
							B32(*((UINT32 *) (physBase + relTable[j].r_offset - virtBase))),
							B32(*((UINT32 *) (physBase + relTable[j].r_offset - virtBase)) + B32(physBase - virtBase))
						);
						if (reserve == 0xDEADBEEF) {
							*((UINT32 *) (physBase + relTable[j].r_offset - virtBase)) += B32(physBase - virtBase);
						}
					} else if (relType == R_ARM_ABS32 && !is_ads_elf) {
						// EXL, 01-Jan-2025: Add "R_ARM_ABS32" relocations handling for GCC ELFs.
						UINT32 k;
						Elf32_Addr elfStrTableAddr = physBase + dynTags[DT_STRTAB] - virtBase;
						INT32 sym_idx = ELF32_R_SYM(relTable[j].r_info);
						char *sym_str = (char *) (elfStrTableAddr + B32(elfSymTable[sym_idx].st_name));
						Elf32_Sym *sym = &elfSymTable[sym_idx];
						INT32 bind_type = ELF32_ST_BIND(sym->st_info);

						UtilLogStringData("  Bind Type %d Str: %s\n", bind_type, sym_str);

						// EXL, 01-Jan-2025: Point symbol to the ELF entry point aka first PT_LOAD segment if it not
						//   found in the library. Workaround for the Lib entry symbol is here.
						*((Elf32_Word *) (physBase + relTable[j].r_offset - virtBase)) = B32(physBase - virtBase);

						// EXL, 01-Jan-2025: Iterate over library entities and set all relocations paying
						//    attention to the DATA, 'D' symbols with shifting.
						for (k = 0; k < ldrNumSymbols; k++) {
							UINT32 ldr_st_name = B32(ldrSymTable[k].st_name);
							UINT32 ldr_st_addr = B32(ldrSymTable[k].st_value);
							if (namecmp(sym_str, &ldrStrTable[ldr_st_name]) == TRUE) {
								UtilLogStringData(
									"API Call #%d:\n  addr 0x%lX\n  old 0x%X\n  new 0x%X\n",
									j, B32(elfSymTable[j].st_value),
									B32(*((Elf32_Word *) (physBase + relTable[j].r_offset - virtBase))),
									(ldr_st_addr > DATA_SHIFT_OFFSET) ?
										(ldr_st_addr - DATA_SHIFT_OFFSET) : ldr_st_addr
								);

								if (reserve == 0xDEADBEEF) {
									*((Elf32_Word *) (physBase + relTable[j].r_offset - virtBase)) =
										(ldr_st_addr > DATA_SHIFT_OFFSET) ?
											B32(ldr_st_addr - DATA_SHIFT_OFFSET) : B32(ldr_st_addr);
								}
							}
						}
					} else {
						// EXL, 01-Jan-2025: Unknown relocation type, just debug and skip it.
						UtilLogStringData(" UNK Reloc #%d\n  Type  %d\n  Off 0x%X\n", j, relType, relTable[j].r_offset);
						// *((UINT32 *) (physBase + relTable[j].r_offset - virtBase)) = 0x00000000;
					}
				}
				break;
		}
	}

	// EXL, 07-Jan-2025: Free memory of allocated program headers.
	suFreeMem(elfProgramHeaders);
	UtilLogStringData("\nFreed %d bytes of elfProgramHeaders.\n\n", sizeof(Elf32_Phdr) * elfHeader.e_phnum);

	if (is_ads_elf) {
		// EXL, 31-Dec-2024: Delete a separate PT_DYNAMIC segment if present.
		if (dynSegment) {
			suFreeMem((void *) dynSegment);
		}

		// Andy51, 01-Nov-2007: Load elfSymTable and elfStrTable.
		// EXL, 25-Dec-2024: Go to the ELF sections and parse them. We interested in SHT_SYMTAB and SHT_STRTAB only.
		//   #define SHT_SYMTAB                     (2)                 // Symbol table.
		//   #define SHT_STRTAB                     (3)                 // String table.
		for (i = 0; i < elfHeader.e_shnum; i++) {
			// EXL, 25-Dec-2024: Seek and read other ELF sections.
			//   We interested in SHT_SYMTAB and SHT_STRTAB sections.
			if (DL_FsFSeekFile(file, elfHeader.e_shoff + i * elfHeader.e_shentsize, SEEK_WHENCE_SET) != RESULT_OK) {
				suFreeMem((void *) physBase);
				return ELDR_SEEK_FAILED;
			}
			if (DL_FsReadFile((void *) &elfSectionHeader, sizeof(Elf32_Shdr), 1, file, &read) != RESULT_OK) {
				suFreeMem((void *) physBase);
				return ELDR_READ_FAILED;
			}

#if defined(SIMULA)
			elfSectionHeader.sh_name      = B32(elfSectionHeader.sh_name);
			elfSectionHeader.sh_type      = B32(elfSectionHeader.sh_type);
			elfSectionHeader.sh_flags     = B32(elfSectionHeader.sh_flags);
			elfSectionHeader.sh_addr      = B32(elfSectionHeader.sh_addr);
			elfSectionHeader.sh_offset    = B32(elfSectionHeader.sh_offset);
			elfSectionHeader.sh_size      = B32(elfSectionHeader.sh_size);
			elfSectionHeader.sh_link      = B32(elfSectionHeader.sh_link);
			elfSectionHeader.sh_info      = B32(elfSectionHeader.sh_info);
			elfSectionHeader.sh_addralign = B32(elfSectionHeader.sh_addralign);
			elfSectionHeader.sh_entsize   = B32(elfSectionHeader.sh_entsize);
#endif

			UtilLogStringData(
				"Section #%d:\n  sh_type %d\n  sh_offset 0x%X\n  sh_size  0x%X\n",
				i, elfSectionHeader.sh_type, elfSectionHeader.sh_offset, elfSectionHeader.sh_size
			);

			if (elfSectionHeader.sh_type == SHT_SYMTAB) {
				// EXL, 25-Dec-2024: Alloc memory in RAM for Symbol table section then seek and read it.
				elfSymTable = (Elf32_Sym *) suAllocMem(elfSectionHeader.sh_size, NULL);
				if (DL_FsFSeekFile(file, elfSectionHeader.sh_offset, SEEK_WHENCE_SET) != RESULT_OK) {
					suFreeMem((void *) physBase);
					if (elfSymTable) { suFreeMem(elfSymTable); }
					if (elfStrTable) { suFreeMem(elfStrTable); }
					return ELDR_SEEK_FAILED;
				}
				if (DL_FsReadFile(elfSymTable, elfSectionHeader.sh_size, 1, file, &read) != RESULT_OK) {
					suFreeMem((void *) physBase);
					if (elfSymTable) { suFreeMem(elfSymTable); }
					if (elfStrTable) { suFreeMem(elfStrTable); }
					return ELDR_READ_FAILED;
				}

				// Andy51, 01-Nov-2007: sizeof(Elf32_Sym) here.
				// EXL, 25-Dec-2024: Count of sizeof(Elf32_Sym) elements in SHT_SYMTAB section.
				elfNumSymbols = elfSectionHeader.sh_size >> 4;
			} else if ((elfSectionHeader.sh_type == SHT_STRTAB) && (i != elfHeader.e_shstrndx)) {
				// EXL, 25-Dec-2024: Alloc memory in RAM for String table section then seek and read it.
				//   We want to skip last SHT_STRTAB section, it containts not function names but ELF section names.
				elfStrTable = (char *) suAllocMem(elfSectionHeader.sh_size, NULL);
				if (DL_FsFSeekFile(file, elfSectionHeader.sh_offset, SEEK_WHENCE_SET) != RESULT_OK) {
					suFreeMem((void *) physBase);
					if (elfSymTable) { suFreeMem(elfSymTable); }
					if (elfStrTable) { suFreeMem(elfStrTable); }
					return ELDR_SEEK_FAILED;
				}
				if (DL_FsReadFile(elfStrTable, elfSectionHeader.sh_size, 1, file, &read) != RESULT_OK) {
					suFreeMem((void *) physBase);
					if (elfSymTable) { suFreeMem(elfSymTable); }
					if (elfStrTable) { suFreeMem(elfStrTable); }
					return ELDR_READ_FAILED;
				}

				elfStrTableSize = elfSectionHeader.sh_size;
			}
		}

		UtilLogStringData(
			"API Calls Fix Start:  ldrStrTable 0x%X  ldrSymTable 0x%X  ldrNumSymbols %d",
			ldrStrTable, ldrSymTable, ldrNumSymbols
		);

		// Andy51, 01-Nov-2007: We look for matches and replace addresses.
		// EXL, 25-Dec-2024: We interested in global functions and data constants values.
		for (i = 0; i < elfNumSymbols; i++) {
#if defined(SIMULA)
			elfSymTable[i].st_name    = B32(elfSymTable[i].st_name);
			elfSymTable[i].st_value   = B32(elfSymTable[i].st_value);
			elfSymTable[i].st_size    = B32(elfSymTable[i].st_size);
			elfSymTable[i].st_info    = elfSymTable[i].st_info;
			elfSymTable[i].st_other   = elfSymTable[i].st_other;
			elfSymTable[i].st_shndx   = B16(elfSymTable[i].st_shndx);
#endif
			if (
				(ELF32_ST_BIND(elfSymTable[i].st_info) == STB_GLOBAL) &&
					((ELF32_ST_TYPE(elfSymTable[i].st_info) == STT_FUNC) ||
					((ELF32_ST_TYPE(elfSymTable[i].st_info) == STT_OBJECT) && (elfSymTable[i].st_size == 0)))
			) {
				UtilLogStringData(
					"Possible entry:  i %d  st_name %d  name %s",
					i, elfSymTable[i].st_name, &elfStrTable[elfSymTable[i].st_name]
				);

				// EXL, 25-Dec-2024: Iterate over library entities.
				for (j = 0; j < ldrNumSymbols; j++) {
					if (namecmp(&elfStrTable[elfSymTable[i].st_name], &ldrStrTable[B32(ldrSymTable[j].st_name)]) == TRUE) {
						UtilLogStringData(
							"API Call #%d:\n  addr 0x%X\n  old 0x%X\n  new 0x%X\n",
							i, elfSymTable[i].st_value,
							B32(*((UINT32 *) (physBase + elfSymTable[i].st_value - virtBase))),
							B32(ldrSymTable[j].st_value)
						);

						if (reserve == 0xDEADBEEF) {
							// EXL, 25-Dec-2024: Separate data constants from function addresses.
							if (B32(ldrSymTable[j].st_value) > DATA_SHIFT_OFFSET) {
								*((UINT32 *) (physBase + elfSymTable[i].st_value - virtBase)) =
									ldrSymTable[j].st_value - B32(DATA_SHIFT_OFFSET);
							} else {
								*((UINT32 *) (physBase + elfSymTable[i].st_value - virtBase + 0x0C)) =
									ldrSymTable[j].st_value;
							}
						}
					}
				}
			}
		}

		// EXL, 25-Dec-2024: Free Symbol and String tables and close ELF file.
		suFreeMem(elfStrTable);
		suFreeMem(elfSymTable);
	} else {
		// EXL, 01-Jan-2025: DT_JMPREL symbols handling for ELFs generated by GCC. The GCC uses
		//   DT_JMPREL + DT_SYMTAB/DT_STRTAB section instead of SHT_SYMTAB/SHT_STRTAB sections in ELFs generated by ADS.
		Elf32_Addr elfStrTableAddr;
		Elf32_Addr *pltJumpTblPtr;

		elfSymTable = (Elf32_Sym *) (physBase + dynTags[DT_SYMTAB] - virtBase);
		relTable = (Elf32_Rel *) (physBase + dynTags[DT_JMPREL] - virtBase);
		elfStrTableAddr = physBase + dynTags[DT_STRTAB] - virtBase;
		// EXL, 13-Apr-2025: This is a pointer to the first jump in the PLT jump table:
		// E5 2D E0 04 E5 9F E0 04 E0 8F E0 0E E5 BE F0 08 00 00 00 D0 47 78 E7 FD E2 8F C6 00 E2 8C CA 00 E5 BC F0 C0
		// |    0     |     1     |     2     |     3     |     4     |  5  |  5  |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		pltJumpTblPtr = (
			(Elf32_Addr *) (
				physBase + B32(*((Elf32_Word *) (physBase + B32(relTable[0].r_offset) - virtBase))) - virtBase
			)
		);
		// EXL, 23-Apr-2025: It has 4778E7FD opcode in Thumb after 4 .plt bytes? If yes, just skip it.
		pltJumpTblPtr += (B32(*(pltJumpTblPtr + 5)) == 0x4778E7FD) ? 6 : 5;

		// EXL, 25-Dec-2024: Iterate over "R_ARM_JUMP_SLOT" relocations in '.rel.plt' section.
		for (i = 0; i * sizeof(Elf32_Rel) < dynTags[DT_PLTRELSZ]; ++i) {
			INT32 sym_idx = ELF32_R_SYM(B32(relTable[i].r_info));
			char *sym_str = (char *) (elfStrTableAddr + B32(elfSymTable[sym_idx].st_name));
			Elf32_Sym *sym = &elfSymTable[sym_idx];
			INT32 func_bind = ELF32_ST_BIND(sym->st_info);

			UtilLogStringData("Req Func: %d, %s\n", func_bind, sym_str);

			// EXL, 25-Dec-2024: Iterate over library entities.
			for (j = 0; j < ldrNumSymbols; j++) {
				UINT32 ldr_st_name = B32(ldrSymTable[j].st_name);
				UINT32 ldr_st_addr = B32(ldrSymTable[j].st_value);
				if (namecmp(sym_str, &ldrStrTable[ldr_st_name]) == TRUE) {
					UtilLogStringData(
						"API Call #%d:\n  addr 0x%lX\n  old 0x%X\n  new 0x%X\n",
						i, B32(elfSymTable[i].st_value),
						B32(*((Elf32_Word *) (physBase + B32(relTable[i].r_offset) - virtBase))),
						ldr_st_addr
					);

					// EXL, 13-Apr-2024: We need to patch the PLT jump table at runtime in the ADS way.
					//   This will resolve the launch issues on real ARMv4T, ARM7TDMI-S, and Big-Endian CPUs/MCUs.
					// E2 8F C6 00 E2 8C CA 00 E5 BC F0 C0 => E5 9F C0 00 E1 2F FF 1C 10 00 00 00 (function address)
					{
						Elf32_Word ARM_ADRL_1;
						Elf32_Word ARM_ADRL_2;
						Elf32_Word ARM_LDR;

						// EXL, 23-Apr-2025: It has 4778E7FD opcode in Thumb? If yes, just skip it.
						if (B32(*(pltJumpTblPtr)) == 0x4778E7FD) {
							pltJumpTblPtr += 1;
						}

						ARM_ADRL_1 = B32(*(pltJumpTblPtr+0));
						ARM_ADRL_2 = B32(*(pltJumpTblPtr+1));
						ARM_LDR    = B32(*(pltJumpTblPtr+2));

						// add ip, pc, #0, #12    =>   ldr ip, [pc]
						// add ip, ip, #0, #20    =>   bx ip
						// ldr pc, [ip, #0xc0]!   =>   10 00 00 00 (function address)

						*(pltJumpTblPtr + 0) = B32(0xE59FC000);
						*(pltJumpTblPtr + 1) = B32(0xE12FFF1C);
						*(pltJumpTblPtr + 2) = B32(ldr_st_addr);

						UtilLogStringData(
							"=> PLT PATCH: %08X: %08X %08X %08X => %08X %08X %08X\n",
							pltJumpTblPtr,
							ARM_ADRL_1, ARM_ADRL_2, ARM_LDR,
							B32(*(pltJumpTblPtr+0)), B32(*(pltJumpTblPtr+1)), B32(*(pltJumpTblPtr+2))
						);

						pltJumpTblPtr += 3;
					}

					if (reserve == 0xDEADBEEF) {
						*((Elf32_Word *) (physBase + B32(relTable[i].r_offset) - virtBase)) = B32(ldr_st_addr);
					}
				}
			}
		}
	}

#if !defined(SIMULA)
	DL_FsCloseFile(file);
#else
	fclose(file);
#endif

	UtilLogStringData(" Starting ELF at 0x%X with reserve = 0x%X", physBase + elfHeader.e_entry - virtBase, reserve);

	// EXL, 25-Dec-2024: Start ELF from the "e_entry" address, "start" stub which call "Register" ELF entry point.
#if !defined(SIMULA)
	((Entry) (physBase + elfHeader.e_entry - virtBase))(file_uri, params, reserve);
#else
	file = fopen("ELF_MEMORY_DUMP.bin", "wb");
	if (file) {
		fwrite((UINT32 *) (physBase), upperAddr - virtBase, 1, file);
		fclose(file);
	}
#endif

	UtilLogStringData(" ELF returned\n");

	return ELDR_SUCCESS;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Simula: The ELF Loader Simulator, EXL, 06-Jan-2025\n\n");
		fprintf(stderr, "Usage (use '-r' for relocations):\n\n");
		fprintf(stderr, "\t./simula elfloader.lib HelloMoto.elf\n");
		fprintf(stderr, "\t./simula elfloader.lib HelloMoto.elf -r\n");
		fprintf(stderr, "\n");
		return 1;
	}

	FILE *lib = fopen(argv[1], "rb");
	if (!lib) {
		fprintf(stderr, "Cannot find library '%s' file.\n", argv[1]);
		return 2;
	}
	fseek(lib, 0, SEEK_END);
	UINT32 lib_size = ftell(lib);
	fseek(lib, 0, SEEK_SET);
	UINT8 *library_data = (UINT8 *) malloc(lib_size);
	fread(library_data, lib_size, 1, lib);
	fclose(lib);

	IRAM_ELF_T iram_elf;
	iram_elf.iram_mem = NULL;
	iram_elf.eram_mem = NULL;
	iram_elf.size_mem = 0;

	return loadELF(argv[2], NULL, (void *) library_data, argv[3] ? 0xDEADBEEF : 0, &iram_elf);
}
