#ifndef __UTILS_H__
#define __UTILS_H__

#include <typedefs.h>
#include "api.h"



enum {
	ttSimple,
	ttCyclical,
	ttMAX
};
typedef UINT8 TIMER_TYPE_T;


typedef struct
{
	UINT32			count;
	ldrElf			*root;
	ldrElf			*tail;
} ELF_LIST_T;

extern ELF_LIST_T		loadedElfsList;


#ifdef __cplusplus
extern "C" {
#endif


UINT32 namecmp( const char * str1, const char * str2 );
const WCHAR * getName( const WCHAR * uri );
char * u_utoa( const WCHAR * ustr, char * astr );
char * u_nutoa( const WCHAR * ustr, char * astr, int n );
WCHAR * u_natou( const char * astr, WCHAR * ustr, int n );

void UtilLogStringData( const char * format, ... );
void UtilDebug( const char * format, ... );

UINT32 dump_elf( const void * addr, UINT32 size, const WCHAR * name );

/* handle returned */
UINT32 CreateTimer( UINT32 id, UINT32 period, TIMER_TYPE_T type, SU_PORT_T port );
UINT32 StopTimer( UINT32 handle );


BOOL UtilIsRectIntersect( const RECT_T * r1, const RECT_T * r2 );
BOOL UtilRectIntersect( RECT_T * dst, const RECT_T * r1, const RECT_T * r2 );


ldrElf * elfListAdd( ELF_LIST_T * list, ldrElf * elf );
ldrElf * elfListFind( ELF_LIST_T * list, void * addr );
ldrElf * elfListFindAndDel( ELF_LIST_T * list, void * addr );


// состояние: TRUE - клавиатура заблок., FALSE  - разблок.
BOOL KeypadLock( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __UTILS_H__ */
