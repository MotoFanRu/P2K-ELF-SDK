// UTF-8 w/o BOM

#ifndef SDK_TYPEDEFS_H
#define SDK_TYPEDEFS_H

#include "P2kTypes.h"

typedef UINT32 EVENT_CODE_T; // MOVE: App? Event?
//#define AFW_EVENT_CODE_T EVENT_CODE_T // DROP
typedef UINT32 UIS_DIALOG_T; // MOVE: App? Uis?
typedef UINT32 SU_PORT_T;    // MOVE: SUAPI?
typedef void *SU_PORT_HANDLE; // MOVE: SUAPI?
//typedef UINT32 AFW_ID_T;      // MOVE: App? Afw?
//typedef UINT64 DL_FS_MID_T;   // MOVE: DL_FS?
//typedef UINT16 FILE_HANDLE_T;  // MOVE: DL_FS?


//typedef unsigned int size_t;  // ?
//typedef UINT32 HANDLE_T;  // ?

//typedef int SU_TIME;      // MOVE: SUAPI, TIME?
//typedef INT64 SU_TIME64; // MOVE: SUAPI, TIME?

//typedef UINT32 UIS_COLOR_T; // MOVE: UIS, Canvas?

// MOVE: App? Afw?
//typedef UINT16 REG_ID_T;
//typedef UINT16 APP_ID_T;
//typedef UINT32 AFW_ID_T;
//typedef UINT32 LOG_ID_T;

// MOVE: App? Afw?
typedef struct {
	SU_PORT_T port;
	UINT32 handle;
} IFACE_DATA_T;

// ?
typedef struct {
	UINT32 R0;
	UINT32 R1;
} _u64;

// MOVE: App? Afw?
typedef struct APPLICATION_T APPLICATION_T;
typedef struct EVENT_STACK_T EVENT_STACK_T;
typedef struct EVENT_T EVENT_T;

// MOVE: ? DROP?
#define WCHAR_PARAMS_MAX (64)

// EXTERN_LIB - используюется для того чтобы хапать значения из либы O_o
// ??????
typedef UINT32 EXTERN_LIB(void);

#endif  // SDK_TYPEDEFS_H
