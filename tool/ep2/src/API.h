
#ifndef __LDR_API_H__
#define __LDR_API_H__

#include <typedefs.h>
#include <apps.h>
#include <ATI.h>
#include "elfloader.h"
#include "config.h"



typedef struct
{
	INT16 x1, y1, x2, y2;
} RECT_T;

typedef void (*dispCbk)( const RECT_T * );


// din-lib mode
enum
{
	RTLD_NOW		= 0,
	RTLD_LAZY		= 1,
	RTLD_LOCAL		= 0,
	RTLD_GLOBAL		= 2
};


#ifdef __cplusplus
extern "C" {
#endif


ldrElf*	ldrGetElfsList( UINT32 *count );
ldrElf*	ldrFindElf( const char * name );
BOOL	ldrIsLoaded( const char *name );
UINT32	ldrRequestEventBase( void );
UINT32	ldrSendEvent( UINT32 evcode );
UINT32	ldrSendEventToApp( UINT32 afwid, UINT32 evcode, void *attachment, UINT32 att_size, FREE_BUF_FLAG_T fbf );
UINT32	ldrSendEventToAppEmpty( UINT32 afwid, UINT32 evcode );
UINT32	ldrLoadElf( const WCHAR * uri, const WCHAR * params );
UINT32	ldrLoadElfOverEP1( const WCHAR * uri, const WCHAR * params );
UINT32	ldrUnloadElf( void );

const char* ldrGetPhoneName( void );
const char* ldrGetPlatformName( void );
const char* ldrGetFirmwareVersion( void );


// Callbacks API

void	cbkDisplayInit( void );

BOOL	ldrDisplayCbkReg( dispCbk cbk, const RECT_T * target, BOOL postflush );
BOOL	ldrDisplayCbkUnReg( dispCbk cbk );

void	cbkDisplayHookPre( const RECT_T * updatedRect, void * src, UINT8 displayID );
void	cbkDisplayHookPost( const RECT_T * updatedRect, void * src, UINT8 displayID );

// Constants API
UINT32	ldrGetConstVal( UINT16 constID );
UINT32	ldrInitEventHandlersTbl( EVENT_HANDLER_ENTRY_T * tbl,  UINT32 base );
UINT32	ldrFindEventHandlerTbl( const EVENT_HANDLER_ENTRY_T * tbl,  EVENT_HANDLER_T * hfn );

/* доступ к АТИ */
const	AHIDRVINFO_T * ldrGetAhiDrvInfo( void );
const	AHIDEVICE_T ldrGetAhiDevice( void );

/* конфиг */
const	LDR_CONFIG_T * ldrGetConfig( void );
UINT32	ldrLoadConfig( void );
void	ldrSetDefaultConfig( void );

/* перегрузка либы */
UINT32	ldrLoadDefLibrary( void );

void	ldrToggleConsole( void );

// Dynamic libraries
DLHANDLE	dlopen_int( const WCHAR * uri, int mode, const WCHAR * cur_dir );
DLHANDLE	dlopen( const WCHAR * uri, int mode );
UINT32		dlclose( DLHANDLE handle );
void *		dlsym( DLHANDLE handle, const char * name );
UINT32		dlerror( void );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* __LDR_API_H__ */

