#ifndef __ELFLOADER_H__
#define __ELFLOADER_H__

#include <typedefs.h>
#include "features.h"
#include "elf.h"
#include "error_codes.h"


#define EVBASE_RESERVE		0x40


#define MAX_LIBRARIES		24
#define LIBNAME_MAXLEN		31

typedef struct
{
	char				name[LIBNAME_MAXLEN+1];
	UINT32				count;
	UINT32				refs;
	UINT32				link;
	char				*strTab;
	void				*physBase;
	//void				*_fini;
	
} Ldr_Lib;

typedef Ldr_Lib * DLHANDLE;


#define MAX_HEADERS		4
#define MAX_NEEDED		6

typedef struct
{
	Elf32_Ehdr 			elfHeader;
	Elf32_Phdr			elfProgramHeaders[MAX_HEADERS];
	Elf32_Addr			virtBase, physBase, dynSegment;
	UINT32				imageSize;
	Elf32_Phdr			*pDynPH;
	Elf32_Word			dynTags[DT_MAX_TAG+1];
	UINT32				link;
	UINT32				neededCnt;
	Ldr_Lib				*needed[MAX_NEEDED];
	char				*strTable;
	Elf32_Sym			*dynSymTable;
	Ldr_Lib				*lib;					// Ldr_Lib + SymTable + StrTable
} Ldr_IElf;

#define VIRT2PHYS(IElf, addr) ( (UINT32)((IElf).physBase) + (UINT32)(addr) - (UINT32)((IElf).virtBase) )

#define DEFLIB_VERSION_LEN 7
#define DEFLIB_FIRMWARE_LEN 23
typedef struct
{
	UINT32		magic;
	char		ver[DEFLIB_VERSION_LEN+1];
	char		firmware[DEFLIB_FIRMWARE_LEN+1];
	UINT32		symCnt;
	UINT32		strTabSz;
	UINT32		strTabOff;
	UINT32		constCnt;
	UINT32		constOff;
	
} Ldr_DefLibHdr; /*56=0x38*/


typedef struct 
{
	UINT16		*ID;
	UINT32		*Vals;
	UINT32		count;
	
} CONST_LIBRARY_T;


typedef struct tagldrElf
{
	char						*name;	// name of the ELF
	UINT32						evbase;	// event code base (was in Register before)
	struct tagAPPLICATION_T		*app;	// APPLICATION_T pointer
	void						*addr;	// address of ELF image in the memory
	UINT32						size;	// size of the ELF image
	UINT32						link;	// linked libraries
	WCHAR						*dir;	// working dir
	struct tagldrElf			*next;	// list pointer
} ldrElf;


extern Ldr_Lib			*fnLibraries[MAX_LIBRARIES];
extern UINT32			fnLibCount;
extern CONST_LIBRARY_T	cstLibrary;


typedef ldrElf * (*ElfEntry)( const WCHAR *, const WCHAR * );


#ifdef __cplusplus
extern "C" {
#endif


UINT32	loadElf( const WCHAR * uri, const WCHAR * params );
UINT32	unloadElf( void * addr );

UINT32	loadLibrary( const WCHAR * uri, Ldr_Lib ** plib, int mode );
UINT32	unloadLibrary( Ldr_Lib * lib );
UINT32	findSymbol( Ldr_Lib * lib, const char * name );
UINT32	findLibrary( const WCHAR * name, const WCHAR * cur_dir, WCHAR * path );

UINT32	loadDefaultLib( const WCHAR * uri );
UINT32	unloadDefaultLib( void );
BOOL	isMainLibraryLoaded( void );

/* internal use */
UINT32	prepareElf( const WCHAR * uri, Ldr_IElf * pIElf, Elf32_Half e_type );
UINT32	readElf( const WCHAR * uri, Ldr_IElf * pIElf, Elf32_Half e_type );
UINT32	clearElf( Ldr_IElf * pIElf, BOOL full );
UINT32	freeElf( ldrElf * elf );
UINT32	registerLibrary( Ldr_Lib * lib, UINT32 * ind );
UINT32	unloadLibs( UINT32 link );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* __ELFLOADER_H__ */
