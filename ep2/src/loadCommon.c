
#include "elfloader.h"
#include <mem.h>
#include <filesystem.h>
#include <utilities.h>
#include "console.h"
#include "utils.h"
#include "config.h"
#include "API.h"
#include "elf.h"
#include "features.h"
#include "dbg.h"


Ldr_Lib				*fnLibraries[MAX_LIBRARIES];
UINT32				fnLibCount;

CONST_LIBRARY_T		cstLibrary;


static UINT32	checkElf( Ldr_IElf * pIElf, Elf32_Half e_type );
static UINT32	calcImageSize( Ldr_IElf * pIElf );
static UINT32	prepareDynSegment( Ldr_IElf * pIElf );
static UINT32	relocatePLT( Ldr_IElf * pIElf );
static UINT32	relocateDyn( Ldr_IElf * pIElf );
static UINT32	linkLibraries( Ldr_IElf * pIElf, const WCHAR * cur_dir );
static UINT32	findFunction( const char * name, Ldr_Lib ** needed );


// read ELF file and prepare image for execute or common library
// загрузка ELF-файла и подготовка образа (релокации, подключение либ)
// status: DONE
// TODO: cur_dir to heap (DONE)
//
UINT32	prepareElf( const WCHAR * uri, Ldr_IElf * pIElf, Elf32_Half e_type )
{
	UINT32				result = LDR_RESULT_OK;
	WCHAR				*cur_dir = NULL;
	INT32				err;
	UINT32				i;
	
	dbgf( "enter, 0x%x\n", &i );
	
	pIElf->needed[0] = fnLibraries[0];	// Всегда сперва ищем в стд. библиотеке
	pIElf->needed[1] = fnLibraries[1];	// Затем - в библиотеке патча
	pIElf->neededCnt = 2;				// default library and patchLib
	
	do
	{
		result = readElf( uri, pIElf, e_type );
		if ( result != LDR_RESULT_OK )
			break;
		
		result = prepareDynSegment( pIElf );
		if ( result != LDR_RESULT_OK )
			break;
		
		/************** LOAD NEEDED **************/
		
		//DL_FsSGetVolume( uri, cur_dir );
		//DL_FsSGetPath( uri, cur_dir + u_strlen( cur_dir ) ) ;
		
		if ( pIElf->neededCnt > 2 )
		{
			cur_dir = suAllocMem( FS_MAX_URI_NAME_LENGTH + 1, &err );
			dbgf( "alloc cur_dir=0x%x\n", cur_dir );
			if ( !cur_dir )
			{
				result = LDR_RESULT_ERROR_MEM_ALLOC;
				break;
			}
			
			u_strcpy( cur_dir, uri );
			i = u_strlen( cur_dir ) - 1;
			while ( i > 0 && cur_dir[i] != L'/' ) i--;
			if ( i > 0 ) cur_dir[i] = 0;
			
			result = linkLibraries( pIElf, cur_dir );
			if ( result != LDR_RESULT_OK )
				break;
		}
		
		/**************** RELOCATIONS ******************/
		
		result = relocatePLT( pIElf );
		if ( result != LDR_RESULT_OK )
			break;
		
		result = relocateDyn( pIElf );
		if ( result != LDR_RESULT_OK )
			break;
		
	}
	while (0);
	
	if ( cur_dir )
	{
		dbgf( "free cur_dir=0x%X\n", cur_dir );
		suFreeMem( cur_dir );
	}
	
	if ( result != LDR_RESULT_OK )
	{
		clearElf( pIElf, TRUE );
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// read ELF file and allocate buffers
// чтение ELF-файла с выделением памяти под буфферы
// status: DONE
//
UINT32 readElf( const WCHAR * uri, Ldr_IElf * pIElf, Elf32_Half e_type )
{
	UINT32				result = LDR_RESULT_OK;
	FILE_HANDLE_T		f;
	UINT8				fresult;
	UINT32				i, count;
	Elf32_Phdr			*pPH;
	INT32				err;
	Elf32_Addr			addr;
	//char				buf[48+1];
	
	//u_nutoa( uri, buf, 48 );
	//dbgf( "enter, uri = '%s', e_type = %d\n", buf, e_type );
	//dbgf( "enter, e_type = %d\n", e_type );
	dbgf( "enter, 0x%x\n", &addr );
	
	do
	{
		// Open ELF File
		f = DL_FsOpenFile( uri, FILE_READ_MODE, 0 );
		if ( f == FILE_HANDLE_INVALID )
		{
			result = LDR_RESULT_ERROR_FILE_OPEN;
			break;
		}
		
		/************ ELF HEADER ***********/
		
		// Read ELF Header
		fresult = DL_FsReadFile( (void*)&pIElf->elfHeader, sizeof(Elf32_Ehdr), 1, f, &count );
		if ( fresult != 0 || count < 1 )
		{
			result = LDR_RESULT_ERROR_FILE_READ;
			break;
		}
		
		/*dbgf("%s loading...\nELF header:\n  e_entry  0x%X\n  e_phoff  0x%X\n  e_phnum  %d\n",
					buf,
					pIElf->elfHeader.e_entry,
					pIElf->elfHeader.e_phoff,
					pIElf->elfHeader.e_phnum );*/
		
		// check for correct ELF-file
		result = checkElf( pIElf, e_type );
		if ( result != LDR_RESULT_OK )
			break;
		
		/************ PROGRAM HEADERS ***********/
		
		// Seek to Program Headers
		fresult = DL_FsFSeekFile( f, pIElf->elfHeader.e_phoff, SEEK_WHENCE_SET );
		if ( fresult != 0 )
		{
			result = LDR_RESULT_ERROR_FILE_SEEK;
			break;
		}
		
		// Read Program Headers
		fresult = DL_FsReadFile( (void*)pIElf->elfProgramHeaders, 
				sizeof(Elf32_Phdr), pIElf->elfHeader.e_phnum, f, &count );
		if ( fresult != 0 || count < pIElf->elfHeader.e_phnum )
		{
			result = LDR_RESULT_ERROR_FILE_READ;
			break;
		}
		
		/*********** IMAGE SIZE ***********/
		
		result = calcImageSize( pIElf );
		if ( result != LDR_RESULT_OK )
			break;
		
		pIElf->physBase = (Elf32_Addr)suAllocMem( pIElf->imageSize, &err );
		dbgf( "alloc physBase=0x%x\n", pIElf->physBase );
		if ( !pIElf->physBase )
		{
			result = LDR_RESULT_ERROR_MEM_ALLOC;
			break;
		}
		
		//dbgf( "Physical Base  0x%X\n", pIElf->physBase );
		
		/************ PROCESS PROGRAM HEADERS ***********/
		
		for ( i=0; i < pIElf->elfHeader.e_phnum; i++ )
		{
			pPH = &pIElf->elfProgramHeaders[i];
			
			switch ( pIElf->elfProgramHeaders[i].p_type )
			{
				/************ LOAD SEGMENT ***********/
				case PT_LOAD:
				{
					fresult = DL_FsFSeekFile( f, pPH->p_offset, SEEK_WHENCE_SET );
					if ( fresult != 0 )
					{
						result = LDR_RESULT_ERROR_FILE_SEEK;
						break;
					}
					
					addr = VIRT2PHYS( *pIElf, pPH->p_vaddr );
					fresult = DL_FsReadFile( (void*)addr, pPH->p_filesz, 1, f, &count );
					if ( fresult != 0 || count < 1 )
					{
						result = LDR_RESULT_ERROR_FILE_READ;
						break;
					}
					
					if ( pPH->p_memsz > pPH->p_filesz )
						__rt_memclr( (void*)(addr + pPH->p_filesz), 
										pPH->p_memsz - pPH->p_filesz );
				
					/*dbgf( "  Segment #%d loading  0x%X  0x%d\n\n",
						 i,
						 pIElf->physBase + pPH->p_vaddr - pIElf->virtBase,
						 pPH->p_filesz );*/
					
					break;
				}
				
				/************ DYNAMIC SEGMENT ***********/
				case PT_DYNAMIC:
				{
					pIElf->dynSegment = (Elf32_Addr)suAllocMem( pPH->p_filesz, &err );
					dbgf( "alloc dynSegment=0x%x\n", pIElf->dynSegment );
					if ( !pIElf->dynSegment )
					{
						result = LDR_RESULT_ERROR_MEM_ALLOC;
						break;
					}
					
					fresult = DL_FsFSeekFile( f, pPH->p_offset, SEEK_WHENCE_SET );
					if ( fresult != 0 )
					{
						result = LDR_RESULT_ERROR_FILE_SEEK;
						break;
					}
					
					fresult = DL_FsReadFile( (void*)pIElf->dynSegment, pPH->p_filesz, 1, f, &count );
					if ( fresult != 0 || count < 1 )
					{
						result = LDR_RESULT_ERROR_FILE_READ;
						break;
					}
					
					pIElf->pDynPH = pPH;
					
					break;
				}
				
			} // switch ( pIElf->elfProgramHeaders[i].p_type )
			
			if ( result != LDR_RESULT_OK )
				break;
			
		} // for( i=0; i < pIElf->elfHeader.e_phnum; i++ )
	
	}
	while (0);
	
	if ( f != FILE_HANDLE_INVALID )
		DL_FsCloseFile( f );
	
	if ( result != LDR_RESULT_OK )
	{
		clearElf( pIElf, TRUE );
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// free temp buffers and main buffers if full=TRUE
// освобождает временные буферы и главные буферы если full=TRUE
// status: DONE
//
UINT32 clearElf( Ldr_IElf * pIElf, BOOL full )
{
	if ( !pIElf ) return LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	if ( pIElf->dynSegment )
	{
		dbgf( "free dynSegment=0x%X\n", pIElf->dynSegment );
		suFreeMem( (void*)pIElf->dynSegment );
		pIElf->dynSegment = NULL;
	}
	
	if ( full )
	{
		unloadLibs( pIElf->link );
		
		if ( pIElf->physBase )
		{
			dbgf( "free physBase=0x%X\n", pIElf->physBase );
			suFreeMem( (void*)pIElf->physBase );
			pIElf->physBase = NULL;
		}
		
		if ( pIElf->lib ) 
		{
			dbgf( "free lib=0x%X\n", pIElf->lib );
			suFreeMem( (void*)pIElf->lib );
			pIElf->lib = NULL;
		}
	}
	
	return LDR_RESULT_OK;
}


// check ELF file for correct format
// проверяет эльф на корректность формата (а вдруг левое подсунут =))
//
UINT32 checkElf( Ldr_IElf * pIElf, Elf32_Half e_type )
{
	if ( !(	*((UINT32*)pIElf->elfHeader.e_ident) == 0x7F454c46 &&	// '.ELF'
			pIElf->elfHeader.e_ident[4] == ELFCLASS32 &&			// 32 bits
			pIElf->elfHeader.e_ident[5] == ELFDATA2MSB &&			// BigEndian
			pIElf->elfHeader.e_ident[6] == EV_CURRENT &&			// default
			pIElf->elfHeader.e_type == e_type						// ET_DYN or ET_EXEC
		) ) return LDR_RESULT_ERROR_FORMAT;
	
	if ( pIElf->elfHeader.e_phnum >= MAX_HEADERS )
		return LDR_RESULT_ERROR_PHNUM_MUCH;
	
	dbgf( "e_flags=%x, eabi=%x\n", pIElf->elfHeader.e_flags, 
			pIElf->elfHeader.e_flags & EF_ARM_EABIMASK );
	
	if ( (pIElf->elfHeader.e_flags & EF_ARM_EABIMASK) == 0x02000000 )
		return LDR_RESULT_ERROR_EABI_VERSION2;
	
	return LDR_RESULT_OK;
}


// calc size of ELF image
// Подсчитаем размер образа
// status: DONE
//
UINT32 calcImageSize( Ldr_IElf * pIElf )
{
	Elf32_Phdr			*pPH;
	Elf32_Addr			upperAddr = 0, addr;
	UINT32				sumMem=0, sumSize=0;
	UINT32				i;
	
	pIElf->virtBase = (Elf32_Addr)(-1);
	
	for ( i=0; i < pIElf->elfHeader.e_phnum; i++ )
	{
		pPH = &pIElf->elfProgramHeaders[i];
		
		/*dbgf("Segment #%d:\n  p_type  %d\n  p_vaddr  0x%X\n  p_memsz  0x%X\n",
				i,
				pPH->p_type,
				pPH->p_vaddr,
				pPH->p_memsz );*/
		
		if ( pPH->p_type == PT_LOAD )
		{
			if ( pPH->p_vaddr < pIElf->virtBase )
				pIElf->virtBase = pPH->p_vaddr;
			
			addr = pPH->p_vaddr + pPH->p_memsz;
			if( addr > upperAddr )
				upperAddr = addr;
		}
		
		sumMem += pPH->p_memsz;
		sumSize += pPH->p_filesz;
	}
	
	/*dbgf("\nFinished analysis:\n  Virtual Base  0x%X\n  Mem Needed  0x%X\n  Upper Address  0x%X\n  Summary Mem  0x%X\n  Summary Size  0x%X\n\n",
					pIElf->virtBase,
					upperAddr - pIElf->virtBase,
					upperAddr,
					sumMem,
					sumSize);*/
	
	pIElf->imageSize = upperAddr - pIElf->virtBase; //sumMem;
	
	return LDR_RESULT_OK;
}


// process DYN segment
// подготовка сегмента DYN
// status: DONE
//
UINT32 prepareDynSegment( Ldr_IElf * pIElf )
{
	Elf32_Dyn			tag;
	UINT32				i, j;
	
	dbgf( "enter, 0x%x\n", &j );
	//dbgf(" dynSegment 0x%X sz: %d\n", pIElf->dynSegment, pIElf->elfProgramHeaders[i].p_filesz);
	
	//idxPHDyn = i;
	
	// Действительно ли это нужно? t.a.: в общем случае надо
	__rt_memclr( pIElf->dynTags, (DT_MAX_TAG+1)*4 );
	
	// Analyse Tags
	j = 0;
	while (1)
	{
		tag = ((Elf32_Dyn*)pIElf->dynSegment)[ j++ ];
		if ( !tag.d_tag ) break;
		
		if ( tag.d_tag == DT_NEEDED && pIElf->neededCnt < MAX_NEEDED )
		{
			dbgf( "DT_NEEDED: %i, d_val=%x\n", pIElf->neededCnt, tag.d_val );
			
			// пока запоминаем имена либ (смещения в strTable)
			pIElf->needed[ pIElf->neededCnt++ ] = (Ldr_Lib*)tag.d_val;
		}
		else
		{
			pIElf->dynTags[ tag.d_tag ] = tag.d_val;
		}
	}
	
	// Mark end
	pIElf->needed[ pIElf->neededCnt ] = NULL;
	
	pIElf->dynSymTable = (Elf32_Sym*)(pIElf->dynSegment + pIElf->dynTags[DT_SYMTAB] - 
			pIElf->pDynPH->p_offset);
	
	pIElf->strTable = (char*)(pIElf->dynSegment + pIElf->dynTags[DT_STRTAB] - 
			pIElf->pDynPH->p_offset);
	
	// пока запоминаем имена либ (указатели на строки)
	for ( i=2; i < pIElf->neededCnt; i++ )
	{
		pIElf->needed[i] = (Ldr_Lib*)&pIElf->strTable[ (UINT32)pIElf->needed[i] ];
		dbgf( "DT_NEEDED: %i, name='%s'\n", i, pIElf->needed[i] );
	}
	
	// Если загружаем динамическую библиотеку - делаем релокацию R_ARM_RELATIVE для всех локальных символов
	// Нужно для R_ARM_ABS32
	if ( pIElf->elfHeader.e_type == ET_DYN )
	{
		for ( i=1; i < pIElf->dynTags[DT_NONABI_SYMTABNO]; i++ )
		{
			//pIElf->dynSymTable[i].st_value += pIElf->physBase - pElf->virtBase;
			pIElf->dynSymTable[i].st_value = 
					VIRT2PHYS( *pIElf, pIElf->dynSymTable[i].st_value );
		}
	}
	
	dbgf( "exit, 0x%x\n", 0 );
	
	return LDR_RESULT_OK;
}


// process relocation of PLT
// провести релокацию PLT
// status: done
//
UINT32 relocatePLT( Ldr_IElf * pIElf )
{
	UINT32				result = LDR_RESULT_OK;
	Elf32_Rel			*relTable;
	Elf32_Word			relType, relSym;
	Elf32_Sym			*importsSymTable, *sym;
	UINT32				i;
	
	dbgf( "enter, 0x%x\n", &i );
	/*dbgf("PLT Relocation: DT_JMPREL=0x%X, DT_PLTRELSZ=%d, DT_NONABI_IMPORTS=0x%X\n",
		pIElf->dynTags[DT_JMPREL], pIElf->dynTags[DT_PLTRELSZ], 
		pIElf->dynTags[DT_NONABI_IMPORTS] );*/
	
	// Совершаем релокации с импортами (после разделения импортов - только в .rel.plt)
	if ( pIElf->dynTags[DT_JMPREL] == 0 )
		return LDR_RESULT_OK;
	
	relTable = (Elf32_Rel*)(pIElf->dynSegment + pIElf->dynTags[DT_JMPREL] - 
			pIElf->pDynPH->p_offset);
	
	importsSymTable = (Elf32_Sym*)(pIElf->dynSegment + 
			pIElf->dynTags[DT_NONABI_IMPORTS] - pIElf->pDynPH->p_offset);
	
	i = 0;
	while ( i < pIElf->dynTags[DT_PLTRELSZ]/sizeof(Elf32_Rel) )
	{
		relType = ELF32_R_TYPE( relTable[i].r_info );
		relSym = ELF32_R_SYM( relTable[i].r_info );

		sym = &importsSymTable[ relSym ];

		/*dbgf( "PLT Reloc #%d, Type=%d, Off=0x%X\n", i, 
				relType, relTable[i].r_offset);
		
		dbgf( "Symbol import: [%x]='%s'\n", sym->st_name, 
				&pIElf->strTable[ sym->st_name ] );*/
		
		sym->st_value = findFunction( &pIElf->strTable[ sym->st_name ], 
				pIElf->needed );
		if ( sym->st_value == NULL )
		{
			dbgf( "FAILED to import symbol! '%s'\n", &pIElf->strTable[ sym->st_name ] );
			cprintf_int( "\x8C""Import failed: %s\n", &pIElf->strTable[sym->st_name] );
			result = LDR_RESULT_ERROR_RELOC_NOT_FIND;
			break;
		}
		
		/*dbgf( " R_ARM_JUMP_SLOT or R_ARM_ABS32\n  Old  0x%X\n  New  0x%X\n",
				*( (UINT32*)VIRT2PHYS( *pIElf, relTable[i].r_offset ) ),
				sym->st_value );*/
		
		*( (UINT32*)VIRT2PHYS( *pIElf, relTable[i].r_offset ) ) = sym->st_value;
		
		i++;
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// process local relocations
// провести релокацию DYN (внутренние)
// status: done
//
UINT32 relocateDyn( Ldr_IElf * pIElf )
{
	UINT32				result = LDR_RESULT_OK;
	Elf32_Rel			*relTable;
	Elf32_Word			relType, relSym;
	Elf32_Sym			*sym;
	UINT32				*addr;
	UINT32				i;
	
	dbgf( "enter, 0x%x\n", &i );
	/*dbgf( "DYN Relocation: DT_REL=0x%X, DT_RELSZ=%d\n",
			pIElf->dynTags[DT_REL], pIElf->dynTags[DT_RELSZ] );*/
	
	// Совершаем релокации с внутренними символами (после разделения импортов - только в .rel.dyn)
	if ( pIElf->dynTags[DT_REL] == 0 )
		return LDR_RESULT_OK;
	
	relTable = (Elf32_Rel*)(pIElf->dynSegment + pIElf->dynTags[DT_REL] - 
			pIElf->pDynPH->p_offset);
	
	i = 0;
	while ( i < pIElf->dynTags[DT_RELSZ]/sizeof(Elf32_Rel) )
	{
		relType = ELF32_R_TYPE( relTable[i].r_info );
		relSym = ELF32_R_SYM( relTable[i].r_info );
		
		sym = &pIElf->dynSymTable[ relSym ];
		
		/*dbgf( " Reloc #%d\n  Type  %d\n  Off  0x%X\n",
				i,
				relType,
				relTable[i].r_offset );*/
		
		switch ( relType )
		{
			case R_ARM_ABS32:
			{
				/*dbgf( " R_ARM_ABS32\n  addr  0x%X\n  New  0x%X\n",
						VIRT2PHYS( *pIElf, relTable[i].r_offset ),
						sym->st_value);*/
				
				addr = (UINT32*)VIRT2PHYS( *pIElf, relTable[i].r_offset );
				*addr = (UINT32)sym->st_value;
				
				break;
			}
		
			case R_ARM_RELATIVE:
			{
				/*dbgf( " R_ARM_RELATIVE\n  Old  0x%X\n  New  0x%X\n",
						*((long*)VIRT2PHYS( *pIElf, relTable[i].r_offset )),
						*((long*)VIRT2PHYS( *pIElf, relTable[i].r_offset )) + 
								pIElf->physBase - pIElf->virtBase );*/
				
				addr = (UINT32*)VIRT2PHYS( *pIElf, relTable[i].r_offset );
				*addr = VIRT2PHYS( *pIElf, *addr );
				
				break;
			}
			
			default:
			{
				dbgf( " FAILED to relocate symbol! type=%d\n", relType );
				cprint_int("\x8C""Relocate failed!\n" );
				result = LDR_RESULT_ERROR_RELOC_UNK_TYPE;
				break;
			}
		}
		
		if ( result != LDR_RESULT_OK ) break;
		
		i++;
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// load and link libraries for ELF
// подготовить и подключить необходимые библиотеки
// status: done
// TODO: lib_uri to heap (DONE)
//
UINT32 linkLibraries( Ldr_IElf * pIElf, const WCHAR * cur_dir )
{
	UINT32				result = LDR_RESULT_OK;
	UINT32				i, j;
	char				*lname;
	Ldr_Lib				*lib;
	WCHAR				*lib_uri = NULL;
	INT32				err;
	WCHAR				wname[LIBNAME_MAXLEN + 1];
	//char				buf[48+1];
	
	dbgf( "enter, 0x%x\n", &wname[0] );
	
	pIElf->link = 0;
	
	//dbgf( "needed[0] = 0x%x\n", pIElf->needed[0] );
	
	// Подгружаем динамические библиотеки
	for ( i=2; i < pIElf->neededCnt; i++ )
	{
		lname = (char*)pIElf->needed[i];
		dbgf( "Loading [%x]='%s'\n", lname, lname );
		
		// Проверяем, не загружена ли уже эта  библиотека
		for ( j=MAX_LIBRARIES-1; j > 1; j-- )
		{
			if ( fnLibraries[j] == NULL ) continue;
			if ( namecmp( fnLibraries[j]->name, lname ) )
			{
				//Библиотека найдена - добавляем указатель на неё
				
				pIElf->needed[i] = fnLibraries[j];
				fnLibraries[j]->refs++;
				pIElf->link  |= 1 << (j-2); 
				break;
			}
		}
		
		if ( j > 1 )
		{
			dbgf( "find existing: ind=%d, name=%s\n", j, fnLibraries[j]->name );
		}
		else
		{
			// Библиотека ещё не загружена. Пробуем найти
			
			lib_uri = suAllocMem( FS_MAX_URI_NAME_LENGTH + 1, &err );
			dbgf( "alloc lib_uri=0x%x\n", lib_uri );
			if ( !lib_uri )
			{
				result = LDR_RESULT_ERROR_MEM_ALLOC;
				break;
			}
			
			u_natou( lname, wname, LIBNAME_MAXLEN );
			result = findLibrary( wname, cur_dir, lib_uri );
			//u_nutoa( lib_uri, buf, 48 );
			
			if ( result == LDR_RESULT_OK )
			{
				// Путь к библиотеке найден
				
				//dbgf( "Found a library '%s'\n", buf );
				
				// Пытаемся её загрузить
				
				result = loadLibrary( lib_uri, &lib, 0 );
				if ( result != LDR_RESULT_OK )
					break;
				
				result = registerLibrary( lib, &j );
				if ( result != LDR_RESULT_OK )
				{
					unloadLibrary( lib );
					break;
				}
				
				// Удачно
				
				//fnLibraries[j]->refs++; // now in loadLibrary
				
				pIElf->needed[i] = lib;
				pIElf->link |= 1 << (j-2); 
			}
			else
			{
				// Не нашли =(
				
				dbgf( "Not found a library '%s'\n", lname );
				//result = LDR_RESULT_ERROR_LIB_NOT_FIND;
				break;
			}
			
		}	// if ( j == 1 )
		
	}	// for ( i=2; i < pIElf->neededCnt; i++ )
	
	if ( lib_uri )
	{
		dbgf( "free lib_uri=0x%X\n", lib_uri );
		suFreeMem( lib_uri );
	}
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// register a library in fnLibraries[]
// регистрация либы в глобальном списке общих либ
// status: done
//
UINT32	registerLibrary( Ldr_Lib * lib, UINT32 * ind )
{
	UINT32				i;
	
	dbgf( "enter, 0x%x\n", &i );
	
	i = 2;
	while ( i < MAX_LIBRARIES && fnLibraries[i] != NULL ) 
		i++;
	
	if ( i == MAX_LIBRARIES )
	{
		dbg( "fnLibraries[] is full!\n" );
		return LDR_RESULT_ERROR_LIB_MUCH;
	}
	
	fnLibraries[i] = lib;
	fnLibCount++;
	
	dbgf( "fnLibCount=%d\n", fnLibCount );
	
	dbgf( "ind=%x, i=%d\n", ind, i );
	if ( ind )
		*ind = i;
	
	dbgf( "exit, *ind=%d\n", i );
	
	return LDR_RESULT_OK;
}


// find function on linked libraries
// найти ф-цию среди подключенных библиотек
// status: DONE
//
UINT32	findFunction( const char * name, Ldr_Lib ** needed )
{
	UINT32		j=0, val=0;
	
	//dbgf( "enter, name=%s\n", name );
	
	if ( needed[0] == NULL ) // Stdlib is absent
		j++;
	
	while ( needed[j] != NULL )
	{
		//dbgf( "lib='%s'\n", needed[j]->name );
		
		val = findSymbol( needed[j], name );
		if ( val ) break;
		j++;
	}
	
	//dbgf( "exit, val=%x\n", val );
	
	return val;
}


// Returns the path to the specified library
// по имени либы ищет путь к ней
// status: DONE
// TODO: find first in current path (ELF folder) (DONE)
//
UINT32	findLibrary( const WCHAR * name, const WCHAR * cur_dir, WCHAR * path )
{
	UINT32		result = LDR_RESULT_OK;
	UINT32		i = 0, l;
	#ifdef DEBUG
	char		buf[64+1];
	#endif
	
	dbgf( "enter, 0x%x\n", &buf[0] );
	
	do
	{
		if ( !name || !path )
		{
			result = LDR_RESULT_ERROR_BAD_ARGUMENT;
			break;
		}
		
		#ifdef DEBUG
		u_nutoa( name, buf, 64 );
		dbgf( "Library name is '%s'\n", buf );
		#endif
		
		if ( cur_dir )
		{
			u_strncpy( path, cur_dir, FS_MAX_URI_NAME_LENGTH - 1 );
			l = u_strlen( path );
			
			#ifdef DEBUG
			u_nutoa( path, buf, 64 );
			dbgf( "Libpath cur '%s', %d\n", buf, l );
			#endif
			
			if ( path[l-1] != L'/' ) 
				path[l++] = L'/';
			
			u_strncpy( &path[l], name, FS_MAX_URI_NAME_LENGTH - l );
			
			if ( DL_FsFFileExist( path ) )
				break;
		}
		
		for ( i = 0; i < LIBPATHCNT; i++ )
		{
			u_strncpy( path, Config.LibsPaths[i], FS_MAX_URI_NAME_LENGTH - 1 );
			l = u_strlen( path );
			
			#ifdef DEBUG
			u_nutoa( Config.LibsPaths[i], buf, 64 );
			dbgf( "Libpath %d, '%s', %d\n", i, buf, l );
			#endif
			
			if ( path[l-1] != L'/' ) 
				path[l++] = L'/';
			
			u_strncpy( &path[l], name, FS_MAX_URI_NAME_LENGTH - l );
			
			if ( DL_FsFFileExist( path ) )
				break;
		}
		
		if ( i == LIBPATHCNT )
			result = LDR_RESULT_ERROR_LIB_NOT_FIND;
	}
	while ( 0 );
	
	#ifdef DEBUG
	if ( result == LDR_RESULT_OK )
	{
		u_nutoa( path, buf, 64 );
		dbgf( "Library found at '%s'\n", buf );
	}
	else
	{
		dbg( "Library not found\n" );
	}
	#endif
	
	dbgf( "exit, 0x%x\n", result );
	
	return result;
}


// unload libs associated with link
// выгрузить библиотеки, ассоциированные с link
// status: DONE
//
UINT32	unloadLibs( UINT32 link )
{
	UINT32		i, bitmask;
	Ldr_Lib		*lib;
	
	// first two libs are standard and are not controlled by the link
	
	bitmask = 1;
	for ( i = 2; i < MAX_LIBRARIES; i++ )
	{
		if ( link & bitmask )
		{
			lib = fnLibraries[i]; 
			
			if ( lib )
			{
				dbgf( "unloading lib[%d]=0x%x, refs=%d\n", i, lib, lib->refs );
				
				if ( lib->refs == 1 ) // will be free
				{
					fnLibraries[i] = NULL;
					fnLibCount--;
				}
				
				unloadLibrary( lib );
			}
			else
			{
				dbgf( "ERROR: link with unloaded lib[%d]!", i );
			}
		}
		
		bitmask <<= 1;
	}
	
	return LDR_RESULT_OK;
}


/* EOF */
