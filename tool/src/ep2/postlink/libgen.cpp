
#include "libgen.h"
#include "constlib.h"

char		*fnStrTbl;
UINT32		fnStrIdx, apiStrIdx;
Elf32_Sym	*fnSymTbl;
UINT32		fnSymCnt;


char		*cstStrTbl;
UINT32		cstStrIdx;
Ldr_Sym		*cstSymTbl;
UINT32		cstSymCnt;

extern char*		constNamesStrTbl;
extern int			constNamesStrIdx;
extern Ldr_Sym*		constNamesSymTbl;
extern UINT32		constNamesCount;

#define LIBGEN_PHNUM	2
#define LIBGEN_TAGNUM	4


const Elf32_Ehdr defElfHeader = {
	"\x7F""ELF\x01\x01\x01",
	ET_DYN,
	EM_ARM,
	EV_CURRENT,
	0, //entry
	0, //phoff
	0, //shoff
	0x4000000, //flags
	sizeof(Elf32_Ehdr),
	sizeof(Elf32_Phdr),
	0, //phnum
	sizeof(Elf32_Shdr),
	0, //shnum
	0 //shstrndx
};

const char defShStrTab[] = {
	"\0"
	".dynamic\0"
	".dynsym\0"
	".dynstr\0"
	".shstrtab\0"
};

const UINT8 defShStrIdx[] = { 0, 1, 10, 18, 26, 36 };

enum
{
	LSIDX_NULL = 0,
	LSIDX_DYNAMIC,
	LSIDX_DYNSYM,
	LSIDX_DYNSTR,
	LSIDX_SHSTRTAB,
	LSIDX_NUM_SECTIONS
};



UINT32	libgenMain( char *symdef, char *constHdr )
{
	int				i;
	// Загружаем def
	i = prepareDef( Config.deffile, &def );
	if ( !i ) return 1;

	i = parseFnSymDef(symdef);
	if ( i ) return 2;
	
	makeStubLib();

	parseCHeader(constHdr);
	parseConstSymDef(symdef);

	makeLdrLib();

	free(fnSymTbl);
	free(fnStrTbl);

	free(cstSymTbl);
	free(cstStrTbl);

	free(constNamesSymTbl);
	free(constNamesStrTbl);

	freeDef( &def );
	
	return 0;
}

