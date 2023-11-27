
#ifndef COMMON_H
#define COMMON_H

#include "typedefs.h"

#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#if defined(WIN32)
#include <conio.h>
#include <windows.h>
#endif

#include "elf.h"

//#define LILENDIAN

//-stdlib stdlib.sym -fw R373_G_0E.30.49R -v v080813

typedef struct
{
	int		shared:1;
	int		stubs:1;
	int		stdlib:1;
	char	deffile[128];
	char	version[8];
	char	firmware[24];
	char	header[64];
	char	output[64];

} CONFIG_T;

extern CONFIG_T Config;


#define INC_OA(x) Addr += (x); \
				  Off  += (x);
#define INC_O(x)  Off  += (x);
#define INC_A(x)  Addr += (x);

extern UINT32			Addr, Off;

UINT32	getFileSize(FILE *f);


typedef struct
{
	UINT32		count;
	char*		*strings;
	char		*strTable;
} STR_TABLE_T;

extern STR_TABLE_T		def;

UINT32	prepareDef( const char * filename, STR_TABLE_T * def );
UINT32	freeDef( STR_TABLE_T * def );
UINT32 findInDef( STR_TABLE_T * def, char * str );

/*---------------------------------------------*\
|			Endianness operations				|
\*---------------------------------------------*/

void switchEndian(Elf32_Ehdr *hdr);
void switchEndian(Elf32_Shdr *hdr);
void switchEndian(Elf32_Phdr *hdr);
void switchEndian(Elf32_Sym *sym);
void switchEndian(Elf32_Dyn *dyn);

#if defined(INLINE_INTEL_MSVC_ASSEMBLER_CODE)
__inline UINT64 E64( UINT64 x )
{
	__asm{
		mov	eax, [ebp+0x8]
		mov	edx, [ebp+0xC]
		xchg ah, al
		rol eax, 0x10
		xchg ah,al
		xchg edx, eax
		xchg ah, al
		rol eax, 0x10
		xchg ah,al
	}
}

__inline UINT32 E32( UINT32 x )
{
	UINT32	res;

	__asm{
		mov	eax, x
		bswap eax
//		xchg ah,al
//		rol eax, 0x10
//		xchg ah,al
		mov	 res, eax
	}
	return res;
}

__inline UINT16 E16(UINT16 x)
{
	UINT16	res;

	__asm{
		xor  eax, eax
		mov	 ax, x
		xchg ah,al
		mov  res, ax
	}
	return res;
}
#else
static inline UINT64 E64(UINT64 val) {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_bswap64(val);
#elif defined(_MSC_VER)
	return _byteswap_uint64(val);
#else
	// Fallback for other compilers
	return ((val & 0xFF00000000000000ull) >> 56) |
		   ((val & 0x00FF000000000000ull) >> 40) |
		   ((val & 0x0000FF0000000000ull) >> 24) |
		   ((val & 0x000000FF00000000ull) >>  8) |
		   ((val & 0x00000000FF000000ull) <<  8) |
		   ((val & 0x0000000000FF0000ull) << 24) |
		   ((val & 0x000000000000FF00ull) << 40) |
		   ((val & 0x00000000000000FFull) << 56);
#endif
}

static inline UINT32 E32(UINT32 val) {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_bswap32(val);
#elif defined(_MSC_VER)
	return _byteswap_ulong(val);
#else
	// Fallback for other compilers
	return ((val & 0xFF000000) >> 24) |
		   ((val & 0x00FF0000) >>  8) |
		   ((val & 0x0000FF00) <<  8) |
		   ((val & 0x000000FF) << 24);
#endif
}

static inline UINT16 E16(UINT16 val) {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_bswap16(val);
#elif defined(_MSC_VER)
	return _byteswap_ushort(val);
#else
	// Fallback for other compilers
	return (val >> 8) | (val << 8);
#endif
}
#endif

#ifdef LILENDIAN

#ifdef E16
#undef E16
#endif
#define E16

#ifdef E32
#undef E32
#endif
#define E32

#ifdef E64
#undef E64
#endif
#define E64

#endif


#define CONV16(x) ((x) = E16(x));
#define CONV32(x) ((x) = E32(x));



#endif
