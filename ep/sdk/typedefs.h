// UTF-8 w/o BOM

#ifndef SDK_TYPEDEFS_H
#define SDK_TYPEDEFS_H

#ifdef EMUELF
	#include <basetsd.h>
	#include <windows.h>
	#include <memory.h>

	// system types conflict workaround

	#define INT32					ELFINT32
	#define UINT32					ELFUINT32
	#define INT64					ELFINT64
	#define BOOL					ELFBOOL
#endif

typedef signed char				INT8;
typedef unsigned char			UINT8;
typedef signed short			INT16;
typedef unsigned short			UINT16;
typedef signed long				INT32;
typedef unsigned long			UINT32;

#if (defined (WIN32)) && !(defined (__GNUC__))
	typedef signed __int64			INT64;
	typedef unsigned __int64		UINT64;
#else
	typedef signed long long		INT64;
	typedef unsigned long long		UINT64;
#endif

typedef UINT8					BOOL;
typedef UINT8					BOOLEAN;

#ifdef __cplusplus
typedef wchar_t					WCHAR;
#else
typedef UINT16					WCHAR;
#endif

typedef UINT16					W_CHAR;
typedef UINT8					BYTE;
typedef UINT16					UIS_STRING_T;
typedef char					UIS_ASCII_CHAR;

typedef INT8 SYN_BOOL;
#define SYN_TRUE   (SYN_BOOL)(1)
#define SYN_FALSE  (SYN_BOOL)(0)

#define SYN_SUCCESS  (0)
#define SYN_FAIL     (1)
typedef INT32 SYN_RETURN_STATUS_T;
typedef INT32 RETURN_STATUS_T;

#define SYN_NULL   0


#ifndef EMUELF
	#define TRUE					(BOOL)(1)
	#define FALSE					(BOOL)(0)
#endif


#define true					(BOOL)(1)
#define false					(BOOL)(0)

#define RESULT_OK				(0)
#define RESULT_FAIL				(1)

#ifndef EMUELF
	#define NULL					(0)
#endif

typedef UINT32					EVENT_CODE_T;
#define AFW_EVENT_CODE_T		EVENT_CODE_T
typedef UINT32					UIS_DIALOG_T;
typedef UINT32					SU_PORT_T;
typedef void*					SU_PORT_HANDLE;
typedef UINT32					AFW_ID_T;
typedef UINT64					DL_FS_MID_T;

typedef unsigned int			size_t;

typedef UINT32					HANDLE_T;

typedef int SU_TIME;
typedef INT64 SU_TIME64;


typedef UINT32				UIS_COLOR_T;

enum FREE_BUF_FLAG_ENUM
{
	FBF_FREE = 100,  // ОС должна освободить буффер
	FBF_LEAVE        // Приложение должно следить за освобождением буффера
} ;
typedef UINT8 FREE_BUF_FLAG_T;

typedef struct {
	SU_PORT_T 	port;
	UINT32 		handle;
} IFACE_DATA_T;

typedef struct
{
	UINT32	R0;
	UINT32	R1;
} _u64;

typedef struct APPLICATION_T APPLICATION_T;
typedef struct EVENT_STACK_T EVENT_STACK_T;
typedef struct EVENT_T EVENT_T;



#endif // SDK_TYPEDEFS_H