UINT32 makeStubLib()
{
	char			*res;
	Elf32_Ehdr		*elfHeader;
	Elf32_Phdr		*programHeaders;
	Elf32_Shdr		*sectionHeaders;
	Elf32_Dyn		*dynTags;
	Elf32_Sym		*dynsym;
	UINT32			i, size;

	size = fnSymCnt*sizeof(Elf32_Sym)*4;

	res = (char*)malloc(size);
	if ( !res ) return 1;
	
	Off = 0;

	// Header

	elfHeader = (Elf32_Ehdr*)&res[Off];
	memcpy( elfHeader, &defElfHeader, sizeof(Elf32_Ehdr) );

		INC_O( sizeof(Elf32_Ehdr) );

	// Section Headers
	// we need only .dynamic, .dynsym, .dynstr, .shstrtab

	elfHeader->e_shoff = Off;
	sectionHeaders = (Elf32_Shdr*)&res[Off];

	//null
	memset( sectionHeaders, 0, sizeof(Elf32_Shdr) );

	//dynamic
	sectionHeaders[LSIDX_DYNAMIC].sh_addr = 0;
	sectionHeaders[LSIDX_DYNAMIC].sh_addralign = 0;
	sectionHeaders[LSIDX_DYNAMIC].sh_entsize = sizeof(Elf32_Dyn);
	sectionHeaders[LSIDX_DYNAMIC].sh_flags = SHF_NONE;
	sectionHeaders[LSIDX_DYNAMIC].sh_info = 0;
	sectionHeaders[LSIDX_DYNAMIC].sh_link = 0;
	sectionHeaders[LSIDX_DYNAMIC].sh_name = defShStrIdx[LSIDX_DYNAMIC];
	sectionHeaders[LSIDX_DYNAMIC].sh_type = SHT_DYNAMIC;
	

	//.dynsym
	sectionHeaders[LSIDX_DYNSYM].sh_addr = 0;
	sectionHeaders[LSIDX_DYNSYM].sh_addralign = 0;
	sectionHeaders[LSIDX_DYNSYM].sh_entsize = sizeof(Elf32_Sym);
	sectionHeaders[LSIDX_DYNSYM].sh_flags = SHF_NONE;
	sectionHeaders[LSIDX_DYNSYM].sh_info = 0;
	sectionHeaders[LSIDX_DYNSYM].sh_link = LSIDX_DYNSTR;
	sectionHeaders[LSIDX_DYNSYM].sh_name = defShStrIdx[LSIDX_DYNSYM];
	sectionHeaders[LSIDX_DYNSYM].sh_type = SHT_DYNSYM;

	//.dynstr
	sectionHeaders[LSIDX_DYNSTR].sh_addr = 0;
	sectionHeaders[LSIDX_DYNSTR].sh_addralign = 0;
	sectionHeaders[LSIDX_DYNSTR].sh_entsize = sizeof(char);
	sectionHeaders[LSIDX_DYNSTR].sh_flags = SHF_NONE;
	sectionHeaders[LSIDX_DYNSTR].sh_info = 0;
	sectionHeaders[LSIDX_DYNSTR].sh_link = 0;
	sectionHeaders[LSIDX_DYNSTR].sh_name = defShStrIdx[LSIDX_DYNSTR];
	sectionHeaders[LSIDX_DYNSTR].sh_type = SHT_STRTAB;

	//.shstrtab
	sectionHeaders[LSIDX_SHSTRTAB].sh_addr = 0;
	sectionHeaders[LSIDX_SHSTRTAB].sh_addralign = 0;
	sectionHeaders[LSIDX_SHSTRTAB].sh_entsize = sizeof(char);
	sectionHeaders[LSIDX_SHSTRTAB].sh_flags = SHF_NONE;
	sectionHeaders[LSIDX_SHSTRTAB].sh_info = 0;
	sectionHeaders[LSIDX_SHSTRTAB].sh_link = 0;
	sectionHeaders[LSIDX_SHSTRTAB].sh_name = defShStrIdx[LSIDX_SHSTRTAB];
	sectionHeaders[LSIDX_SHSTRTAB].sh_type = SHT_STRTAB;

		INC_O( sizeof(Elf32_Shdr)*LSIDX_NUM_SECTIONS );

	// Sections

	// .dynamic
		// we need: DT_STRTAB DT_SYMTAB DT_STRSZ DT_NULL
		size = sizeof(Elf32_Dyn)*LIBGEN_TAGNUM;
		dynTags = (Elf32_Dyn*)&res[Off];

		sectionHeaders[LSIDX_DYNAMIC].sh_offset = Off;
		sectionHeaders[LSIDX_DYNAMIC].sh_size = size;

		INC_O( size );

	// .dynsym
		size = sizeof(Elf32_Sym)*fnSymCnt;
		memcpy( &res[Off], fnSymTbl, size );
		dynsym = (Elf32_Sym*)&res[Off];
		sectionHeaders[LSIDX_DYNSYM].sh_offset = Off;
		sectionHeaders[LSIDX_DYNSYM].sh_size = size;
		INC_O( size );

	// .dynstr
		size = fnStrIdx;
		memcpy( &res[Off], fnStrTbl, size );
		sectionHeaders[LSIDX_DYNSTR].sh_offset = Off;
		sectionHeaders[LSIDX_DYNSTR].sh_size = size;
		INC_O( size );

	// .shstrtab
		size = defShStrIdx[LSIDX_NUM_SECTIONS];
		memcpy( &res[Off], defShStrTab, size );
		sectionHeaders[LSIDX_SHSTRTAB].sh_offset = Off;
		sectionHeaders[LSIDX_SHSTRTAB].sh_size = size;
		INC_O( size );


	//Fill in the dynamic tags section

		dynTags[0].d_tag = DT_SYMTAB;
		dynTags[0].d_val = sectionHeaders[LSIDX_DYNSYM].sh_offset;

		dynTags[1].d_tag = DT_STRTAB;
		dynTags[1].d_val = sectionHeaders[LSIDX_DYNSTR].sh_offset;

		dynTags[2].d_tag = DT_STRSZ;
		dynTags[2].d_val = sectionHeaders[LSIDX_DYNSTR].sh_size;

		dynTags[3].d_tag = DT_NULL;
		dynTags[3].d_val = 0;
		
	// Program Headers
		programHeaders = (Elf32_Phdr*)&res[Off];
		elfHeader->e_phoff = Off;

	// NULL
		memset( programHeaders, 0, sizeof(Elf32_Phdr) );
	// DYNAMIC
		programHeaders[1].p_type = PT_DYNAMIC;
		programHeaders[1].p_offset = sectionHeaders[LSIDX_DYNAMIC].sh_offset;
		programHeaders[1].p_vaddr = 0;
		programHeaders[1].p_paddr = 0;
		programHeaders[1].p_filesz = sectionHeaders[LSIDX_DYNAMIC].sh_size;
		programHeaders[1].p_memsz = 0;
		programHeaders[1].p_flags = PF_R;
		programHeaders[1].p_align = 4;
	
		INC_O( sizeof(Elf32_Phdr)*LIBGEN_PHNUM );


	// Finish the header

	elfHeader->e_phnum = LIBGEN_PHNUM;
	elfHeader->e_shnum = LSIDX_NUM_SECTIONS;
	elfHeader->e_shstrndx = LSIDX_SHSTRTAB;

	// Endian switches
	#ifndef LILENDIAN
	switchEndian( elfHeader );

	for ( i=0; i<LSIDX_NUM_SECTIONS; i++ )
		switchEndian( &sectionHeaders[i] );

	for ( i=0; i<LIBGEN_TAGNUM; i++ )
		switchEndian( &dynTags[i] );

	for ( i=0; i<fnSymCnt; i++ )
		switchEndian(&dynsym[i]);

	for ( i=0; i<LIBGEN_PHNUM; i++ )
		switchEndian(&programHeaders[i]);
		
	elfHeader->e_ident[5] = 2;

	#endif

	FILE			*f;

	f = fopen( "std.sa", "wb" );
	if ( f==0 ) 
	{
		printf( "can't open 'std.sa'\n" );
		free(res);
		return 2;
	}

	fwrite( res, Off, 1, f );

	fclose(f);

	free(res);

	return 0;
}

