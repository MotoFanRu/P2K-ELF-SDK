
#include "elfloader.h"
#include <mem.h>
#include <filesystem.h>
#include <utilities.h>
//#include "elfloaderApp.h"
#include "console.h"
#include "utils.h"
#include "config.h"
#include "API.h"
#include "elf.h"
#include "features.h"
#include "dbg.h"


extern Ldr_Lib		patchLib;


// load default library {library.bin}
// загрузить стандартную библиотеку {library.bin}
// staus: done
// TODO: хранить в sym[i].st_name сразу адрес строки (DONE)
// DefLibrary format:
// header{Ldr_DefLibHdr}[1]
// symTab{Ldr_Sym}[symCnt]
// +-<strTabOff
// strTab{char}[strTabSz]
// +-<constOff
// cstLibrary.ID{UINT16}[constCnt]
// cstLibrary.Vals{UINT32}[constCnt]
//
UINT32	loadDefaultLib( const WCHAR * uri )
{
	UINT32				result = LDR_RESULT_OK;
	FILE_HANDLE_T		f;
	UINT8				fresult;
	UINT32				i, count, size;
	INT32				err;
	Ldr_DefLibHdr		lHdr;
	Ldr_Lib				*lib = NULL;
	char				*strTable;
	Ldr_Sym				*ldrSym;
	
	dbg( "enter\n" );
	cprint_int("\x89""Loading library...\n");
	
	if ( fnLibraries[0] )
		unloadDefaultLib();
	
	//fnLibraries[0] = NULL;
	fnLibraries[1] = &patchLib;
	
	if ( !uri )
		uri = Config.DefLibraryPath;
	
	do
	{
		f = DL_FsOpenFile( uri, FILE_READ_MODE, 0 );
		if ( f == FILE_HANDLE_INVALID )
		{
			cprint_int("\x8C""Library can't open file\n");
			result = LDR_RESULT_ERROR_FILE_OPEN;
			break;
		}
		
		dbgf( "file = 0x%X, size = %i\n", f, DL_FsGetFileSize(f) );
		
		// Прочитаем заголовок файла
		fresult = DL_FsReadFile( (void*)&lHdr, sizeof(Ldr_DefLibHdr), 1, f, &count );
		if ( fresult != 0 || count < 1 ) 
		{
			cprint_int("\x8C""Library can't read file\n");
			result = LDR_RESULT_ERROR_FILE_READ;
			break;
		}
		
		if ( lHdr.magic != 0x7F4C4942 )
		{
			cprint_int("\x8C""Library is corrupt\n");
			result = LDR_RESULT_ERROR_FORMAT;
			break;
		}
		
		if ( !namecmp( lHdr.firmware, product_version ) )
		{
			cprint_int("\x8C""Library firmware mismatch\n");
			result = LDR_RESULT_ERROR_FORMAT;
			break;
		}
		
		// Размер библиотеки функций
		size = lHdr.constOff - sizeof(Ldr_DefLibHdr);
		lib = (Ldr_Lib*)suAllocMem( sizeof(Ldr_Lib) + size, &err );
		dbgf( "alloc deflib=0x%x\n", lib );
		if ( !lib )
		{
			cprint_int("\x8C""Library can't alloc mem\n");
			result = LDR_RESULT_ERROR_MEM_ALLOC;
			break;
		}
		
		dbgf( "symCnt = %i, lib = 0x%X\n", lHdr.symCnt, lib );
		
		strTable = (char*)(lib + 1) + lHdr.strTabOff - sizeof(Ldr_DefLibHdr);
		
		strncpy( lib->name, "DefLib", LIBNAME_MAXLEN );
		//lib->name[0] = 0;
		lib->count = lHdr.symCnt;
		lib->refs = 0;
		lib->strTab = strTable;
		
		fresult = DL_FsReadFile( (void*)(lib + 1), size, 1, f, &count );
		if ( fresult != 0 || count < 1 )
		{
			cprint_int("\x8C""Library can't read file\n");
			result = LDR_RESULT_ERROR_FILE_READ;
			break;
		}
		
		ldrSym = (Ldr_Sym*)(lib + 1);
		for ( i=0; i < lHdr.symCnt; i++ )
		{
			ldrSym[i].st_name = (UINT32)&strTable[ ldrSym[i].st_name ];
		}
		
		// Библиотека констант
		
		cstLibrary.ID = NULL;
		cstLibrary.Vals = NULL;
		cstLibrary.count = lHdr.constCnt;
		
		if ( lHdr.constCnt > 0 )
		{
			cstLibrary.ID = (UINT16*)suAllocMem( lHdr.constCnt * sizeof(UINT16), &err );
			dbgf( "alloc ID=0x%x\n", cstLibrary.ID );
			if ( !cstLibrary.ID )
			{
				cprint_int("\x8C""CONST library can't alloc mem\n");
				result = LDR_RESULT_ERROR_MEM_ALLOC;
				break;
			}
			
			fresult = DL_FsReadFile( (void*)(cstLibrary.ID), sizeof(UINT16), lHdr.constCnt, f, &count );
			if ( fresult != 0 || count < lHdr.constCnt )
			{
				cprint_int("\x8C""CONST library can't read file\n");
				result = LDR_RESULT_ERROR_FILE_READ;
				break;
			}
			
			cstLibrary.Vals = (UINT32*)suAllocMem( lHdr.constCnt * sizeof(UINT32), &err );
			dbgf( "alloc Vals=0x%x\n", cstLibrary.Vals );
			if ( !cstLibrary.Vals )
			{
				cprint_int("\x8C""CONST library can't alloc mem\n");
				result = LDR_RESULT_ERROR_MEM_ALLOC;
				break;
			}
			
			fresult = DL_FsReadFile( (void*)(cstLibrary.Vals), sizeof(UINT32), lHdr.constCnt, f, &count );
			if ( fresult != 0 || count < lHdr.constCnt )
			{
				cprint_int("\x8C""CONST library can't read file\n");
				result = LDR_RESULT_ERROR_FILE_READ;
				break;
			}
		}
		
		cprintf("\x89""Library \x82%s\x89 loaded:\n  \x8E%d\x85 functions\x89 and \x8E%d\x85 consts\n", lHdr.ver, lHdr.symCnt, lHdr.constCnt);
		
	}
	while (0);
	
	if ( f != FILE_HANDLE_INVALID )
		DL_FsCloseFile( f );
	
	fnLibraries[0] = lib;
	
	dbgf( "exit, 0x%x\n", result );
	
	if ( result != LDR_RESULT_OK )
	{
		unloadDefaultLib();
		fnLibCount = 0;
	}
	else
	{
		fnLibCount = 2;
	}
	
	//dbgf( "fnLibraries[0] = 0x%x\n", fnLibraries[0] );
	
	return result;
}


// unload default library (library.bin)
// выгрузка стандартной библиотеки
// status: done
//
UINT32	unloadDefaultLib( void )
{
	if ( fnLibraries[0] ) 
	{
		dbgf( "free deflib=0x%X\n", fnLibraries[0] );
		suFreeMem( fnLibraries[0] );
		fnLibraries[0] = NULL;
	}
	
	if ( cstLibrary.ID ) 
	{
		dbgf( "free ID=0x%X\n", cstLibrary.ID );
		suFreeMem( cstLibrary.ID );
		cstLibrary.ID = NULL;
	}
	
	if ( cstLibrary.Vals ) 
	{
		dbgf( "free Vals=0x%X\n", cstLibrary.Vals );
		suFreeMem( cstLibrary.Vals );
		cstLibrary.Vals = NULL;
	}
	
	cstLibrary.count = 0;
	
	return LDR_RESULT_OK;
}


// check of Def Library is loaded
// проверка: загружена ли стандартная библиотека функций
//
BOOL	isMainLibraryLoaded( void )
{
	return fnLibraries[0] != NULL;
}


/* EOF */
