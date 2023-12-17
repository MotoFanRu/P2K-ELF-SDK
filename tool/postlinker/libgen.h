
#ifndef LIBGEN_H
#define LIBGEN_H

#include "common.h"

typedef struct
{
	UINT32		magic;
	char		ver[8];
	char		firmware[24];
	UINT32		symCnt;
	UINT32		strTabSz;
	UINT32		strTabOff;
	UINT32		constCnt;
	UINT32		constOff;
} ldrLibHdr;

UINT32	libgenMain( char *symdef, char *constHdr );
UINT32	makeStubLib( );
UINT32	makeLdrLib( );
UINT32	parseFnSymDef( char *path );
UINT32	parseConstSymDef( char *path );
char* openAndFormat( char* path, UINT32 *nlines, UINT32 *size );


/*---------------------------------------------*\
|			Endianness operations					|
\*---------------------------------------------*/

void switchEndian(ldrLibHdr *hdr);

#endif