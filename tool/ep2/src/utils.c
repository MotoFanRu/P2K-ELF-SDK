#include "utils.h"
#include <events.h>
#include <filesystem.h>
#include <utilities.h>
#include <stdarg1.h>
#include <dl.h>
#include "api.h"
//#include "elfloaderApp.h"
#include "features.h"
#include "dbg.h"


ELF_LIST_T			loadedElfsList;



UINT32 namecmp( const char * str1, const char * str2 )
{
	UINT32 i=0;
	
	if ( !str1 || !str2 ) return 0;
	
	while( str1[i] == str2[i] )
	{
		if(str1[i++] == 0) return 1;
	}
	return 0;
}


const WCHAR * getName( const WCHAR * uri )
{
	int i;
	
	if ( !uri ) return NULL;
	i = u_strlen(uri);
	
	do
	{
		i--;
	} while ( i > 0 && uri[i-1] != L'/' );
	
	return &uri[i];
}


char* u_utoa( const WCHAR * ustr, char * astr )
{
	UINT32  i=0;
	
	if ( !astr ) return NULL;
	if ( !ustr )
	{
		astr[0] = 0;
	}
	else
	{
		do
		{
			astr[i] = (char)ustr[i];
		} while ( ustr[i++] != 0 );
	}
	
	return astr;
}


char * u_nutoa( const WCHAR * ustr, char * astr, int n )
{
	UINT32  i=0;
	
	if ( !astr ) return NULL;
	if ( !ustr || !n )
	{
		astr[0] = 0;
	}
	else
	{
		while ( i < n && ustr[i] != 0 )
		{
			astr[i] = (char)ustr[i];
			i++;
		}
		astr[i] = 0;
	}
	
	return astr;
}


WCHAR * u_natou( const char * astr, WCHAR * ustr, int n )
{
	UINT32  i=0;
	
	if ( !ustr ) return NULL;
	if ( !astr || !n )
	{
		ustr[0] = 0;
	}
	else
	{
		while ( i < n && astr[i] != 0 )
		{
			ustr[i] = (WCHAR)astr[i];
			i++;
		}
		ustr[i] = 0;
	}
	
	return ustr;
}


void UtilLogStringData( const char * format, ... )
{
	char			buffer[255];
	va_list			vars;
	
	if ( !format || !format[0] ) return;
	
	va_start( vars, format );
	vsnprintf( buffer, 250, format, vars );
	va_end( vars );
	
	suLogData( 0, 0x5151, 1, strlen( buffer ) + 1, buffer );
}


void UtilDebug( const char * format, ... )
{
	char			buffer[255];
	va_list			vars;
	
#ifdef LOG_TO_FILE
	FILE_HANDLE_T	f;
	UINT32			written;
	UINT32			sz;
#endif
	
	if ( !format || !format[0] ) return;
	
	va_start( vars, format );
	vsnprintf( buffer, 250, format, vars );
	va_end( vars );
	
	PFprintf( buffer );
	
#ifdef LOG_TO_FILE
	sz = strlen( buffer );
	/*buffer[sz++] = '\r';
	buffer[sz++] = '\n';
	buffer[sz] = '\0';*/
	f = DL_FsOpenFile( FTR_LOG_FILE_URI, FILE_APPEND_MODE, 0 );
	if ( f == FILE_HANDLE_INVALID ) return;
	DL_FsWriteFile( (void*)buffer, sz, 1, f, &written );
	DL_FsCloseFile( f );
#endif
}


#ifdef DUMP_ELF
UINT32 dump_elf( const void * addr, UINT32 size, const WCHAR * name )
{
	FILE_HANDLE_T		fh;
	UINT8				fr;
	UINT32				count;
	WCHAR				dst[FILEURI_MAX_LEN+1];
	
	
	u_strcpy( dst, FTR_DUMPS_DIR );
	u_strcat( dst, name );
	u_strcat( dst, L"." );
	u_itoh( (UINT32)addr,  &dst[ u_strlen(dst) ] );
	u_strcat( dst, L".dump");
	
	fh = DL_FsOpenFile( dst, FILE_WRITE_MODE, 0 );
	fr = DL_FsWriteFile( (void*)addr, size, 1, fh, &count );
	fr |= DL_FsCloseFile( fh );
	
	return fr == 0 ? RESULT_OK : RESULT_FAIL;
}
#endif


