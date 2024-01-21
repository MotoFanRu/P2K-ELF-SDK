
#include "elfloader.h"
#include <mem.h>
//#include <filesystem.h>
#include <utilities.h>
//#include "elfloaderApp.h"
#include "console.h"
#include "utils.h"
//#include "config.h"
//#include "API.h"
#include "elf.h"
#include "features.h"
#include "dbg.h"


static UINT32	prepareLibrary( Ldr_IElf * pIElf, const WCHAR * name );
static UINT32	freeLibrary( Ldr_Lib * lib );


typedef UINT32 INIT_FUNC_T( void );

// load dinamic library
// загрузка библиотеки
// status: DONE
//
UINT32	loadLibrary( const WCHAR * uri, Ldr_Lib ** plib, int mode )
{
	UINT32			result = LDR_RESULT_OK;
	Ldr_IElf		iElf;
	Ldr_IElf		*pIElf = &iElf;
	char			buf[48+1];
	INIT_FUNC_T		*_init;
	
	dbgf( "enter, stack=%x\n", &buf[0] );
	
	__rt_memclr( (void*)pIElf, sizeof(Ldr_IElf) );
	
	do
	{
		if ( !uri || !plib ) 
		{
			result = LDR_RESULT_ERROR_BAD_ARGUMENT;
			break;
		}
		
		#ifdef DEBUG
		u_nutoa( uri, buf, 48 );
		dbgf( "uri='%s', mode=%x\n", buf, mode );
		#endif
		
		if ( !DL_FsFFileExist( uri ) )
		{
			result = LDR_RESULT_ERROR_FILE_NOT_EXIST;
			break;
		}
		
		result = prepareElf( uri, pIElf, ET_DYN );
		if ( result != LDR_RESULT_OK )
			break;
		
		result = prepareLibrary( pIElf, getName( uri ) );
		if ( result != LDR_RESULT_OK )
			break;
		
		/* INIT */
		//_init = (INIT_FUNC_T*)VIRT2PHYS( *pIElf, pIElf->dynTags[DT_INIT] );
		_init = (INIT_FUNC_T*)findSymbol( pIElf->lib, "_init" );
		if ( _init )
		{
			result = _init();
			dbgf( "Lib '%s', _init() return 0x%x\n", pIElf->lib->name, result );
			if ( result != LDR_RESULT_OK )
			{
				cprintf_int( "Lib '%s', _init() return 0x%x\n", pIElf->lib->name, result );
				result = LDR_RESULT_ERROR_LIB_INIT_FAILED;
				break;
			}
		}
		else
			dbgf( "Lib '%s', _init() not find\n", pIElf->lib->name );
		
		pIElf->lib->refs++;
		
		*plib = pIElf->lib;
		dbgf( "plib = 0x%x, lib = 0x%x\n", plib, *plib );
	}
	while (0);
	
	if ( result != LDR_RESULT_OK )
	{
		#ifndef DEBUG
		u_nutoa( uri, buf, 48 );
		#endif
		cprintf_int( "\x8C""Loading Lib '%s' failed, error #%x\n", buf, result );
	}
	
	if ( pIElf )
	{
		clearElf( pIElf, result != LDR_RESULT_OK );
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// finish processing ELF as library
// окончательная обработка эльфа как общей библиотеки
// status: done
// TODO: хранить в sym[i].st_name сразу адрес строки (DONE)
// Library format:
// header{Ldr_Lib}[1]
// symTab{Ldr_Sym}[DT_NONABI_SYMTABNO]
// strTab{char}[DT_STRSZ]
//
UINT32	prepareLibrary( Ldr_IElf * pIElf, const WCHAR * name )
{
	UINT32				result = LDR_RESULT_OK;
	Ldr_Sym				*ldrSym;
	INT32				err;
	UINT32				i;
	
	dbgf( "enter, 0x%x\n", &i );
	/*dbgf( "enter, DT_NONABI_SYMTABNO=%d, DT_STRSZ=%d\n", 
			pIElf->dynTags[DT_NONABI_SYMTABNO], pIElf->dynTags[DT_STRSZ] );*/
	
	pIElf->lib = (Ldr_Lib*)suAllocMem( sizeof(Ldr_Lib) + 
			(pIElf->dynTags[DT_NONABI_SYMTABNO] - 1) * sizeof(Ldr_Sym) + 
				pIElf->dynTags[DT_STRSZ], &err );
	dbgf( "alloc lib=0x%x\n", pIElf->lib );
	
	if ( pIElf->lib )
	{
		u_nutoa( name, pIElf->lib->name, LIBNAME_MAXLEN );
		pIElf->lib->count = pIElf->dynTags[DT_NONABI_SYMTABNO] - 1;
		pIElf->lib->refs = 0;
		pIElf->lib->link = pIElf->link;
		
		pIElf->lib->strTab = (char*)(pIElf->lib + 1) + 
				(pIElf->dynTags[DT_NONABI_SYMTABNO] - 1) * sizeof(Ldr_Sym);
		
		__rt_memcpy( pIElf->lib->strTab, pIElf->strTable, pIElf->dynTags[DT_STRSZ] );
		
		ldrSym = (Ldr_Sym*)(pIElf->lib + 1);
		for ( i=1; i < pIElf->dynTags[DT_NONABI_SYMTABNO]; i++ )
		{
			//ldrSym[i-1].st_name = pIElf->dynSymTable[i].st_name;
			ldrSym[i-1].st_name = 
					(UINT32)&pIElf->lib->strTab[ pIElf->dynSymTable[i].st_name ];
			ldrSym[i-1].st_value = pIElf->dynSymTable[i].st_value;
		}
		
#ifdef DUMP_ELF
		dump_elf( (void*)pIElf->physBase, pIElf->imageSize, name );
#endif
		
		pIElf->lib->physBase = (void*)pIElf->physBase;
		
		//pIElf->lib->_fini = (void*)VIRT2PHYS( *pIElf, pIElf->dynTags[DT_FINI] );
	}
	else
	{
		result = LDR_RESULT_ERROR_MEM_ALLOC;
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return LDR_RESULT_OK;
}


// free mem from library and linked resources
// освобождение памяти из под либы и связанных ресурсов
//
UINT32	unloadLibrary( Ldr_Lib * lib )
{
	UINT32				result = LDR_RESULT_OK;
	
	dbgf( "enter, lib=0x%X\n", lib );
	
	if ( lib )
	{
		lib->refs--;
		dbgf( "refs = %d\n", lib->refs );
		
		if ( lib->refs == 0 )
		{
			dbgf( "free library\n", lib );
			result = freeLibrary( lib );
		}
	}
	else
		result = LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


typedef UINT32 FINI_FUNC_T( void );

// free mem from library and linked resources
// освобождение памяти из под либы и связанных ресурсов
//
UINT32	freeLibrary( Ldr_Lib * lib )
{
	UINT32			result = LDR_RESULT_OK;
	FINI_FUNC_T		*_fini;
	
	if ( !lib )
		return LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	/* FINI */
	//_fini = (INIT_FUNC_T*)lib->_fini;
	_fini = (FINI_FUNC_T*)findSymbol( lib, "_fini" );
	if ( _fini )
	{
		result = _fini();
		dbgf( "Lib '%s', _fini() return 0x%x\n", lib->name, result );
		if ( result != LDR_RESULT_OK )
		{
			cprintf_int( "Lib '%s', _fini() return 0x%x\n", lib->name, result );
			result = LDR_RESULT_ERROR_LIB_FINI_FAILED;
		}
	}
	else
		dbgf( "Lib '%s', _fini() not find\n", lib->name );
	
	unloadLibs( lib->link );
	
	if ( lib->physBase )
	{
		dbgf( "free physBase=0x%x\n", lib->physBase );
		suFreeMem( (void*)lib->physBase );
	}
	
	dbgf( "free lib=0x%x\n", lib );
	suFreeMem( (void*)lib );
	
	return result;
}


// find symbol by name
// ищет символ по имени
// status: DONE
// TODO: хранить в sym[i].st_name сразу адрес строки (DONE)
//
UINT32	findSymbol( Ldr_Lib * lib, const char * name )
{
	UINT32		i;
	Ldr_Sym		*sym;
	
	sym = (Ldr_Sym*)(lib + 1); //Символы следуют сразу за заголовком
	
	for ( i=0; i < lib->count; i++ )
	{
		//dbgf( "test '%s'=%x\n", sym[i].st_name, sym[i].st_value );
		//if ( namecmp( &lib->strTab[ sym[i].st_name ], name ) )
		if ( namecmp( (char*)sym[i].st_name, name ) )
			return sym[i].st_value;
	}
	
	return NULL;
}


/* EOF */