int sortCmpSym( const void *s1, const void *s2 )
{
	return ( ((Ldr_Sym*)s1)->st_name - ((Ldr_Sym*)s2)->st_name );
}


#define LDRAPI_CNT		(def.count)
#define ldrAPIStrTab	(def.strings)

UINT32 makeLdrLib()
{
	FILE		*f;
	ldrLibHdr	hdr;
	Ldr_Sym		*sym;
	UINT32		i, j;

	fnSymCnt -= (LDRAPI_CNT+1); //-NULL symbol
	//fnSymCnt -= (def.count+1); //-NULL symbol

	memset( &hdr, 0, sizeof(ldrLibHdr) );
	hdr.magic = E32(0x42494C7F);
	strcpy( hdr.ver, Config.version );
	strcpy( hdr.firmware, Config.firmware );
	hdr.symCnt = fnSymCnt;
	hdr.strTabSz = apiStrIdx-1; //-1 because of leading \0 in strtab
	hdr.strTabOff = sizeof(ldrLibHdr) + fnSymCnt*sizeof(Ldr_Sym);
	hdr.constCnt = cstSymCnt;
	hdr.constOff = hdr.strTabOff+hdr.strTabSz;

	sym = (Ldr_Sym*)malloc( fnSymCnt*sizeof(Ldr_Sym) );
	if ( !sym ) return 1;

	for ( i=0; i<fnSymCnt; i++ )
	{
		sym[i].st_value = fnSymTbl[i+1].st_value;
		sym[i].st_name = fnSymTbl[i+1].st_name-1; //-1 because of leading \0 in strtab
	}

	// Resolve the const names
	if ( cstSymCnt > 0 )
	{
		for ( j=0; j<cstSymCnt; j++ )
		{	
			for ( i=0; i<constNamesCount; i++ )
			{
				if ( strcmp(&cstStrTbl[cstSymTbl[j].st_name], &constNamesStrTbl[constNamesSymTbl[i].st_name]) == 0 )
				{
					cstSymTbl[j].st_name = constNamesSymTbl[i].st_value;
					break;
				}
			}

			if ( i == constNamesCount )
			{
				printf(" ERROR: Constant ID \"%s\" is undefined, assuming zero.\n", &cstStrTbl[cstSymTbl[j].st_name]);
				cstSymTbl[j].st_name = 0;
			}
		}

		qsort( cstSymTbl, cstSymCnt, sizeof(Ldr_Sym), sortCmpSym );
	
		// Endianness conversions
		#ifndef LILENDIAN

		switchEndian( &hdr );
		for ( i=0; i<fnSymCnt; i++ )
		{
			CONV32( sym[i].st_name );
			CONV32( sym[i].st_value );
		}

		for ( i=0; i<cstSymCnt; i++ )
		{
			/* именно CONV16, т.к. ID константы UINT16 */
			CONV16( cstSymTbl[i].st_name );
			CONV32( cstSymTbl[i].st_value );
		}

		#endif
	}	// if ( cstSymCnt > 0 )
	
	f = fopen( "std.lib", "wb" );
	if ( f==0 ) 
	{
		printf( "can't open 'std.lib'\n" );
		free( sym );
		return 2;
	}

	// Header
	fwrite( &hdr, sizeof(ldrLibHdr), 1, f );

	// Functions
	fwrite( sym, fnSymCnt*sizeof(Ldr_Sym), 1, f );
	fwrite(fnStrTbl+1, apiStrIdx-1, 1, f );

	// Constants
	if ( cstSymCnt > 0 )
	{
		for ( i=0; i<cstSymCnt; i++ )
		{
			fwrite( &cstSymTbl[i].st_name, 2, 1, f );
		}

		for ( i=0; i<cstSymCnt; i++ )
		{
			fwrite( &cstSymTbl[i].st_value, 4, 1, f );
		}
	}

	fclose(f);

	free(sym);

	return 0;
}

