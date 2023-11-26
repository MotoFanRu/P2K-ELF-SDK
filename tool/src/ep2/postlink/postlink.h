
#ifndef POSTLINK_H
#define POSTLINK_H

#include "common.h"
#include "libgen.h"
#include "constlib.h"


// В PLT секции в начале присутствует ненужный для данной реализации код
// STR     LR, [SP,#-4]!
// LDR     LR, =$_GLOBAL_OFFSET_TABLE_ ; PIC mode
// NOP
// LDR     PC, [LR,#8]!
// DCD $_GLOBAL_OFFSET_TABLE_
// Его размер считаем фиксированным
#define PLT_INIT_SIZE 	20

typedef struct
{
	UINT32 bxpc; //BX PC \ NOP = (const)0x7847C046;
	UINT32 adr;  //ADR	R12, loc_678 = (const)0x00C68FE2
	UINT32 add;  //ADD	R12, R12, #0 = (const)0x00CA8CE2
	UINT32 ldr;  //LDR  PC, [R12,#0x580]! 
	//(const)0xE5BCF000 | <off12>
	//LE: ?? F? BC E5

} PLT_ENTRY_OLD_T;

typedef struct
{
	UINT32 bxpc; //BX PC \ NOP = (const)0x477846C0;
	UINT32 ldr;  //LDR  R12, [PC,#0x0]
	//E59FC000 | <off12_to_got_entry>-8
	//LE: 00 C0 9F E5
	UINT32 bxlr; //BX LR = (const)E12FFF1C

} PLT_ENTRY_NEW_T;


enum REL_MODE_T
{
	RM_PTR=0,
	RM_PTR_ENDIAN,
	RM_VIRTUAL

};



UINT32 InitStructs();
UINT32 AddCode();
UINT32 AddBSS();
UINT32 BuildPLT();


UINT32 __fastcall makeBL(UINT32 from, UINT32 to, char mode);
UINT32 findSectionIdxByName(const char* name, Elf32_Shdr *sectHdrs, char* strtab);
UINT32 findSectionIdxByOldIdx(UINT32 oldidx);
UINT32 findRefs(UINT32 addr, UINT32 *refs);
UINT32 addSymbol(UINT32 oldindex, UINT32 section, BOOL named=TRUE);
UINT32 addStrData( char* str );
UINT32 addSHStrData( char* str );
UINT32 getPLTReference(PLT_ENTRY_OLD_T *plt, UINT32 offset);
UINT32 Relocate(UINT32 *target, REL_MODE_T mode);
UINT32 BuildSectionHeaders();
UINT32 CountSymbols();
UINT32 BuildLocalRels();
UINT32 ReserveDynamicSection();
UINT32 BuildDynamicTags();
UINT32 BuildSHStrTab();
UINT32 PlaceStrTab(UINT32 index);
UINT32 InitProgramHeaders();
UINT32 findInStrTab(char *strtab, UINT32 size, char *str);
UINT32 BuildSharedExports();

int printWelcome();
int printHelp();
void initConfig();
int parseOption(char** argv, int index);

char* removePath(char *path);


#define PHDRS_NUM			2
#define STATIC_TAGS_NUM		11

typedef struct
{
	const char	*name;
	Elf32_Word	type;
	Elf32_Word	flags;
	Elf32_Word	prop;

	Elf32_Shdr	*oldhdr;
	Elf32_Shdr	newhdr;

	UINT32		idx;
	UINT32		oldidx;
	UINT32		value; //section-specific micellaneous data
	void		*oldata;
	void		*data;
} SECTION_PROPS_T;

const char *ptypes[]=
{
	"PT_NULL",
	"PT_LOAD",
	"PT_DYNAMIC",
	"PT_INTERP",
	"PT_NOTE",
	"PT_SHLIB",
	"PT_PHDR"
};

#define SPROP_LEAVE			0
#define SPROP_CREATE		1
#define SPROP_IMAGINARY		2

enum
{ 
	SIDX_TEXT=0,
	SIDX_RODATA,
	SIDX_DATA,
	SIDX_PLT,
	SIDX_GOT,
	SIDX_BSS,
	SIDX_DYNAMIC,
	SIDX_REL_PLT,
	SIDX_IMPORTS,
	SIDX_REL_DYN,
	SIDX_DYNSYM,
	//SIDX_SYMTAB,
	//SIDX_HASH,
	//SIDX_INTERP,
	//SIDX_COMMENT,
	
	//SIDX_STRTAB,
	SIDX_DYNSTR,
	SIDX_SHSTRTAB,
	PROPS_CNT
};


SECTION_PROPS_T sProps[PROPS_CNT]=
{
	{ ".text",		SHT_PROGBITS,	SHF_ALLOC|SHF_EXECINSTR },
	{ ".rodata",	SHT_PROGBITS,	SHF_ALLOC },
	{ ".data",		SHT_PROGBITS,	SHF_ALLOC|SHF_WRITE },
	{ ".plt",		SHT_PROGBITS,	SHF_ALLOC|SHF_EXECINSTR },
	{ ".got",		SHT_NOBITS,		SHF_ALLOC, SPROP_IMAGINARY },
	{ ".bss",		SHT_NOBITS,		SHF_ALLOC|SHF_WRITE, SPROP_IMAGINARY },
	{ ".dynamic",	SHT_DYNAMIC,	SHF_NONE },
	{ ".rel.plt",	SHT_REL,		SHF_NONE },
	{ ".imports",	SHT_DYNSYM,		SHF_NONE, SPROP_CREATE },
	{ ".rel.dyn",	SHT_REL,		SHF_NONE },
	{ ".dynsym",	SHT_DYNSYM,		SHF_NONE },
//	{ ".symtab",	SHT_SYMTAB,		SHF_NONE },
//	{ ".hash",		SHT_HASH,		SHF_NONE },
//	{ ".interp",	SHT_PROGBITS,	SHF_NONE },
//	{ ".comment",	SHT_PROGBITS,	SHF_NONE },
	
	{ ".dynstr",	SHT_STRTAB,		SHF_NONE },
	{ ".shstrtab",	SHT_STRTAB,		SHF_NONE },
//	{ ".strtab",	SHT_STRTAB,		SHF_NONE },

	
	//".data"
	//"ER_RO"
	//"ER_RW"
	//"ER_ZI"

	//".note", //?

	// "" NULL section, discard
	// ".debug_frame" debug info, discard
};

#define PROP_SZ_UNKNOWN (UINT32)(0)



/*---------------------------------------------*\
|			Endianness operations				|
\*---------------------------------------------*/


void switchEndian(PLT_ENTRY_OLD_T *plt);
void switchEndian(Elf32_Rel *rel);


#endif