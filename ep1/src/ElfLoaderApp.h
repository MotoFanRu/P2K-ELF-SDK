/*
 * Project:
 *   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
 *
 * About:
 *   P2K wrapper for the ELF Loader application.
 *
 * Author:
 *   Andy51, 26-Aug-2007
 */

#ifndef ELF_LOADER_APP_H
#define ELF_LOADER_APP_H

#include "ElfLoaderApp.h"

#include <apps.h>

#define MAX_RUNNING_ELFS               (16)

typedef enum {
	STATE_ANY,
	STATE_MAX
} STATE_T;

typedef struct {
	APPLICATION_T apd;
	UINT32 reserve;
	void *Library;
	IRAM_ELF_T iram_elf;
	UINT16 running_elfs_event_ids[MAX_RUNNING_ELFS];
	UINT32 running_elfs_img_addrs[MAX_RUNNING_ELFS];
	UINT16 current_elf_index;
} ELFLOADER_INSTANCE_DATA_T;

extern void ElfLoaderStart(void);

extern UINT32 LoadLibrary(ELFLOADER_INSTANCE_DATA_T *p_app_data);

#endif /* ELF_LOADER_APP_H */