UINT32	parseFnSymDef( char *path )
{
	UINT32			i, j;
	UINT32			nlines, size;
	char			*buf;
	char			mode;

	buf = openAndFormat( path, &nlines, &size );
	if ( !buf ) return 1;

	fnSymTbl = (Elf32_Sym*)malloc( (nlines+LDRAPI_CNT)*sizeof(Elf32_Sym) );
	//fnSymTbl = (Elf32_Sym*)malloc( (nlines+def.count)*sizeof(Elf32_Sym) );
	fnStrTbl = (char*)malloc( size+LDRAPI_CNT*32 );
	//fnStrTbl = (char*)malloc( size+def.count*32 );
	fnStrIdx = 1;
	fnStrTbl[0] = '\0';

	// Запоняем symtab
	// Нулевой символ должен быть NULL
	memset(&fnSymTbl[0], 0, sizeof(Elf32_Sym));
	fnSymCnt = 1;
	j = 0;
	for ( i=0; i < nlines; i++ )
	{
		if ( buf[j] == '0' && buf[j+1] == 'x')
		{
			sscanf( &buf[j], "%X %c %s", &fnSymTbl[fnSymCnt].st_value, &mode, &fnStrTbl[fnStrIdx] );
			if (mode == 'T') fnSymTbl[fnSymCnt].st_value++;
			
			if ( (mode == 'A') || (mode == 'T') || (mode=='D') )
			{
				fnSymTbl[fnSymCnt].st_name = fnStrIdx;
				fnSymTbl[fnSymCnt].st_shndx = LSIDX_DYNSTR;
				fnSymTbl[fnSymCnt].st_info = ELF32_ST_INFO(1, STT_FUNC);
				fnSymTbl[fnSymCnt].st_other = 0;
				fnSymTbl[fnSymCnt].st_size = 0;

				fnStrIdx += (UINT32)strlen( &fnStrTbl[fnStrIdx] )+1;
				fnSymCnt++;
			}
		}

		// Skip a line
		while ( buf[j] != 0 )
			j++;
		j++;		
	}

	free( buf );

	apiStrIdx = fnStrIdx;
	// Добавляем функции из API загрузчика
	for ( i=0; i < LDRAPI_CNT; i++ )
	//for ( i=0; i<def.count; i++ )
	{
		strcpy( &fnStrTbl[fnStrIdx], ldrAPIStrTab[i] );
		//strcpy( &fnStrTbl[fnStrIdx], def.strings[i] );
		fnSymTbl[fnSymCnt].st_name = fnStrIdx;
		fnSymTbl[fnSymCnt].st_value = 1;
		fnSymTbl[fnSymCnt].st_shndx = LSIDX_DYNSTR;
		fnSymTbl[fnSymCnt].st_info = ELF32_ST_INFO(1, STT_FUNC);
		fnSymTbl[fnSymCnt].st_other = 0;
		fnSymTbl[fnSymCnt].st_size = 0;
		fnStrIdx += (UINT32)strlen( &fnStrTbl[fnStrIdx] )+1;
		fnSymCnt++;
	}

	return 0;
}

