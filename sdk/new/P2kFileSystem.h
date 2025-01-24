/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_FS_H
#define P2K_SDK_FS_H

#include "P2kT.h"
#include "P2kFileSystemT.h"

#ifdef __cplusplus
extern "C" {
#endif

// Неизвестные параметры следует задавать нулём

FILE_HANDLE_T DL_FsOpenFile(const WCHAR *uri, UINT8 mode, UINT16 owner);

UINT8 DL_FsCloseFile(FILE_HANDLE_T handle);

UINT8 DL_FsReadFile(void *buffer, UINT32 size, UINT32 count, FILE_HANDLE_T handle, UINT32 *read);

UINT8 DL_FsWriteFile(void *buffer, UINT32 size, UINT32 count, FILE_HANDLE_T handle, UINT32 *written);

UINT8 DL_FsFSeekFile(FILE_HANDLE_T handle, INT32 off, SEEK_WHENCE_T whence);

// Размер файла
UINT32 DL_FsGetFileSize(FILE_HANDLE_T handle);
UINT32 DL_FsSGetFileSize(const WCHAR *uri, UINT16 owner);

// Имя файла без расширения
WCHAR *DL_FsGetFileName(FILE_HANDLE_T handle, WCHAR *name);
WCHAR *DL_FsSGetFileName(const WCHAR *file_uri, WCHAR *name);

// Имя файла с расширением
WCHAR *DL_FsGetFileNameWithExt(FILE_HANDLE_T handle, WCHAR *name);
WCHAR *DL_FsSGetFileNameWithExt(const WCHAR *uri, WCHAR *name);

// Путь к файлу (без диска)
WCHAR *DL_FsGetPath(FILE_HANDLE_T handle, WCHAR *path);
WCHAR *DL_FsSGetPath(const WCHAR *file_uri, WCHAR *path);

// Тип файла
WCHAR *DL_FsGetFileType(
	FILE_HANDLE_T handle,
	WCHAR *type
);  // Мне удалось получить тип только тех файлов, которые лежали на диске а. хз почему :(
WCHAR *DL_FsSGetFileType(const WCHAR *uri, WCHAR *type);  // а эта ф-ция работает нормально со всеми файлами!

// Диск на котором расположен файл /a /b /e
WCHAR *DL_FsGetVolume(FILE_HANDLE_T handle, WCHAR *vol_uri);
WCHAR *DL_FsSGetVolume(const WCHAR *file_uri, WCHAR *vol_uri);

BOOL DL_FsDirExist(const WCHAR *uri);
BOOL DL_FsFFileExist(const WCHAR *uri);

UINT32 DL_FsFGetPosition(FILE_HANDLE_T handle);

// Переименовать файл
UINT8 DL_FsRenameFile(FILE_HANDLE_T handle, const WCHAR *new_name);
UINT8 DL_FsSRenameFile(const WCHAR *old_uri, const WCHAR *new_name, UINT16 owner);

// Удалить файл
UINT8 DL_FsDeleteFile(const WCHAR *uri, FS_REMOVE_T rmtype);

// Переместить файл
UINT8 DL_FsFMoveFile(const WCHAR *src_uri, const WCHAR *dst_uri, UINT16 owner);
// Копировать файл?
UINT32 DL_FsICopyFile( void * r0, WCHAR * uri1, WCHAR * uri2, UINT32 param3, UINT32 uarg_0 );	//r0=0, r3=0

// Создать директорию
UINT8 DL_FsMkDir(const WCHAR *uri, UINT16 owner);

// Удалить директорию
UINT8 DL_FsRmDir(const WCHAR *uri, UINT16 owner, FS_REMOVE_T rmtype);

UINT8 DL_FsFSetAttr(FILE_HANDLE_T handle, UINT16 attrib);
UINT8 DL_FsSSetAttr(const WCHAR *uri, UINT16 owner, UINT16 attrib);

UINT16 DL_FsGetAttr(FILE_HANDLE_T handle);
UINT16 DL_FsSGetAttr(const WCHAR *uri, UINT16 owner);

void DL_FsFlush(void);

// получить физический идентификатор файла
UINT32 DL_FsGetIDFromURI(const WCHAR *uri, FS_MID_T *id);
// получить путь к файлу по id
WCHAR *DL_FsGetURIFromID(const FS_MID_T *id, WCHAR *uri);

// получает список дисков
WCHAR *DL_FsVolumeEnum(WCHAR *result);  // result = L{'/','a',0xfffe,'/', ...}

/* Свойства раздела (диска) */
VOLUME_DESCR_T *DL_FsGetVolumeDescr(WCHAR *volume, VOLUME_DESCR_T *vd);

BOOL DL_FsIsCardInserted(const WCHAR *vol_uri);

UINT32 URINameValid(WCHAR *uri);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_FS_H */

/** @} */ /* end of P2K_Types */
