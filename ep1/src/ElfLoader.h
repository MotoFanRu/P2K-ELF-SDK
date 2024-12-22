#ifndef ELFLOADER_H
#define ELFLOADER_H

//#include <SDK.h>
//#include <DL_FS.h>

#include <typedefs.h>
#include <mem.h>

#define EVCODE_RESERVE    0x40
#define EVCODE_BASE       0xA000

#define EVCODE_STARTLDR     EVCODE_BASE
#define EVCODE_LOADELF      EVCODE_BASE + 1
#define EVCODE_UNLOADELF    EVCODE_BASE + 2
#define EVCODE_GETEVBASE    EVCODE_BASE + 3

typedef unsigned long	Elf32_Addr;		//Unsigned program address
typedef unsigned short	Elf32_Half;		//Unsigned medium integer
typedef unsigned long	Elf32_Off;		//Unsigned file offset
typedef          long	Elf32_Sword;	//Signed large integer
typedef unsigned long	Elf32_Word;		//Unsigned large integer

/////////////////////////Section Header//////////////////////////////
#define EI_NIDENT 16

typedef struct 
{
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;


//E_type—This member identifies the object file type.
#define ET_NONE		0		//No file type
#define ET_REL		1		//Re-locatable file
#define ET_EXEC		2		//Executable file
#define ET_DYN		3		//Shared object file
#define ET_CORE		4		//Core file
#define ET_LOPROC	0xff00	//Processor-specific
#define ET_HIPROC	0xffff	//Processor-specific

//E_machine—This member’s value specifies the required architecture for an individual file.
#define EM_NONE			0	//No machine
#define EM_M32			1	//AT&T WE 32100
#define EM_SPARC		2	//SPARC
#define EM_386			3	//Intel Architecture
#define EM_68K			4	//Motorola 68000
#define EM_88K			5	//Motorola 88000
#define EM_860			7	//Intel 80860
#define EM_MIPS			8	//MIPS RS3000 Big-Endian
#define EM_MIPS_RS4_BE	10	//MIPS RS4000 Big-Endian
#define EM_ARM			40	//ARM/Thumb Architecture

//E_version—This member identifies the object file version.
#define EV_NONE			0	//Invalid version
#define EV_CURRENT		1	//Current version

/////////////////////////Program Header//////////////////////////////
typedef struct 
{
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

//Segment Types, p_type
#define PT_NULL		0			//The array element is unused; other members' values are undefined. This type
								//	lets the program header table have ignored entries.
#define PT_LOAD		1			//The array element specifies a loadable segment, described by p_filesz and
								//	p_memsz (for additional explanation, see PT_LOAD below).
#define PT_DYNAMIC	2			//The array element specifies dynamic linking information. See subsection 4.7.
#define PT_INTERP	3			//The array element specifies the location and size of a null-terminated path
								//	name to invoke as an interpreter.
#define PT_NOTE		4			//The array element specifies the location and size of auxiliary information.
#define PT_SHLIB	5			//This segment type is reserved but has unspecified semantics.
#define PT_PHDR		6			//The array element, if present, specifies the location and size of the program
								//	header table itself (for additional explanation, see PT_ PHDR below).
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff	//Values in this inclusive range are reserved for processor-specific semantics.

/////////////////////////Dynamic section//////////////////////////////
typedef struct 
{
	Elf32_Sword d_tag;
	Elf32_Word  d_val;
} Elf32_Dyn;

//Dynamic section tags
#define DT_NULL			0	//Ignored. This entry marks the end of the dynamic array. mandatory
#define DT_NEEDED		1	//Index in the string table of the name of a needed library. multiple
#define DT_PLTRELSZ		2	//These entries are unused by versions 1-2 of the ARM EABI. unused
#define DT_PLTGOT		3
#define DT_HASH			4	//The offset of the hash table section in the dynamic segment. mandatory
#define DT_STRTAB		5	//The offset of the string table section in the dynamic segment. mandatory
#define DT_SYMTAB		6	//The offset of the symbol table section in the dynamic segment. mandatory
#define DT_RELA			7	//The offset in the dynamic segment of an SHT_RELA relocation section, Its byte size, and the byte size of an ARM RELA-type relocation entry. optional
#define DT_RELASZ		8
#define DT_RELAENT		9
#define DT_STRSZ		10	//The byte size of the string table section. mandatory
#define DT_SYMENT		11	//The byte size of an ARM symbol table entry—16. mandatory
#define DT_INIT			12	//These entries are unused by versions 1-2 of the ARM EABI. unused
#define DT_FINI			13
#define DT_SONAME		14	//The Index in the string table of the name of this shared object. mandatory
#define DT_RPATH		15	//Unused by the ARM EABI. unused
#define DT_SYMBOLIC		16
#define DT_REL			17	//The offset in the dynamic segment of an SHT_REL relocation section, Its byte size, and the byte size of an ARM REL-type relocation entry optional
#define DT_RELSZ		18
#define DT_RELENT		19
#define DT_PLTREL		20	//These entries are unused by versions 1-2 of the ARM EABI. unused
#define DT_DEBUG		21
#define DT_TEXTREL		22
#define DT_JMPREL		23
#define DT_BIND_NOW		24
#define DT_LOPROC		0x70000000 //Values in this range are reserved to the ARM EABI. unused
#define DT_HIPROC		0x7fffffff

/////////////////////////Relocation//////////////////////////////

typedef struct 
{
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
} Elf32_Rel;

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

//ARM relocation types
#define R_ARM_NONE            0   //Any No relocation. Encodes dependencies between sections.
#define R_ARM_PC24            1   //ARM B/BL S – P + A
#define R_ARM_ABS32           2   //32-bit word S + A
#define R_ARM_REL32           3   //32-bit word S – P + A
#define R_ARM_PC13            4   //ARM LDR r, [pc,…] S – P + A
#define R_ARM_ABS16           5   //16-bit half-word S + A
#define R_ARM_ABS12           6   //ARM LDR/STR S + A
#define R_ARM_THM_ABS5        7   //Thumb LDR/STR S + A
#define R_ARM_ABS8            8   //8-bit byte S + A
#define R_ARM_SBREL32         9   //32-bit word S – B + A
#define R_ARM_THM_PC22        10  //Thumb BL pair S – P+ A
#define R_ARM_THM_PC8         11  //Thumb LDR r, [pc,…] S – P + A
#define R_ARM_AMP_VCALL9      12  //AMP VCALL Obsolete—SA-1500 only.
#define R_ARM_SWI24           13  //ARM SWI S + A
#define R_ARM_THM_SWI8        14  //Thumb SWI S + A
#define R_ARM_XPC25           15  //ARM BLX S – P+ A
#define R_ARM_THM_XPC22       16  //Thumb BLX pair S – P+ A
#define R_ARM_COPY            20  //32 bit word Copy symbol at dynamic link time.
#define R_ARM_GLOB_DAT        21  //32 bit word Create GOT entry.
#define R_ARM_JUMP_SLOT       22  //32 bit word Create PLT entry.
#define R_ARM_RELATIVE        23  //32 bit word Adjust by program base.
#define R_ARM_GOTOFF          24  //32 bit word Offset relative to start of GOT.
#define R_ARM_GOTPC           25  //32 bit word Insert address of GOT.
#define R_ARM_GOT32           26  //32 bit word Entry in GOT.
#define R_ARM_PLT32           27  //ARM BL Entry in PLT.
#define R_ARM_ALU_PCREL_7_0   32  //ARM ADD/SUB (S – P + A) & 0x000000FF
#define R_ARM_ALU_PCREL_15_8  33  //ARM ADD/SUB (S – P + A) & 0x0000FF00
#define R_ARM_ALU_PCREL_23_15 34  //ARM ADD/SUB (S – P + A) & 0x00FF0000
#define R_ARM_LDR_SBREL_11_0  35  //ARM LDR/STR (S – B + A) & 0x00000FFF
#define R_ARM_ALU_SBREL_19_12 36  //ARM ADD/SUB (S – B + A) & 0x000FF000
#define R_ARM_ALU_SBREL_27_20 37  //ARM ADD/SUB (S – B + A) & 0x0FF00000
#define R_ARM_GNU_VTENTRY     100 //32 bit word Record C++ vtable entry.
#define R_ARM_GNU_VTINHERIT   101 //32 bit word Record C++ member usage.
#define R_ARM_THM_PC11        102 //Thumb B S – P + A
#define R_ARM_THM_PC9         103 //Thumb B<cond> S – P + A
#define R_ARM_RXPC25          249 //ARM BLX (?S – ?P) + A For calls between program segments.
#define R_ARM_RSBREL32        250 //Word (?S – ?SB) + A For an offset from SB, the static base.
#define R_ARM_THM_RPC22       251 //Thumb BL/BLX pair (?S – ?P) + A For calls between program segments.
#define R_ARM_RREL32          252 //Word (?S – ?P) + A For on offset between two segments.
#define R_ARM_RABS32          253 //Word ?S + A For the address of a location in the target segment.
#define R_ARM_RPC24           254 //ARM B/BL (?S – ?P) + A For calls between program segments.
#define R_ARM_RBASE           255 //None None—Identifies the segment being relocated by the following relocation directives.

typedef struct 
{
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct 
{
	Elf32_Word st_name;
	Elf32_Addr st_value;
} Ldr_Sym;

typedef struct 
{
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

#define NUM_HEADERS	8

enum
{
    ELDR_SUCCESS = 0,
    ELDR_OPEN_FAILED,
    ELDR_READ_HEADER_FAILED,
    ELDR_READ_FAILED,
    ELDR_SEEK_FAILED
};

UINT32 loadELF( char *file_uri,  char *params,  void *Library,  UINT32 reserve );

/* TODO */
//extern int __rt_memclr(void *ptr, UINT32 cnt);
//#define memclr(p, c) __rt_memclr(p, c)

typedef UINT32 (*Entry)(char*, char*, UINT32);

#endif
