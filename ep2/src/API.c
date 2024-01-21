
#include "API.h"
#include <mem.h>
#include <utilities.h>
#include "elfloader.h"
#include "elfloaderApp.h"
#include "features.h"
#include "utils.h"
#include "console.h"
#include "config.h"
#include "dd.h"
#include "dbg.h"


const char PHONE_PLATFORM[] = FTR_PHONE_PLATFORM;
const char PHONE_NAME[] = FTR_PHONE_NAME;

static UINT32		dl_result;


////////////////////////////////////////////////////////////////////////////////

ldrElf * ldrGetElfsList( UINT32 * count )
{
	if (count) 
		*count = loadedElfsList.count;
	
	return loadedElfsList.root;
}


ldrElf * ldrFindElf( const char * name )
{
	ldrElf		*elf = loadedElfsList.root;
	
	if ( !name ) return NULL;
	while ( elf != NULL )
	{
		if ( namecmp( (char*)name, elf->name) ) 
			return elf;
		elf = elf->next;
	}
	
	return NULL;
}


BOOL ldrIsLoaded( const char *name )
{
	return ldrFindElf( name ) != NULL;
}

////////////////////////////////////////////////////////////////////////////////

UINT32	ldrRequestEventBase( )
{
	UINT32		base = currentEventBase;
	currentEventBase += EVBASE_RESERVE;
	return base;
}


UINT32 ldrSendEvent( UINT32 evcode )
{
	UINT32		status;
	status = AFW_CreateInternalQueuedEvAux(	evcode,
											FBF_LEAVE,
											0,
											NULL );
	//dbgf( "Event = 0x%X, status = %d\n", evcode, status );

	return status;
}


UINT32 ldrSendEventToApp( AFW_ID_T afwid, UINT32 evcode, void *attachment, UINT32 att_size, FREE_BUF_FLAG_T fbf )
{
	UINT32		status;
	status = AFW_CreateInternalQueuedEvPriv( evcode, 0, afwid, 0, 0, 0, fbf, att_size, attachment, 0 );
	return status;
}


UINT32 ldrSendEventToAppEmpty( AFW_ID_T afwid, UINT32 evcode )
{
	UINT32		status;
	status = AFW_CreateInternalQueuedEvPriv( evcode, 0, afwid, 0, 0, 0, FBF_LEAVE, 0, NULL, 0 );
	return status;
}

////////////////////////////////////////////////////////////////////////////////

UINT32	ldrLoadElf( const WCHAR * uri, const WCHAR * params )
{
	UINT32		status;
	
	if ( !DL_FsFFileExist( uri ) )
	{
		status = LDR_RESULT_ERROR_FILE_NOT_EXIST;
		cprint( "\x8C""Loading ELF failed - not exist\n" );
	}
	else
	{
		status = loadElf( uri, params );
	}
	
	dbgf( "status = %x\n", status );
	
	if ( status == LDR_RESULT_ERROR_RETURN_NULL )
	{
		cprint_int( "\x89""ELF loaded but return NULL\n" );
		return RESULT_OK;
	}
	if ( status == LDR_RESULT_ERROR_EABI_VERSION2 )
	{
		cprint_int( "\x89""Loading ELF redirected to EP1\n" );
		return RESULT_OK;
	}
	else if ( status != LDR_RESULT_OK )
	{
		cprintf( "\x8C""Loading ELF failed - error #%x\n", status );
		return RESULT_FAIL;
	}
	else
	{
		cprint_int( "\x89""Loading ELF success\n" );
	}
	
	return RESULT_OK;
}


UINT32 ldrLoadElfOverEP1( const WCHAR * uri, const WCHAR * params )
{
	ADD_EVENT_DATA_T	d;
	
	// u_nutoa handle NULL params
	u_nutoa( uri, d.data.start_params.uri, 63 );
	u_nutoa( params, d.data.start_params.params, 63 );
	
	//cprint_int( "\x89""Loading ELF redirected to EP1\n" );
	return AFW_CreateInternalQueuedEvAuxD( FTR_EV_EP1_LOAD_ELF, (void*)&d, FBF_LEAVE, 0, NULL );
}