UINT32 CreateTimer( UINT32 id, UINT32 period, TIMER_TYPE_T type, SU_PORT_T port )
{
	IFACE_DATA_T			iface;
	
	if ( period == 0 || type >= ttMAX ) return 0;
	
	iface.port = port;
	
	if ( type == ttSimple )
		DL_ClkStartTimer( &iface, period, id );
	else //if (type == ttCyclical)
		DL_ClkStartCyclicalTimer( &iface, period, id );
	
	//dbg( "*** ELFPACK2 *** CreateTimer: id=0x%x, period=0x%x, port=0x%x, 
	//	handle=0x%x", id, period, port, iface.handle );
	
	return iface.handle;
}


UINT32 StopTimer( UINT32 handle )
{
	IFACE_DATA_T			iface;
	
	//dbg( "*** ELFPACK2 *** StopTimer: handle=0x%08x", handle );
	
	if ( handle )
	{
		iface.handle = handle;
		DL_ClkStopTimer( &iface );
	}
	
	return RESULT_OK;
}



#define UtilIsNullRect(rect) (((rect)->x1 == 0) && ((rect)->x2 == 0) && ((rect)->y1 == 0) && ((rect)->y2 == 0))

BOOL UtilIsRectIntersect( const RECT_T * r1, const RECT_T * r2 )
{
	INT16	d;
	
	if ( !r1 || !r2 ) return FALSE;
	if ( UtilIsNullRect(r1) || UtilIsNullRect(r2) ) return TRUE;
	
	d = min(r1->x2, r2->x2) - max(r1->x1, r2->x1);
	if ( d <= 0 ) return FALSE;
	
	d = min( r1->y2, r2->y2 ) - max( r1->y1, r2->y1 );
	if ( d <= 0 ) return FALSE;
	
	return TRUE;
}


BOOL UtilRectIntersect( RECT_T * dst, const RECT_T * r1, const RECT_T * r2 )
{
	if ( !dst ) return UtilIsRectIntersect( r1, r2 );
	if ( !r1 || !r2 ) return FALSE;
	
	if ( UtilIsNullRect(r1) )
	{
		if ( dst != r2 ) *dst = *r2;
		return TRUE;
	}
	
	if ( UtilIsNullRect(r2) )
	{
		if ( dst != r1 ) *dst = *r1;
		return TRUE;
	}
	
	dst->x1 = max(r1->x1, r2->x1);
	dst->x2 = min(r1->x2, r2->x2)+1;
	if ( (dst->x2 - dst->x1) <= 0 ) return FALSE;
	
	dst->y1 = max(r1->y1, r2->y1);
	dst->y2 = min(r1->y2, r2->y2)+1;
	if ( (dst->y2 - dst->y1) <= 0 ) return FALSE;

	return TRUE;
}



ldrElf * elfListAdd( ELF_LIST_T * list, ldrElf * elf )
{
	if ( !list || !elf ) return NULL;
	
	if ( list->root == NULL ) 
		list->root = elf;
	else
		list->tail->next = elf;
	
	list->tail = elf;
	
	list->count++;
	
	elf->next = NULL;
	
	return elf;
}


ldrElf * elfListFind( ELF_LIST_T * list, void * addr )
{
	INT32		def;
	ldrElf		*elf;
	
	if ( !list || !addr ) return NULL;
	
	elf = list->root;
	while ( elf )
	{
		def = (INT32)addr - (INT32)elf->addr;
		if ( (def >= 0) && (def < elf->size) )
		{
			return elf;
		}
		elf = elf->next;
	}
	
	return NULL;
}


ldrElf * elfListFindAndDel( ELF_LIST_T * list, void * addr )
{
	INT32		def;
	ldrElf		*elf, *pelf = NULL;
	
	if ( !list || !addr ) return NULL;
	
	elf = list->root;
	while ( elf )
	{
		def = (INT32)addr - (INT32)elf->addr;
		if ( (def >= 0) && (def < elf->size) )
		{
			if ( elf->next == NULL ) // deleting the last one
				list->tail = pelf;
			
			if ( pelf == NULL )
				list->root = elf->next; // deleting the first one
			else 
				pelf->next = elf->next; // deleting from the middle
			
			list->count--;
			
			return elf;
		}
		pelf = elf;
		elf = elf->next;
	}
	
	return NULL;
}


// состояние: TRUE - клавиатура заблок., FALSE  - разблок.
BOOL KeypadLock( void )
{
	UINT8		keypad_statate;
	
	DL_DbFeatureGetCurrentState( DB_FEATURE_KEYPAD_STATE, &keypad_statate );
	
	return keypad_statate;
}


/* EOF */