UINT32	parseConstSymDef( char *path )
{
	UINT32		i,j, nlines, size;
	char		*buf;
	char		mode;
	
	buf = openAndFormat( path, &nlines, &size );
	if ( !buf ) 
	{		
		cstSymTbl = NULL;
		cstStrTbl = NULL;
		cstStrIdx = 0;
		cstSymCnt = 0;
		return 1;
	}

	cstSymTbl = (Ldr_Sym*)malloc(nlines*sizeof(Ldr_Sym));
	cstStrTbl = (char*)malloc(size);
	cstStrIdx = 0;
	cstSymCnt = 0;

	// Запоняем symtab
	j = 0;
	for ( i=0; i<nlines; i++ )
	{
		if ( buf[j] == '0' && buf[j+1] == 'x')
		{
			sscanf( &buf[j], "%X %c %s", &cstSymTbl[cstSymCnt].st_value, &mode, &cstStrTbl[cstStrIdx] );
			if (mode == 'C')
			{
				cstSymTbl[cstSymCnt].st_name = cstStrIdx;

				cstStrIdx += (UINT32)strlen( &cstStrTbl[cstStrIdx] )+1;
				cstSymCnt++;
			}
		}

		// Skip a line
		while ( buf[j] != '\0' ) j++;
		j++;		
	}

	free( buf );

	return 0;
}


char* openAndFormat( char* path, UINT32 *nlines, UINT32 *size )
{
	int		i, sz, lineCnt;
	FILE	*f;
	char	*buf;

	f = fopen( path, "rb" );
	if ( f==0 ) 
	{
		printf( "can't open '%s'\n", path );
		return NULL; //error
	}

	sz = getFileSize(f);
	buf = (char*)malloc(sz+1);
	fread( buf, sz, 1, f );
	fclose(f);
	buf[sz] = 0;

	// Подсчитаем кол-во строчек
	lineCnt = 0;
	i = 0;
	while ( buf[i] != 0 )
	{
		if ( buf[i] == '\n' )
		{
			lineCnt++;
			buf[i++] = 0;
		}

		i++;
	}

	*size = (UINT32)sz;
	*nlines = (UINT32)(lineCnt+1);
	return buf;

}

/*---------------------------------------------*\
|			Endianness operations				|
\*---------------------------------------------*/

void switchEndian(ldrLibHdr *hdr)
{
	CONV32( hdr->magic );
	CONV32( hdr->symCnt );
	CONV32( hdr->strTabSz );
	CONV32( hdr->strTabOff );
	CONV32( hdr->constCnt );
	CONV32( hdr->constOff );
}