////////////////////////////////////////////////////////////////////////////////

const char * ldrGetPhoneName( void )
{
	return PHONE_NAME;
}


const char * ldrGetPlatformName( void )
{
	return PHONE_PLATFORM;
}


const char * ldrGetFirmwareVersion( void )
{
	return product_version;
}

	/********************************************************\
							CONSTANTS

	\********************************************************/

UINT32 ldrGetConstVal( UINT16 constID )
{
	INT32	mid, low, high;
	
	if( cstLibrary.count == 0 )
	{
		//dbgf( "failed: no constlib loaded! Requested ID = 0x%X\n",   (UINT32)constID );
		return (UINT32)(-1);
	}
	
	// Бинарный поиск
	low = 0;
	high = cstLibrary.count-1;
	while( high >= low )
	{
		mid = ( low + high )/2;
		if( constID < cstLibrary.ID[mid] )
			high = mid - 1;
		else if( constID > cstLibrary.ID[mid] )
			low = mid + 1;
		else
			return cstLibrary.Vals[mid];
	}
	
	//dbgf( "failed: ID not present in constlib! Requested ID = 0x%X\n",   (UINT32)constID );
	
	return (UINT32)(-1);
}


UINT32 ldrInitEventHandlersTbl( EVENT_HANDLER_ENTRY_T *tbl,  UINT32 base )
{
	UINT32		code, i = 0;
	
	if (!tbl) return base;
	while( tbl[i].code != STATE_HANDLERS_END )
	{
		if( tbl[i].code == STATE_HANDLERS_RESERVED )	
		{
			tbl[i].code = base++;
		}
		else
		{
			code = ldrGetConstVal( tbl[i].code );
			if( code != (UINT32)(-1) )
				tbl[i].code = code;
			else
				;//dbg( "warning: leaving event code as-is\n" );
		}
		i++;
	}
	
	return base;
}


