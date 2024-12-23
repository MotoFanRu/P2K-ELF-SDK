/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   ELF Loader implementation for Motorola P2K platform and auxiliary functions.
 *
 * Author:
 *   Andy51, 01-Nov-2007
 */

#include "ElfLoader.h"

#include <loader1.h>
#include <events.h>
#include <filesystem.h>
#include <utilities.h>
#include <mem.h>

typedef UINT32 (*Entry)(char *, char *, UINT32);

extern const char n_phone[];
extern const char n_platform[];
extern const char n_majorfw[];
extern const char n_minorfw[];

// TODO delete this.
#define AFW_AllocateMemory(x) suAllocMem(x, NULL)
#define AFW_FreeAllocatedMemory suFreeMem

UINT32 loadELF(char *file_uri, char *params, void *Library, UINT32 reserve) {
	UINT32          i;
	UINT32          j;

	FS_HANDLE_T     file;
	FS_COUNT_T      read;

	Elf32_Ehdr      elfHeader;
	Elf32_Phdr      elfProgramHeaders[NUM_HEADERS];

	Elf32_Addr      virtBase;           // = NULL;
	Elf32_Addr      physBase;           // = NULL;
	Elf32_Addr      upperAddr;          // = NULL;
	Elf32_Addr      dynSegment;         // = NULL;

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
	W_CHAR          wstr[WCHAR_PARAMS_MAX];

	virtBase        = NULL;
	physBase        = NULL;
	upperAddr       = NULL;
	dynSegment      = NULL;

	sumMem          = 0;
	sumSize         = 0;

	elfNumSymbols   = 0;
	elfStrTableSize = 0;

	elfSymTable     = NULL;
	elfStrTable     = NULL;

	UtilLogStringData("ElfLdr Load Request %s", file_uri);

	// EXL, 23-Dec-2024: Copy ELF uri to WCHAR string then open ELF file.
	u_atou((const char *) file_uri, wstr);

	file = DL_FsOpenFile((WCHAR *) wstr, FILE_READ_MODE, 0);
	if (file == FS_HANDLE_INVALID) {
		return ELDR_OPEN_FAILED;
	}

	// EXL, 23-Dec-2024: First blood, read ELF header!
	if (DL_FsReadFile((void *) &elfHeader, sizeof(Elf32_Ehdr), 1, file, &read) != RESULT_OK) {
		return ELDR_READ_HEADER_FAILED;
	}

	UtilLogStringData(
		"Elf is loading...\n\nELF header:\n  e_entry  0x%X\n  e_phoff  0x%X\n  e_phnum  %d\n\n",
		elfHeader.e_entry,
		elfHeader.e_phoff,
		elfHeader.e_phnum
	);

	// EXL, 24-Dec-2024: ~0 is 0xFFFFFFFF.
	upperAddr =  0;
	virtBase  = ~0;

	// EXL, 24-Dec-2024: Read the program header tables and determine how many RAM they eats.
	for (i = 0; i < elfHeader.e_phnum; i++) {
		// EXL, 24-Dec-2024: Seek to the program header tables and read them.
		if (DL_FsFSeekFile(file, elfHeader.e_phoff + (i * elfHeader.e_phentsize), SEEK_WHENCE_SET) != RESULT_OK) {
			return ELDR_SEEK_FAILED;
		}

		if (DL_FsReadFile((void *) &elfProgramHeaders[i], sizeof(Elf32_Phdr), 1, file, &read) != RESULT_OK) {
			return ELDR_READ_FAILED;
		}

		UtilLogStringData(
			"Segment #%d:\n  p_type  %d\n  p_vaddr  0x%X\n  p_memsz  0x%X\n",
			i,
			elfProgramHeaders[i].p_type,
			elfProgramHeaders[i].p_vaddr,
			elfProgramHeaders[i].p_memsz
		);

		// EXL, 24-Dec-2024: Set a lower [<======>] upper memory addresses for segments.
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

	// EXL, 24-Dec-2024: Allocate RAM memory for sections then clear it.
	physBase = (Elf32_Addr) suAllocMem(upperAddr - virtBase, NULL);
	memclr((void *) physBase, upperAddr - virtBase);

	UtilLogStringData(
		"\nFinished analysis:\n  Virtual Base  0x%X\n  Mem Needed  0x%X\n  Upper Address  0x%X\n  "
		"Summary Mem  0x%X\n  Summary Size  0x%X\n  Physical Base  0x%X\n\n",
		virtBase,
		upperAddr - virtBase,
		upperAddr,
		sumMem,
		sumSize,
		physBase
	);

	// EXL, 24-Dec-2024: Read and deploy segments to RAM.
	for (i = 0; i < elfHeader.e_phnum; i++) {
		switch(elfProgramHeaders[i].p_type) {
			case PT_LOAD:
				// EXL, 24-Dec-2024: Read program segments with executable code to RAM.
				if (DL_FsFSeekFile(file, elfProgramHeaders[i].p_offset, SEEK_WHENCE_SET) != RESULT_OK) {
					suFreeMem((void *) physBase);
					return ELDR_SEEK_FAILED;
				}

				if (
					DL_FsReadFile(
						(void *) (physBase + elfProgramHeaders[i].p_vaddr - virtBase),
						elfProgramHeaders[i].p_filesz, 1, file, &read
					) != RESULT_OK
				) {
					suFreeMem((void *) physBase);
					return ELDR_READ_FAILED;
				}

				UtilLogStringData(
					"  Segment #%d loading  0x%X  0x%d\n\n",
					i,
					physBase + elfProgramHeaders[i].p_vaddr - virtBase,
					elfProgramHeaders[i].p_filesz
				);
				break;
			case PT_DYNAMIC:
				// EXL, 24-Dec-2024: Read dynamic segment with tag functions and addresses.
				dynSegment = (Elf32_Addr) suAllocMem(elfProgramHeaders[i].p_filesz, NULL);

				if (DL_FsFSeekFile(file, elfProgramHeaders[i].p_offset, SEEK_WHENCE_SET) != RESULT_OK) {
					suFreeMem((void *) physBase);
					suFreeMem((void *) dynSegment);
					return ELDR_SEEK_FAILED;
				}

				if (DL_FsReadFile((void *) dynSegment, elfProgramHeaders[i].p_filesz, 1, file, &read) != RESULT_OK) {
					suFreeMem((void *) physBase);
					suFreeMem((void *) dynSegment);
					return ELDR_READ_FAILED;
				}

				j = 0;

				// Andy51, 01-Nov-2007: Load tags from dynamic segment.
				// EXL, 24-Dec-2024: Only part of table until 00 00 00 00 as d_tag. 00 00 00 00 00 00 00 00 is divider?
				do {
					tag = ((Elf32_Dyn *) dynSegment)[j++];
					if (tag.d_tag <= DT_BIND_NOW) {
						dynTags[tag.d_tag] = tag.d_val;
					}
				} while (tag.d_tag);

				UtilLogStringData(
					"Relocation start\n DT_REL  0x%X\n DT_RELSZ  %d\n", dynTags[DT_REL], dynTags[DT_RELSZ]
				);

				relTable = (Elf32_Rel *) (dynSegment + dynTags[DT_REL] - elfProgramHeaders[i].p_vaddr);

				// EXL, 24-Dec-2024: Read dynamic segment with tag functions and addresses.
				for (j = 0; j * sizeof(Elf32_Rel) < dynTags[DT_RELSZ]; j++) {
					// EXL, 24-Dec-2024: Only last byte of UINT32 r_info is relType.
					relType = ELF32_R_TYPE(relTable[j].r_info);

					UtilLogStringData(" Reloc #%d\n  Type  %d\n  Off  0x%X\n", j, relType, relTable[j].r_offset);

					if (relType == R_ARM_RABS32) {
						UtilLogStringData(
							" R_ARM_RABS32\n  Old  0x%X\n  New  0x%X\n",
							*((UINT32 *) (physBase + relTable[j].r_offset - virtBase)),
							*((UINT32 *) (physBase + relTable[j].r_offset - virtBase)) + physBase - virtBase);

						*((UINT32 *) (physBase + relTable[j].r_offset - virtBase)) += physBase - virtBase;
					}
				}
				break;
		}
	}

	if (dynSegment != NULL)	{
		suFreeMem((void *) dynSegment);
	}

	// Правка API вызовов

	/*/////WIN32 Chunk/////////
	FILE		*flib;
	flib = fopen("library.bin", "rb");
	fread(&ldrNumSymbols, sizeof(int), 1, flib);
	fread(&ldrStrTableSize, sizeof(int), 1, flib);

	ldrSymTable = (Ldr_Sym*)malloc(sizeof(Ldr_Sym)*ldrNumSymbols);
	fread(ldrSymTable, sizeof(Ldr_Sym), ldrNumSymbols, flib);

	ldrStrTable = (char*)malloc(sizeof(char)*ldrStrTableSize);
	fread(ldrStrTable, sizeof(char), ldrStrTableSize, flib);
	fclose(flib);
*//////////////////////////

	ldrNumSymbols = ((UINT32*)Library)[0];
	//ldrStrTableSize = ((UINT32*)Library)[1];
	ldrSymTable = (Ldr_Sym*)&((UINT32*)Library)[1];
	ldrStrTable = (char*)&ldrSymTable[ldrNumSymbols];

	///////////////////////////


	// Загружаем elfSymTable и elfStrTable
	for( i=0; i<elfHeader.e_shnum; i++ )
	{
		if( DL_FsFSeekFile(file, elfHeader.e_shoff + i*elfHeader.e_shentsize, SEEK_WHENCE_SET) != RESULT_OK)
		{
			AFW_FreeAllocatedMemory((void*)physBase);
			return ELDR_SEEK_FAILED;
		}
		if( DL_FsReadFile((void*)&elfSectionHeader, sizeof(Elf32_Shdr), 1, file, &read) != RESULT_OK )
		{
			AFW_FreeAllocatedMemory((void*)physBase);
			return ELDR_READ_FAILED;
		}

		UtilLogStringData("Section #%d:\n  sh_type %d\n  sh_offset 0x%X\n  sh_size  0x%X\n",
						  i,
						  elfSectionHeader.sh_type,
						  elfSectionHeader.sh_offset,
						  elfSectionHeader.sh_size );

#define SHT_SYMTAB 2 //The section holds a symbol table.
#define SHT_STRTAB 3 //The section holds a string table.
		if(elfSectionHeader.sh_type == SHT_SYMTAB)
		{
			elfSymTable = (Elf32_Sym*)AFW_AllocateMemory(elfSectionHeader.sh_size);
			if( DL_FsFSeekFile(file, elfSectionHeader.sh_offset, SEEK_WHENCE_SET) != RESULT_OK )
			{
				AFW_FreeAllocatedMemory((void*)physBase);
				if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
				if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
				return ELDR_SEEK_FAILED;
			}
			if( DL_FsReadFile(elfSymTable, elfSectionHeader.sh_size, 1, file, &read) != RESULT_OK )
			{
				AFW_FreeAllocatedMemory((void*)physBase);
				if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
				if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
				return ELDR_READ_FAILED;
			}

			elfNumSymbols = elfSectionHeader.sh_size >> 4; // / sizeof(Elf32_Sym);
		}
		else if( (elfSectionHeader.sh_type == SHT_STRTAB) && (i != elfHeader.e_shstrndx) )
		{
			elfStrTable = (char*)AFW_AllocateMemory(elfSectionHeader.sh_size);
			if( DL_FsFSeekFile(file, elfSectionHeader.sh_offset, SEEK_WHENCE_SET) != RESULT_OK )
			{
				AFW_FreeAllocatedMemory((void*)physBase);
				if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
				if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
				return ELDR_SEEK_FAILED;
			}
			if( DL_FsReadFile(elfStrTable, elfSectionHeader.sh_size, 1, file, &read) != RESULT_OK )
			{
				AFW_FreeAllocatedMemory((void*)physBase);
				if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
				if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
				return ELDR_READ_FAILED;
			}

			elfStrTableSize = elfSectionHeader.sh_size;
		}
	}

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

#define STT_OBJECT 1
#define STT_FUNC 2


	UtilLogStringData("API Calls Fix Start:  ldrStrTable 0x%X  ldrSymTable 0x%X  ldrNumSymbols %d",
					  ldrStrTable,
					  ldrSymTable,
					  ldrNumSymbols);

	// Ищем совпадения и подменяем адреса
	for( i=0; i<elfNumSymbols; i++ )
	{
		if( (ELF32_ST_BIND(elfSymTable[i].st_info)==STB_GLOBAL) &&
			( (ELF32_ST_TYPE(elfSymTable[i].st_info)==STT_FUNC) ||
			 ( (ELF32_ST_TYPE(elfSymTable[i].st_info)==STT_OBJECT) && (elfSymTable[i].st_size==0) ) ) )
		{
			UtilLogStringData("Possible entry:  i %d  st_name %d  name %s", i, elfSymTable[i].st_name, &elfStrTable[elfSymTable[i].st_name]);
			for(j=0; j<ldrNumSymbols; j++ )
			{
				//UtilLogStringData("FOR:  j %d st_name", j, ldrSymTable[j].st_name);
				if(namecmp(&elfStrTable[elfSymTable[i].st_name], &ldrStrTable[ldrSymTable[j].st_name]) == 1)
				{
					UtilLogStringData("API Call #%d:\n  addr 0x%X\n  old 0x%X\n  new 0x%X\n",
									  i,
									  elfSymTable[i].st_value,
									  *((UINT32*)(physBase + elfSymTable[i].st_value - virtBase)),
									  ldrSymTable[j].st_value );

					if(ldrSymTable[j].st_value>0x30000000) *((UINT32*)(physBase + elfSymTable[i].st_value - virtBase)) = ldrSymTable[j].st_value-0x30000000;
					else *((UINT32*)(physBase + elfSymTable[i].st_value - virtBase + 0xC)) = ldrSymTable[j].st_value;
				}
			}
		}
	}

	AFW_FreeAllocatedMemory(elfStrTable);
	AFW_FreeAllocatedMemory(elfSymTable);
	DL_FsCloseFile(file);

	/*/////WIN32 Chunk/////////
    f = fopen("loaded.bin", "wb");
    fwrite((void*)physBase, upperAddr-virtBase, 1, f);
    fclose(f);

    free((void*)physBase);
*/////////////////////////
	// START IT!

	UtilLogStringData(" Starting ELF at 0x%X with reserve = 0x%X", physBase + elfHeader.e_entry - virtBase, reserve);
	((Entry)(physBase + elfHeader.e_entry - virtBase))( file_uri, params, reserve );

	/*if( ((Entry)(physBase + elfHeader.e_entry - virtBase))(file_uri, *reserve, param) == 1)
    { //Start immediately
        AFW_AddEvNoD(p_evg, *evcode_base);
    }*/
	UtilLogStringData(" ELF returned");

	return ELDR_SUCCESS;
}

// EXL, 23-Dec-2024: Immediately start ELF registered for event, this will call ApplicationStart() function of ELF.
UINT32 LdrStartApp(EVENT_CODE_T ev_start) {
	UINT32 status;

	status = RESULT_OK;

	status = AFW_CreateInternalQueuedEvAux(ev_start, FBF_LEAVE, 0, NULL);

	UtilLogStringData(" LdrStartApp 0x%X  status = %d", ev_start, status);

	return status;
}

// EXL, 23-Dec-2024: Create an "EVCODE_LOADELF" event and send it to ElfLoaderApp.c, see Handle_LoadELF() function.
UINT32 LdrLoadELF(W_CHAR *uri, W_CHAR *params) {
	UINT32 status;
	ADD_EVENT_DATA_T ev_data;

	status = RESULT_OK;

	// EXL, 23-Dec-2024: Copy ELF uri and params to the event data, both is 64 symbols length max.
	u_utoa(uri, ev_data.data.start_params.uri);
	if (params != NULL) {
		u_utoa(params, ev_data.data.start_params.params);
	} else {
		// EXL, 23-Dec-2024: Set 0 if no params.
		ev_data.data.start_params.params[0] = 0;
	}

	ev_data.data_tag = 0;

	UtilLogStringData(
		"LdrLoadELF uri: %s  params: %s",
		ev_data.data.start_params.uri,
		ev_data.data.start_params.params
	);

	status = AFW_CreateInternalQueuedEvAuxD(EVCODE_LOADELF, &ev_data, FBF_INVALID, 0, NULL);

	return status;
}

// EXL, 23-Dec-2024: Create an "EVCODE_UNLOADELF" event and send it to ElfLoaderApp.c, see Handle_UnloadELF() function.
UINT32 LdrUnloadELF(void *elf_ptr) {
	UINT32 status;
	ADD_EVENT_DATA_T ev_data;

	UtilLogStringData(" LdrUnloadELF 0x%X  0x%X", elf_ptr);

	status = RESULT_OK;
	ev_data.data_tag = 0;

	// EXL, 23-Dec-2024: Set ELF pointer address value to the short event data.
	*((UINT32 *) ev_data.data.pad) = (UINT32) elf_ptr;

	UtilLogStringData(" LdrUnloadELF send");

	status = AFW_CreateInternalQueuedEvAuxD(EVCODE_UNLOADELF, &ev_data, FBF_LEAVE, 0, NULL);

	return status;
}

// EXL, 23-Dec-2024: Compare two ANSI strings.
UINT32 namecmp(const char *ansi_str_1, const char *ansi_str_2) {
	UINT32 i;

	i = 0;

	while (ansi_str_1[i] == ansi_str_2[i]) {
		if(ansi_str_1[i++] == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

// EXL, 23-Dec-2024: Convert UTF-16BE string to ANSI string.
char *u_utoa(const W_CHAR *from_utf16be_str, char *to_ansi_str) {
	UINT32 i;

	i = 0;

	// EXL, 23-Dec-2024: Get only first byte of UTF16-BE string.
	do {
		to_ansi_str[i] = (char) from_utf16be_str[i];
	} while (from_utf16be_str[i++] != 0);

	return to_ansi_str;
}

// EXL, 23-Dec-2024: Send log to P2K Data Logger utility.
void UtilLogStringData(const char*  format, ...) {
	char buffer[255];
	va_list vars;

	va_start(vars, format);
	vsprintf(buffer, format, vars);
	va_end(vars);

	// TODO:???
	// EXL, 23-Dec-2024: 0x5151 is????
	suLogData(0, 0x5151, 1, strlen(buffer) + 1, buffer);
}

// EXL, 23-Dec-2024: ELF Loader API functions, proper values in machine generated "SysInfo.c" file.
//   Example of "SysInfo.c" file:
//     const char n_phone[3]    = "E1";
//     const char n_platform[4] = "LTE";
//     const char n_majorfw[13] = "R373_G_0E.30";
//     const char n_minorfw[4]  = "49R";
const char *LdrGetPhoneName(void) {
	return n_phone;
}

const char *LdrGetPlatformName(void) {
	return n_platform;
}

const char *LdrGetFirmwareMajorVersion(void) {
	return n_majorfw;
}

const char *LdrGetFirmwareMinorVersion(void) {
	return n_minorfw;
}
