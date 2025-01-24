/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_SEARCH_T_H
#define P2K_SDK_SEARCH_T_H

#include "P2kT.h"
#include "P2kFileSystemT.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT16 FS_SEARCH_NUM_T; /* Number of files matching search criteria */

/* search flags */
#define FS_SEARCH_EXCLUDE                      0x80  // (?)
#define FS_SEARCH_COMBINED                     0x40  // (?) возможно поиск в нескольких местах сразу
#define FS_SEARCH_SORT_CHRONOLOGICAL           0x20  // сортировка по времени (создания/редактирования?)
#define FS_SEARCH_DIR_LISTING                  0x10  // (?) возможно выдаст список папок
#define FS_SEARCH_PATH_START                   0x08  // имена файлов в результате будут содержать полный путь
#define FS_SEARCH_SORT_ALPHANUMERIC            0x04  // сортировка по алфавиту
#define FS_SEARCH_OWNER_MATCH                  0x02  // (?) проверять владельца
#define FS_SEARCH_RECURSIVE                    0x01  // рекурсивный поиск (будет искать в подпапках)
// для удобства
#define FS_SEARCH_START_PATH                   0x08
#define FS_SEARCH_FOLDERS                      0x10
#define FS_SEARCH_SORT_BY_NAME                 0x04
#define FS_SEARCH_RECURSIVE_AND_SORT_BY_NAME   0x0D
#define FS_SEARCH_RECURSIVE_AND_SORT_BY_TIME   0x29
#define FS_SEARCH_SORT_BY_DATE               0x20

/* спец-символы в запросе для поиска */
#define FS_VOLUME_NULL                         0x0000
#define FS_VOLUME_SEPARATOR                    0xFFFE

#define FS_MATCH_NULL                          0x0000
#define FS_MATCH_SEPARATOR                     0xFFFE
#define FS_MATCH_PATH_SEPARATOR                0xFFFD
#define FS_MATCH_PATH_NONREC_SEPARATOR         0xFFFC
#define FS_MATCH_PATH_EXCLUDE_SEPARATOR        0xFFFB
#define FS_MATCH_PATH_NONREC_EXCLUDE_SEPARATOR 0xFFFA

typedef struct {
#if defined(EM1) || defined(EM2) || defined(EA1) || defined(FTR_L7E) || defined(FTR_L9)
	UINT32 flags;
#else
	UINT8 flags;
#endif
	UINT16 attrib;  // Очевидно, в результат попадают те файлы, у которых аттрибуты&mask==attrib
	UINT16 mask;
} FS_SEARCH_PARAMS_T;

typedef struct {
	WCHAR name[FILEPATH_MAX_LEN + 1];  // Полное имя фалйа (с путём)
	UINT16 attrib;
	UINT16 owner;
} FS_SEARCH_RESULT_T;

typedef UINT8 FS_SEARCH_HANDLE_T;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_SEARCH_T_H */

/** @} */ /* end of P2K_Types */
