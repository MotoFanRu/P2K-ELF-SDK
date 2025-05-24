/*
 * Project:
 *   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
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

// EXL, 06-Jan-2025: The order is important here. Be careful when editing.
#if defined(EA1)
#define DATA_SHIFT_OFFSET              (0xC0000000)
#elif defined(EP1) || defined(EG1)
#define DATA_SHIFT_OFFSET              (0x30000000)
#else
#error "Unknown ElfPack flavor! Try to set EP1, EG1, or EA1"
#endif

#define ELF_EP1_ADS                    (0x00)
#define ELF_EP1_GCC                    (0x01)
#define ELF_EP1_IRAM_SEG               (0x02)

#define EVCODE_RESERVE                 (0x40)
#define EVCODE_BASE                    (0xA000)
#define EVCODE_STARTLDR                (EVCODE_BASE)
#define EVCODE_LOADELF                 (EVCODE_BASE + 1)
#define EVCODE_UNLOADELF               (EVCODE_BASE + 2)

typedef struct {
	UINT32 st_name;
	UINT32 st_value;
} Ldr_Sym;

enum {
	ELDR_SUCCESS = 0,
	ELDR_OPEN_FAILED,
	ELDR_READ_HEADER_FAILED,
	ELDR_READ_FAILED,
	ELDR_SEEK_FAILED
};

typedef struct {
	UINT32 *iram_mem;
	UINT32 *eram_mem;
	UINT32  size_mem;
	BOOL    iram_seg;
} IRAM_ELF_T;

extern const char n_phone[];
extern const char n_platform[];
extern const char n_majorfw[];
extern const char n_minorfw[];

typedef UINT32 (*Entry)(char *, char *, UINT32);

extern UINT32 loadELF(char *file_uri, char *params, void *Library, UINT32 reserve, IRAM_ELF_T *iram_elf);

#endif /* ELF_LOADER_H */
