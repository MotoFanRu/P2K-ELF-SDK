
#ifndef __ELF_H__
#define __ELF_H__


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

// e_ident[4] := ELFCLASS32 - идентифицирует архитектуру процессора, дл€ которого предназначен файл.
#define ELFCLASSNONE 0		// не задан 
#define ELFCLASS32 1		// 32-х битна€ платформа (!) 
#define ELFCLASS64 2		// 64-х битна€ платформа

// e_ident[5] := ELFDATA2LSB - пор€док байтов
#define ELFDATANONE 0		// не задан 
#define ELFDATA2LSB 1		// LE = Intel-стиль (!) 
#define ELFDATA2MSB 2		// BE = Motorolla-стиль

// e_ident[6] := EV_CURRENT - верси€ (1)
#define EV_NONE 0			// не задан
#define EV_CURRENT 1		// по-умолчанию (!)

// e_type Ч This member identifies the object file type.
#define ET_NONE		0		//No file type
#define ET_REL		1		//Re-locatable file
#define ET_EXEC		2		//Executable file
#define ET_DYN		3		//Shared object file
#define ET_CORE		4		//Core file
#define ET_LOPROC	0xff00	//Processor-specific
#define ET_HIPROC	0xffff	//Processor-specific

// e_machine Ч This memberТs value specifies the required architecture for an individual file.
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

// e_version Ч This member identifies the object file version.
#define EV_NONE			0	//Invalid version
#define EV_CURRENT		1	//Current version

// e_flags
#define EF_ARM_HASENTRY			0x02	// e_entry contains a program-loader entry point 
#define EF_ARM_SYMSARESORTED	0x04	// Each subsection of the symbol table is sorted by symbol value
#define EF_ARM_DYNSYMSUSESEGIDX 0x08	// Symbols in dynamic symbol tables that are defined in sections
										// included in program segment n have st_shndx = n + 1.
#define EF_ARM_MAPSYMSFIRST		0x10	// Mapping symbols precede other local symbols in the symbol table
#define EF_ARM_EABIMASK		0xFF000000	// This masks an 8-bit version number, the version of the ARM
										// EABI to which this ELF file conforms.
										// A value of 0 denotes unknown conformance.

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

//Program header flags
#define PF_X		0x1			//The program will fetch instructions from the segment.
#define PF_R		0x2			//The program will read data from the segment.
#define PF_W		0x4			//The program will write to the segment.

/////////////////////////Dynamic section//////////////////////////////
typedef struct 
{
	Elf32_Sword d_tag;
	Elf32_Word  d_val;
} Elf32_Dyn;

//Dynamic section tags
#define DT_NULL					0	//Ignored. This entry marks the end of the dynamic array. mandatory
#define DT_NEEDED				1	//Index in the string table of the name of a needed library. multiple
#define DT_PLTRELSZ				2	//These entries are unused by versions 1-2 of the ARM EABI. unused
#define DT_PLTGOT				3
#define DT_HASH					4	//The offset of the hash table section in the dynamic segment. mandatory
#define DT_STRTAB				5	//The offset of the string table section in the dynamic segment. mandatory
#define DT_SYMTAB				6	//The offset of the symbol table section in the dynamic segment. mandatory
#define DT_RELA					7	//The offset in the dynamic segment of an SHT_RELA relocation section, Its byte size, and the byte size of an ARM RELA-type relocation entry. optional
#define DT_RELASZ				8
#define DT_RELAENT				9
#define DT_STRSZ				10	//The byte size of the string table section. mandatory
#define DT_SYMENT				11	//The byte size of an ARM symbol table entryЧ16. mandatory
#define DT_INIT					12	//These entries are unused by versions 1-2 of the ARM EABI. unused
#define DT_FINI					13
#define DT_SONAME				14	//The Index in the string table of the name of this shared object. mandatory
#define DT_RPATH				15	//Unused by the ARM EABI. unused
#define DT_SYMBOLIC				16
#define DT_REL					17	//The offset in the dynamic segment of an SHT_REL relocation section, Its byte size, and the byte size of an ARM REL-type relocation entry optional
#define DT_RELSZ				18
#define DT_RELENT				19
#define DT_PLTREL				20	//These entries are unused by versions 1-2 of the ARM EABI. unused
#define DT_DEBUG				21
#define DT_TEXTREL				22
#define DT_JMPREL				23
#define DT_BIND_NOW				24

#define DT_NONABI_SYMTABNO		25 //Indicates the number of entries in the .dynsym section. 
#define DT_NONABI_IMPORTS		26

#define DT_MAX_TAG				DT_NONABI_IMPORTS

