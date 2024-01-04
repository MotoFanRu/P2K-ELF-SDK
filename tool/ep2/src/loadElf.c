
#include "elfloader.h"
#include <mem.h>
//#include <filesystem.h>
#include <utilities.h>
#include "elfloaderApp.h"
#include "console.h"
#include "utils.h"
//#include "config.h"
#include "API.h"
#include "elf.h"
#include "features.h"
#include "dbg.h"


static UINT32	registerElf( Ldr_IElf * pIElf, const WCHAR * uri, const WCHAR * params );


// load and execute ELF
// загрузка  и запуск эльфа
// status: DONE
// TODO: iElf to heap (DONE)
//
UINT32	loadElf( const WCHAR * uri, const WCHAR * params )
{
	UINT32				result = LDR_RESULT_OK;
	Ldr_IElf			iElf;
	Ldr_IElf			*pIElf = &iElf;
	//INT32				err;
	char				buf[48+1];
	
	u_nutoa( uri, buf, 48 );
	//cprintf_int( "\x87""Loading Elf '%s'\n", buf );
	dbgf( "enter, uri = '%s', stack=%x\n", buf, &buf[0] );
	
	__rt_memclr( (void*)pIElf, sizeof(Ldr_IElf) );
	
	do
	{
		if ( !DL_FsFFileExist( uri ) )
		{
			result = LDR_RESULT_ERROR_FILE_NOT_EXIST;
			break;
		}
		
		result = prepareElf( uri, pIElf, ET_EXEC );
		if ( result == LDR_RESULT_ERROR_EABI_VERSION2 )
		{
			ldrLoadElfOverEP1( uri, params );
			break;
		}
		
		if ( result != LDR_RESULT_OK )
			break;
		
		result = registerElf( pIElf, uri, params );
		//if ( result != LDR_RESULT_OK )
			//break;
		
	}
	while (0);
	
	dbgf( "exit, 0x%x\n", result );
	
	if ( pIElf )
	{
		clearElf( pIElf, result != LDR_RESULT_OK );
	}
	
	return result;
}


// run ELF and register in elf-list
// окончательная обработка эльфа как исполняемого приложения
//
UINT32 registerElf( Ldr_IElf * pIElf, const WCHAR * uri, const WCHAR * params )
{
	UINT32				result = LDR_RESULT_OK;
	ldrElf				*elf;
	int					i;
	INT32				err;
	
	dbgf( "enter, 0x%x\n", &elf );
	dbgf( "starting ELF at 0x%X\n", VIRT2PHYS( *pIElf, pIElf->elfHeader.e_entry ) );
	
#ifdef DUMP_ELF
	dump_elf( (void*)pIElf->physBase, pIElf->imageSize, getName( uri ) );
#endif
	
	elf = ( (ElfEntry)VIRT2PHYS( *pIElf, pIElf->elfHeader.e_entry ) )( uri, params );
	
	dbgf( "ELF returned with 0x%X\n", elf );
	
	if ( elf != NULL )
	{
		/*cprintf_int( "\x87 ELF loaded! Image size: \x8E%d\x87 bytes\n", 
				pIElf->imageSize );*/
		
		elf->addr = (void*)pIElf->physBase;
		elf->size = pIElf->imageSize;
		elf->app = NULL;
		//elf->next = NULL;
		elf->link = pIElf->link;
		
		i = u_strlen( uri ) - 1;
		while ( i >= 0 && uri[i] != L'/' ) i--;
		i++;
		
		elf->dir = suAllocMem( (i + 1) * sizeof(WCHAR), &err );
		if ( elf->dir )
		{
			u_strncpy( elf->dir, uri, i );
			elf->dir[i] = 0;
		}
		
		if ( elfListAdd( &loadedElfsList, elf ) )
			ldrSendEvent( eventElfsListChanged );
	}
	else
	{
		//cprint_int( "\x89""ELF loaded but return NULL\n" );
		result = LDR_RESULT_ERROR_RETURN_NULL;
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// terminate ELF
// выгрузить эльф из памяти (апп эльфа должно быть остановлено заранее)
// 
UINT32 unloadElf( void * addr )
{
	ldrElf		*elf;
	
	dbgf( "enter, addr=0x%X\n", addr );
	
	// find the corresponding ELF
	elf = elfListFindAndDel( &loadedElfsList, addr );
	if ( elf )
	{
		ldrSendEvent( eventElfsListChanged );
		return freeElf( elf );
	}
	
	return LDR_RESULT_FAIL;
}


// free mem from elf and linked resources
// освобождение памяти из под эльфа и связанных ресурсов
// 
UINT32	freeElf( ldrElf * elf )
{
	if ( !elf ) return LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	unloadLibs( elf->link );
	
	if ( elf->dir )
		suFreeMem( elf->dir );
	
	dbgf( "free elf=0x%X\n", elf->addr );
	if ( elf->addr )
		suFreeMem( elf->addr );
	
	return LDR_RESULT_OK;
}


/* EOF */
