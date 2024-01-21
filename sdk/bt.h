// UTF-8 w/o BOM
/* ***********************************
 *	Структуры и событие для приложения,
 *		передающего файлы по БТ
 */
#ifndef SDK_BLUETOOTH_SEND_APP_H
#define SDK_BLUETOOTH_SEND_APP_H

#include <typedefs.h>
#include <filesystem.h>

#define MIME_TYPE_STRING_LEN	50

#define CNT_LTE_TYPE_PR_REC		3
#define CNT_LTE_TYPE_SC_URL		4
#define CNT_LTE_TYPE_FILE		5

#ifdef FTR_L7E
#define CNT_LTE2_TYPE_FILE		6
#else
#define CNT_LTE2_TYPE_FILE		5
#endif



typedef struct {
	WCHAR		uri[FILEURI_MAX_LEN+1];			// strlen("file:/") = 6
	char		mime_type[MIME_TYPE_STRING_LEN+1];	// ="*/*"
	BOOL		remove;
	UINT16		pad;				// =0
	UINT16		num_files;			// =1
} LTE_SEND_BT_FILE;

// R373 BT Attach
typedef struct {
	UINT8		content_type;	// CNT_*
	union {
		LTE_SEND_BT_FILE	file;	// CNT_LTE_TYPE_FILE
		UINT8				pad[592];
		UINT32				pad2[148];	// Нужно, чтобы компилятор выравнял начало обьединения к 4 байтам
	} cnt;
} LTE_SEND_BT_ATT;	// size=596



// LTE2 BT Attach
typedef struct {
#ifdef FTR_L7E
	UINT16		unk2;			// 1
	UINT8		pad4[6];		// 0
#endif
	UINT8		content_type;	// CNT_LTE2_TYPE_FILE for file
	UINT8		pad1[584];		// 0
	BOOL		remove;			// delete files after sending
	UINT16		pad2;			// 0
	FS_MID_T *	p_ids;
	UINT32		num_files;
	UINT8		pad3[1848];		// 0
#ifndef FTR_L7E
	UINT16		unk2;			// 1
	UINT8		pad4[6];		// 0
#endif
} LTE2_SEND_BT_ATT;	// size = 2452


// LTE2 V3R BT Attach
typedef struct {
#ifdef FTR_L7E
	UINT16		unk2;			// 1
	UINT8		pad4[6];		// 0
#endif
	UINT8		content_type;	// CNT_LTE2_TYPE_FILE for file
	UINT8		pad1[584];		// 0
	BOOL		remove;			// delete files after sending
	UINT16		pad2;			// 0
	FS_MID_T *	p_ids;
	UINT32		num_files;
	UINT8		pad3[281];		// 0
#ifndef FTR_L7E
	UINT16		unk2;			// 1
	UINT8		pad4[6];		// 0
#endif
} LTE2_V3R_SEND_BT_ATT;	// size = 2452

// структуры нужно исправить с учетом архитектуры ARM



#endif // SDK_BLUETOOTH_SEND_APP_H