UINT32 ldrFindEventHandlerTbl( const EVENT_HANDLER_ENTRY_T *tbl, EVENT_HANDLER_T *hfn )
{
	UINT32			i = 0;
	
	if (!tbl) return 0;
	while( tbl[i].code != STATE_HANDLERS_END )
	{
		if( tbl[i].hfunc == hfn )	
			return tbl[i].code;
		i++;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void ldrToggleConsole()
{
	if ( !Config.EnableConsole ) return;
	
	if (conAction == CON_ACTION_HIDE || conAction == CON_ACTION_STAY)
	{
		conType = CON_TYPE_SHIFT;
	}
	
	if (conAction == CON_ACTION_FADEIN || conAction == CON_ACTION_STAY)
	{
		conAutoHide = TRUE;
		conAction = CON_ACTION_FADEOUT;
		StopTimer( ctimer_iface.handle );
		ctimer_iface.handle = CreateTimer( TID_FADEOUT, TIME_FADEOUT, ttCyclical, ctimer_iface.port );
	}
	else // CON_ACTION_FADEOUT, CON_ACTION_HIDE
	{
		conAutoHide = FALSE;
		conAction = CON_ACTION_FADEIN;
		StopTimer( ctimer_iface.handle );
		ctimer_iface.handle = CreateTimer( TID_FADEIN, TIME_FADEIN, ttCyclical, ctimer_iface.port );
	}
}


BOOL ldrEnableConsole( BOOL new_state )
{
	BOOL	old_state = Config.EnableConsole;
	
	if ( new_state == old_state ) return old_state;
	
	if ( !new_state )
	{
		if ( conAction != CON_ACTION_HIDE )
		{
			conStep = 0;
			conAction = CON_ACTION_HIDE;
			
			StopTimer( ctimer_iface.handle );
			ctimer_iface.handle = 0;
			
			cscroll( 0 );
			ConsoleRender( NULL );
		}
	}
	
	Config.EnableConsole = new_state;
	
	return old_state;
}

////////////////////////////////////////////////////////////////////////////////

const AHIDRVINFO_T * ldrGetAhiDrvInfo( void )
{
	return ahiDrvInfo;
}


const AHIDEVICE_T ldrGetAhiDevice( void )
{
	return ahiDev;
}

////////////////////////////////////////////////////////////////////////////////

UINT32	ldrLoadDefLibrary( void )
{
	if ( loadDefaultLib( NULL ) == LDR_RESULT_OK )
	{
		cprint_int( "\x89""DefLib loaded successful =)\n" );
		return RESULT_OK;
	}
	
	cprint_int( "\x8C""DefLib don't loaded =(\n" );
	return RESULT_FAIL;
}


////////////////////////////////////////////////////////////////////////////////


void ldrSetDefaultConfig( void )
{
	freeConfig( &Config );
	setDefaultConfig( &Config );
}


const LDR_CONFIG_T * ldrGetConfig( void )
{
	return &Config;
}


UINT32 ldrLoadConfig( void )
{
	UINT32		result;
	
	ldrSetDefaultConfig();
	
	result = loadConfig( &Config, FTR_CONFIG_URI1 );
	if ( result != RESULT_OK )
		result = loadConfig( &Config, FTR_CONFIG_URI2 );
	
	if ( result == RESULT_OK )
		cprint_int( "\x89""Config processed\n" );
	else
		cprintf_int( "\x8C""Config error #%d, Using default\n", result );
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////


// INFO: RTLD_NOW | RTLD_LOCAL by default, RTLD_LAZY and RTLD_GLOBAL ignored
// TODO: RTLD_GLOBAL
DLHANDLE dlopen_int( const WCHAR * uri, int mode, const WCHAR * cur_dir )
{
	UINT32		result = LDR_RESULT_FAIL;
	WCHAR		*lib_uri = NULL;
	INT32		err;
	Ldr_Lib		*lib;
	#ifdef DEBUG
	char		buf[48+1];
	#endif
	
	#ifdef DEBUG
	u_nutoa( cur_dir, buf, 48);
	dbgf( "cur_dir = '%s'\n", buf );
	#endif
	
	if ( !DL_FsFFileExist( uri ) )
	{
		do
		{
			lib_uri = suAllocMem( FS_MAX_URI_NAME_LENGTH + 1, &err );
			dbgf( "alloc lib_uri=0x%x\n", lib_uri );
			if ( !lib_uri )
			{
				result = LDR_RESULT_ERROR_MEM_ALLOC;
				break;
			}
			
			result = findLibrary( uri, cur_dir, lib_uri );
		}
		while ( 0 );
		
		if ( result == LDR_RESULT_OK )
			dl_result = loadLibrary( lib_uri, &lib, mode );
		else
			dl_result = result;
	}
	else
	{
		dl_result = loadLibrary( uri, &lib, mode );
	}
	
	if ( lib_uri )
	{
		dbgf( "free lib_uri=0x%X\n", lib_uri );
		suFreeMem( lib_uri );
	}
	
	if ( dl_result != LDR_RESULT_OK )
		return (DLHANDLE)NULL;
	
	return (DLHANDLE)lib;
}


UINT32 dlclose( DLHANDLE handle )
{
	if ( handle )
		dl_result = unloadLibrary( (Ldr_Lib*)handle );
	else
		dl_result = LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	return dl_result;
}


void * dlsym( DLHANDLE handle, const char * name )
{
	void * sym;
	
	if ( !handle || !name )
	{
		dl_result = LDR_RESULT_ERROR_BAD_ARGUMENT;
		return NULL;
	}
	
	sym = (void*)findSymbol( (Ldr_Lib*)handle, name );
	if ( sym )
	{
		dl_result = LDR_RESULT_OK;
		return sym;
	}
	
	dl_result = LDR_RESULT_ERROR_SYM_NOT_FIND;
	return NULL;
}


UINT32 dlerror( void )
{
	return dl_result;
}


/* EOF */
