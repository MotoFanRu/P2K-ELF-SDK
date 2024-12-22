/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   Entry point and parsing "auto.run" file.
 *
 * Author:
 *   Andy51, 28-Oct-2007
 *
 * Autorun format:
 *   ; This is comment\r\n
 *   <empty string>\r\n
 *   file://a/Elf/Some.elf\r\n
 *   file://a/Elf/Some.elf 0x1234\r\n
 */

#include "AutoRun.h"

#include <loader1.h>
#include <filesystem.h>
#include <utilities.h>
#include <mem.h>

const W_CHAR autorun_uri[] = L"file://b/Elf/auto.run";

// EXL, 22-Dec-2024: This pragma is needed for proper sorting.
#pragma arm section code = "Header"
void AutorunMain(void) {
	FS_HANDLE_T file;
	FS_COUNT_T count;
	UINT32 filesize;
	char *buffer;
	UINT32 i;
	UINT32 result;
	W_CHAR uri[WCHAR_PARAMS_MAX];
	W_CHAR params[WCHAR_PARAMS_MAX];

	i = 0;
	result = PARS_SKIP;

	// Andy51, 28-Oct-2007: Placeholder for replaced RegFn.
	// EXL, 22-Dec-2024: We need to call this because we want the SyncML application to work.
	APP_SyncML_MainRegister();

	// EXL, 22-Dec-2024: Start ELF Loader application.
	ElfLoaderStart();

	// EXL, 22-Dec-2024: Routines of open and read "auto.run" file.
	file = DL_FsOpenFile((const WCHAR *) autorun_uri, FILE_READ_MODE, 0);
	if (file == FS_HANDLE_INVALID) {
		return;
	}
	filesize = DL_FsGetFileSize(file);
	if (filesize == 0) {
		return;
	}
	buffer = (char *) suAllocMem(filesize + 3, NULL);
	DL_FsReadFile(buffer, filesize, sizeof(UINT8), file, &count);
	DL_FsCloseFile(file);

	// EXL, 22-Dec-2024: Insert new line symbol to the end of file if not present.
	if (buffer[filesize - 1] != '\n') {
		buffer[filesize++] = '\r';
		buffer[filesize++] = '\n';
	}
	// EXL, 22-Dec-2024: End of file marker.
	buffer[filesize] = (char) 0xFF;

	// EXL, 22-Dec-2024: Parse "auto.run" file and run ELFs from it.
	do {
		result = ParseString(&buffer[i], &i, uri, params);
		if (result == PARS_DONE) {
			UtilLogStringData(" Load ELF");
			LdrLoadELF((WCHAR *) uri, (WCHAR *) params);
		}
	} while (result != PARS_EOF);

	suFreeMem(buffer);
}

// EXL, 22-Dec-2024: Skip line from parsing and go to the next line (comments, etc).
void SkipLine(char *buffer, UINT32 *p_index) {
	UINT32 i;

	for (i = 0; buffer[i] != '\n'; i++);

	*p_index += i + 1;
}

// EXL, 22-Dec-2024: Parse one line from "auto.run" file.
UINT32 ParseString(char *buffer, UINT32 *p_index, W_CHAR *uri, W_CHAR *params) {
	UINT32 i;
	UINT32 j;

	i = 0;
	j = 0;

	// EXL, 22-Dec-2024: Skip empty "auto.run" file.
	if (buffer[0] == (char) 0xFF) {
		return PARS_EOF;
	}

	// EXL, 22-Dec-2024: Skip white spaces.
	for (i = 0; buffer[i] == ' '; i++);

	// EXL, 22-Dec-2024: Skip comments and empty strings.
	if ((buffer[i] == ';') || (buffer[i] == '\r')) {
		SkipLine(buffer, p_index);
		return PARS_SKIP;
	}

	// Andy51, 28-Oct-2007: Get ELF uri.
	j = i;
	for (j = i; ((buffer[i] != ' ') && (buffer[i] != '\r')); i++);
	buffer[i++] = 0;
	u_atou((const char *) &buffer[j], (WCHAR *) uri);

	// EXL, 22-Dec-2024: End of line handle. No ELF parameters.
	if (buffer[i] == '\n') {
		*p_index += i + 1;
		params[0] = '\0';
		return PARS_DONE;
	}

	// Andy51, 28-Oct-2007: Get ELF params.
	// EXL, 22-Dec-2024: Skip white spaces again.
	for (; buffer[i] == ' '; i++);

	// EXL, 22-Dec-2024: Go to the line with something useful.
	j = i;
	for (; buffer[i] != '\r'; i++);

	// EXL, 22-Dec-2024: Copy ELF params to params.
	buffer[i] = 0;
	u_atou((const char *) &buffer[j], (WCHAR *) params);
	*p_index += i + 2;

	return PARS_DONE;
}
