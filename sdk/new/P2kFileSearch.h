/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SEARCH_H
#define P2K_SEARCH_H

#include "P2kT.h"
#include "P2kDeviceLayerT.h"
#include "P2kFileSearchT.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************
  Поиск файлов
 **************************/

#define DIRECTORY_FILTER_ATTRIBUTE 0x12
#define EV_FILE_SEARCH_COMPLETED   0x8213D

typedef WCHAR *(*FS_URI_FNCT_PTR)(UINT16, WCHAR *);

typedef struct {
	FS_SEARCH_HANDLE_T search_handle;
	FS_SEARCH_NUM_T search_total;
	FS_SEARCH_RESULT_T search_result;
} FS_SEARCH_COMPLETED_INDEX_T;

typedef struct {
	FS_SEARCH_PARAMS_T search;
	FS_SEARCH_NUM_T num;
	WCHAR search_string[FS_MAX_URI_NAME_LENGTH + 1];
	UINT16 file_id[1500 + 50 + 43];
	UINT16 vol_attr;
	FS_SEARCH_HANDLE_T shandle;
	UINT16 owner_id;
	BOOL abort_pending;
} FS_SEARCH_INFO_T;

/* Функция синхронного поиска файлов */
/* search_string формируется из uri папки, где ведётся поиск, затем разделитель 0xFFFE, затем паттерны.
    Например: "file://b/Elf/\xFFFE*.elf" */
UINT16 DL_FsSSearch(
	FS_SEARCH_PARAMS_T params,
	const WCHAR *search_string,
	FS_SEARCH_HANDLE_T *handle,  // out
	UINT16 *res_count,           // out
	UINT16 owner
);

/* Альтернативная функция синхронного поиска файлов */
UINT16 DL_FsSearch(
	const FS_SEARCH_PARAMS_T search,
	const WCHAR *match_names,
	FS_SEARCH_INFO_T **search_info,
	FS_URI_FNCT_PTR *fnct_ptr,
	const UINT16 owner_id
);

/* Функция асинхронного поиска файлов. Результаты поиска ловить по ивенту EV_FILE_SEARCH_COMPLETED */
UINT16 DL_FsISearch(
	const IFACE_DATA_T *iface,
	const FS_SEARCH_PARAMS_T params,
	const WCHAR *match_names,
	const UINT16 owner_id
);

/* Функция для получения [части] списка результатов поиска */
UINT16 DL_FsSearchResults(
	FS_SEARCH_HANDLE_T handle,
	UINT16 start_index,  // 0 based
	UINT16 *count,       // in and out
	FS_SEARCH_RESULT_T *results
);

UINT16 DL_FsSearchClose(FS_SEARCH_HANDLE_T handle);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SEARCH_H */

/** @} */ /* end of P2K_Types */
