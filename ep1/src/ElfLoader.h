/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   ELF Loader implementation for Motorola P2K platform and auxiliary functions.
 *
 * Author:
 *   Andy51, 31-Oct-2007
 */

#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include "elf.h"

#define EVCODE_RESERVE                 (0x40)
#define EVCODE_BASE                    (0xA000)
#define EVCODE_STARTLDR                (EVCODE_BASE)
#define EVCODE_LOADELF                 (EVCODE_BASE + 1)
#define EVCODE_UNLOADELF               (EVCODE_BASE + 2)

typedef struct {
	UINT32 st_name;
	UINT32 st_value;
} Ldr_Sym;

#define MAX_PROG_HEADERS               (8)

enum {
	ELDR_SUCCESS = 0,
	ELDR_OPEN_FAILED,
	ELDR_READ_HEADER_FAILED,
	ELDR_READ_FAILED,
	ELDR_SEEK_FAILED
};

extern UINT32 loadELF(char *file_uri, char *params, void *Library, UINT32 reserve);

#endif /* ELF_LOADER_H */
