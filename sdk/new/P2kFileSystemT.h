/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_FS_T_H
#define P2K_SDK_FS_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FS_MAX_URI_NAME_LENGTH  264  // file://b/a.txt // please, use FS_MAX_URI_NAME_LENGTH+1
#define FS_MAX_PATH_NAME_LENGTH 258  // /b/a.txt //аналогично
#define FS_MAX_FILE_NAME_LENGTH 255  // a.txt // аналогично

#define FILENAME_MAX_LEN        FS_MAX_FILE_NAME_LENGTH
#define FILEURI_MAX_LEN         FS_MAX_URI_NAME_LENGTH
#define FILEPATH_MAX_LEN        FS_MAX_PATH_NAME_LENGTH

#define FS_HANDLE_INVALID       0xFFFF
#define FILE_HANDLE_INVALID     FS_HANDLE_INVALID
#define FILE_INVALID            FS_HANDLE_INVALID

typedef UINT16 FS_HANDLE_T;
typedef FS_HANDLE_T FILE_HANDLE_T;
typedef UINT64 FS_MID_T;  // физический идентификатор файла

typedef UINT32 FS_COUNT_T;      /* Count for writing, reading, inserting, and removing file data */
typedef UINT8 FS_PERCENT_T;     /* Percent complete of an operation (0-100) */
typedef INT32 FS_SEEK_OFFSET_T; /* Seek offset value */
typedef UINT32 FS_SIZE_T;
typedef UINT8 FS_RESULT_T;  // 0 - OK, >= 1 - FAIL
typedef FS_HANDLE_T				FILE;

typedef struct {
	UINT32 offset;
	UINT32 size;
	FILE_HANDLE_T file_handle;
} FILE_HANDLE_REFERENCE_T;

// Неизвестные параметры следует задавать нулём

/* Для mode в DL_FsOpenFile. За подробностями - см. описание
    стандартных функций C stdio.h  */
enum {
	FILE_READ_MODE = 0,          // Открыть дл чтения
	FILE_WRITE_MODE,             // Для записи
	FILE_WRITE_EXIST_MODE,       // Запись, если файл не существует
	FILE_APPEND_MODE,            // Запись в конец файла
	FILE_READ_PLUS_MODE,         // Открыть для чтения и записи
	FILE_WRITE_PLUS_MODE,        // Создать для чтения и записи
	FILE_WRITE_EXIST_PLUS_MODE,  // Создать для чтения и записи, если не существует
	FILE_APPEND_PLUS_MODE  // Открыть или создать файл для чтения/записи в конец
};

// Для whence в DL_FsSeekFile
enum {
	SEEK_WHENCE_SET = 0,
	SEEK_WHENCE_CUR,
	SEEK_WHENCE_END
};
typedef UINT8 SEEK_WHENCE_T;

// Атрибуты
#define FS_ATTR_DEFAULT                        0x0000
#define FS_ATTR_READONLY                       0x0001
#define FS_ATTR_HIDDEN                         0x0002
#define FS_ATTR_SYSTEM                         0x0004
#define FS_ATTR_VOLUME                         0x0008
#define FS_ATTR_DIRECTORY                      0x0010
#define FS_ATTR_ARCHIVE                        0x0020

// VOLUME_DESCR_T.device
typedef enum {
	FS_DEVICE_NONE = 0x0000,
	FS_DEVICE_FLASH = 0x0001,
	FS_DEVICE_MMC_INTERNAL = 0x0002,
	FS_DEVICE_MMC = 0x0004,
	FS_DEVICE_MMC_EXTERNAL = FS_DEVICE_MMC,
	FS_DEVICE_SDC = 0x0008,
	FS_DEVICE_NETWORK = 0x0010,
	FS_DEVICE_FLASH_NONUSER = 0x0020,
	FS_DEVICE_TFR = 0x0040,
	FS_DEVICE_MMC_RAMDISK = 0x0100,
	FS_DEVICE_ALL = 0x7FFF,
	FS_DEVICE_UNSUPPORTED = 0x8000,
	FS_DEVICE_INVALID = FS_DEVICE_UNSUPPORTED
} FS_DEVICE_T;

typedef struct {
	WCHAR volume[3];     // +0
#if defined(EM1) || defined(EM2) || defined(EA1)
	WCHAR vol_name[13];  // flash (for phone)
#else
	WCHAR vol_name[12];  // +6
#endif
	UINT32 free;         // +32 in bytes
	UINT32 capacity;     // +36 in bytes
	UINT16 vol_attr;     // +40 // ??. For vol /b attr==type
	UINT8 device_id;     // +42 0x01
#if defined(EM1) || defined(EM2) || defined(EA1)
	UINT8 unk2;        // 0x00
#endif
	UINT16 device;       // +44 ?? 0x40 - TRANS / 0x01 - flash. FileSystem???
#if defined(EM1) || defined(EM2) || defined(EA1)
	UINT16 unk3;
	UINT32 unk4;  // pointer to DSP??? only for TRANS
#else
	UINT32 serial_num;   // +48
#endif
} VOLUME_DESCR_T;

// Тип удаления
enum FS_REMOVE_T_ENUM_ {
	FS_REMOVE_DEFAULT = 0,
	FS_REMOVE_FORCE = 1
};

typedef UINT8 FS_REMOVE_T;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_FS_T_H */

/** @} */ /* end of P2K_Types */