#define DT_LOPROC				0x70000000 //Values in this range are reserved to the ARM EABI. unused
//#define DT_SYMTABNO		0x70000011 //Indicates the number of entries in the .dynsym section. 
#define DT_HIPROC				0x7fffffff

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
#define R_ARM_PC24            1   //ARM B/BL S Ц P + A
#define R_ARM_ABS32           2   //32-bit word S + A
#define R_ARM_REL32           3   //32-bit word S Ц P + A
#define R_ARM_PC13            4   //ARM LDR r, [pc,Е] S Ц P + A
#define R_ARM_ABS16           5   //16-bit half-word S + A
#define R_ARM_ABS12           6   //ARM LDR/STR S + A
#define R_ARM_THM_ABS5        7   //Thumb LDR/STR S + A
#define R_ARM_ABS8            8   //8-bit byte S + A
#define R_ARM_SBREL32         9   //32-bit word S Ц B + A
#define R_ARM_THM_PC22        10  //Thumb BL pair S Ц P+ A
#define R_ARM_THM_PC8         11  //Thumb LDR r, [pc,Е] S Ц P + A
#define R_ARM_AMP_VCALL9      12  //AMP VCALL ObsoleteЧSA-1500 only.
#define R_ARM_SWI24           13  //ARM SWI S + A
#define R_ARM_THM_SWI8        14  //Thumb SWI S + A
#define R_ARM_XPC25           15  //ARM BLX S Ц P+ A
#define R_ARM_THM_XPC22       16  //Thumb BLX pair S Ц P+ A
#define R_ARM_COPY            20  //32 bit word Copy symbol at dynamic link time.
#define R_ARM_GLOB_DAT        21  //32 bit word Create GOT entry.
#define R_ARM_JUMP_SLOT       22  //32 bit word Create PLT entry.
#define R_ARM_RELATIVE        23  //32 bit word Adjust by program base.
#define R_ARM_GOTOFF          24  //32 bit word Offset relative to start of GOT.
#define R_ARM_GOTPC           25  //32 bit word Insert address of GOT.
#define R_ARM_GOT32           26  //32 bit word Entry in GOT.
#define R_ARM_PLT32           27  //ARM BL Entry in PLT.
#define R_ARM_ALU_PCREL_7_0   32  //ARM ADD/SUB (S Ц P + A) & 0x000000FF
#define R_ARM_ALU_PCREL_15_8  33  //ARM ADD/SUB (S Ц P + A) & 0x0000FF00
#define R_ARM_ALU_PCREL_23_15 34  //ARM ADD/SUB (S Ц P + A) & 0x00FF0000
#define R_ARM_LDR_SBREL_11_0  35  //ARM LDR/STR (S Ц B + A) & 0x00000FFF
#define R_ARM_ALU_SBREL_19_12 36  //ARM ADD/SUB (S Ц B + A) & 0x000FF000
#define R_ARM_ALU_SBREL_27_20 37  //ARM ADD/SUB (S Ц B + A) & 0x0FF00000
#define R_ARM_GNU_VTENTRY     100 //32 bit word Record C++ vtable entry.
#define R_ARM_GNU_VTINHERIT   101 //32 bit word Record C++ member usage.
#define R_ARM_THM_PC11        102 //Thumb B S Ц P + A
#define R_ARM_THM_PC9         103 //Thumb B<cond> S Ц P + A
#define R_ARM_RXPC25          249 //ARM BLX (?S Ц ?P) + A For calls between program segments.
#define R_ARM_RSBREL32        250 //Word (?S Ц ?SB) + A For an offset from SB, the static base.
#define R_ARM_THM_RPC22       251 //Thumb BL/BLX pair (?S Ц ?P) + A For calls between program segments.
#define R_ARM_RREL32          252 //Word (?S Ц ?P) + A For on offset between two segments.
#define R_ARM_RABS32          253 //Word ?S + A For the address of a location in the target segment.
#define R_ARM_RPC24           254 //ARM B/BL (?S Ц ?P) + A For calls between program segments.
#define R_ARM_RBASE           255 //None NoneЧIdentifies the segment being relocated by the following relocation directives.

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

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

//Symbol Types, ELF32_ST_TYPE
#define STT_NOTYPE				0 //The symbol's type is not specified.
#define STT_OBJECT				1 //The symbol is associated with a data object, such as a variable, an array, and so on.
#define STT_FUNC					2 //The symbol is associated with a function or other executable code.
#define STT_SECTION				3 //The symbol is associated with a section. Symbol table entries of this type exist
								  //primarily for relocation and normally have STB_LOCAL binding.
#define	STT_FILE				4 //A file symbol has STB_LOCAL binding, its section index is SHN_A BS, and it
								  //precedes the other STB_LOCAL symbols for the file, if it is present.


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

//Section Types, sh_type
#define SHT_NULL		0	//This value marks a section header that does not have an associated
							// section. Other members of the section header have undefined values
#define SHT_PROGBITS	1	//The section holds information defined by the program, whose format and
							// meaning are determined solely by the program.
#define SHT_SYMTAB		2	//The section holds a symbol table.
#define SHT_STRTAB		3	//The section holds a string table.
#define SHT_RELA		4	//The section holds relocation entries with explicit addends, such as type
							// Elf32_Rela for the 32-bit class of object files. An object file may have
							// multiple relocation sections. See Relocation below for details.
#define SHT_HASH		5	//The section holds a symbol hash table.
#define SHT_DYNAMIC		6	//The section holds information for dynamic linking.
#define SHT_NOTE		7	//This section holds information that marks the file in some way.
#define SHT_NOBITS		8	//A section of this type occupies no space in the file but otherwise resembles
							// SHT_PROGBITS. Although this section contains no bytes, the sh_offset
							// member contains the conceptual file offset.
#define SHT_REL			9	//The section holds relocation entries without explicit addends, such as type
							// Elf32_Rel for the 32-bit class of object files. An object file may have multiple
							// relocation sections. See Relocation below for details.
#define SHT_SHLIB		10	//This section type is reserved but has unspecified semantics.
#define SHT_DYNSYM		11	//The section holds a symbol table.


//Section Attribute Flags, sh_flags
#define SHF_NONE		0x0
#define SHF_WRITE		0x1	//The section contains data that should be writable during process execution
#define SHF_ALLOC		0x2 //The section occupies memory during process execution. Some control
							// sections do not reside in the memory image of an object file; this
							// attribute is off for those sections
#define SHF_EXECINSTR	0x4 //The section contains executable machine instructions.


#endif /* __ELF_H__ */
