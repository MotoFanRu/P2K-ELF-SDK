// postlink.cpp : Defines the entry point for the console application.
//

#include "postlink.h"

#define INVALID_BXPC 0xE28FC600

#ifdef LILENDIAN

#define PLT_BXPC 0x46C04778


#else

#define PLT_BXPC 0xc0467847

#endif

#define DEFLIBNAME	"std.sa"


char			*file, *result;
char			dynstr[8192];
char			shstrtab[512];
char			*path;

Elf32_Ehdr		*oldHeader, *newHeader;
Elf32_Phdr		*oldPhdr, *newPhdr;

UINT32			Addr;				// Текущий виртуальный адрес внутри образа эльфа (см. INC_A, INC_OA)
UINT32			Off;				// Текущее смещение в файле образа эльфа (см. INC_O, INC_OA)
UINT32			curIdx = 1;			// Хз, уже не помню
UINT32			externalCnt = 0;	// Кол-во внешних (импортируемых) символов в эльф
UINT32			internalCnt = 0;	// Кол-во внутренних символов (используются для релокаций)
UINT32			pltCnt = 0;			// Кол-во полезных (отфильтрован мусор) записей в PLT таблице (и в GOT, соответственно)
UINT32			extRelCnt = 0;		// Кол-во релокаций, связанных с внешними символами (кол-во записей в REL_PLT)
UINT32			symLsRelCnt = 0;	// Кол-во безсимвольных релокаций (их много)
UINT32			extPltCnt = 0;		// Кол-во записей PLT связанных с импортируемыми ф-циями

CONFIG_T		Config;

STR_TABLE_T		def;

//elib.so -shared -def elib.def
//telf.elf
//#define tst(x) printf("%s, res = %d\n", #x, x)

int main(int argc, char* argv[])
{
	UINT32			filesize;
	int				res=1;
	UINT32			i;


	printWelcome();

	if (argc == 1)
	{
		return printHelp();
	}
	else
	{
		path = NULL;
		initConfig();
		while ( (res < argc) && (res != -1) )
			res = parseOption(argv, res);

		if(res == -1)
		{
			getch();
			return 0;
		}

	}

	if (path == NULL)
	{
		printf("*ERROR: input file not specified!\n");
		getch();
		return 0;
	}

	if ( Config.stdlib )
	{
		libgenMain(path, Config.header);
		return 0;
	}

	// Read all the file contents
	FILE		*f = fopen(path, "rb");

	if (f==NULL) return 0; //не должно происходить

	filesize = getFileSize(f);

	file = (char*)malloc(filesize);
	result = (char*)malloc(filesize);

	fread(file, filesize, 1, f);
	fclose(f);

		Off = 0;
		Addr = 0x8000;

// Init the structs

	oldHeader = (Elf32_Ehdr*)&file[0];

	#ifndef LILENDIAN
	switchEndian(oldHeader);
	#endif

	newHeader = (Elf32_Ehdr*)&result[Off];

	memcpy(newHeader, oldHeader, sizeof(Elf32_Ehdr));
			
	INC_O(sizeof(Elf32_Ehdr));
		

	InitStructs();

// Reserve the program headers, do not fill for now

	oldPhdr = (Elf32_Phdr*)&file[oldHeader->e_phoff];
	newPhdr	= (Elf32_Phdr*)&result[Off];

	//assert(oldHeader->e_phnum==4);

	newHeader->e_phoff = Off;
		
		INC_O(PHDRS_NUM * sizeof(Elf32_Phdr));

// Init the .text, .bss and .rodata sections

		res = AddCode();

		res = CountSymbols();

// Build the PLT, GOT, REL_PLT and IMPORT sections
		res = BuildPLT();


// Complete the .dynsym and build the .rel.dyn section

		res = BuildLocalRels();

// Make the so-specific dynamic data

		res = BuildSharedExports();
		if (res == 2)
		{
			printf("*ERROR: specified def file not found!\n");
			free(file);
			free(result);
			return 0;
		}

// Build the .dynamic section
		res = BuildDynamicTags();

// Place the string tables
		res = PlaceStrTab(SIDX_DYNSTR);
		res = BuildSHStrTab();

	#ifndef LILENDIAN
		Elf32_Rel		*rel;
		rel = (Elf32_Rel*)sProps[SIDX_REL_PLT].data;
		if (rel != NULL)
			for ( i=0; i < sProps[SIDX_REL_PLT].newhdr.sh_size; i+=sizeof(Elf32_Rel), rel++ )
				switchEndian(rel);
		
		rel = (Elf32_Rel*)sProps[SIDX_REL_DYN].data;
		if (rel != NULL)
			for (i=0; i < sProps[SIDX_REL_DYN].newhdr.sh_size; i+=sizeof(Elf32_Rel), rel++ )
				switchEndian(rel);
	#endif

// Init the section headers
		res = BuildSectionHeaders();

// Fill in the program headers
		res = InitProgramHeaders();

// Update the entry

		res = oldHeader->e_entry - sProps[SIDX_TEXT].oldhdr->sh_addr;
		newHeader->e_entry = sProps[SIDX_TEXT].newhdr.sh_addr + (res);

		if (Config.shared)
			newHeader->e_type = ET_DYN;
		else
			newHeader->e_type = ET_EXEC;

// Save the result

	// Endianness
	#ifndef LILENDIAN
	Elf32_Sym 			*sym = (Elf32_Sym*)sProps[SIDX_DYNSYM].data;
	if (sym != NULL)
		for ( i = 0; i < sProps[SIDX_DYNSYM].newhdr.sh_size; i+=sizeof(Elf32_Sym), sym++ )
			switchEndian(sym);

	sym = (Elf32_Sym*)sProps[SIDX_IMPORTS].data;
	if (sym != NULL)
		for ( i = 0; i < sProps[SIDX_IMPORTS].newhdr.sh_size; i+=sizeof(Elf32_Sym), sym++ )
			switchEndian(sym);

	switchEndian(newHeader);

	#endif
	
	f = fopen(Config.output, "wb");
	fwrite(result, Off, 1, f);
	fclose(f);
	//getch();

	free(file);
	free(result);


	return 0;
}

UINT32	getFileSize(FILE *f)
{
	UINT32 sz;
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	return sz;
}

__declspec(naked) UINT32 __fastcall makeBL(UINT32 from, UINT32 to, char mode)
{

	__asm{
		ADD		ecx, 4		// current PC is +4 at least (in thumb)
		SUB		edx, ecx //edx = offset
		MOV		eax, edx
		MOV		ecx, [esp+4]
		CMP		ecx, 'T'
		JE		_thumb_bl

// ARM BL generation
		SUB		eax, 0x4	// add another +4 to 'from' offset for ARM PC
		SHR		eax, 2
		BSWAP	eax
		MOV		al,	0xEB

		RET		4

// THUMB BL generation
_thumb_bl:
		SHR		edx, 12
		XCHG	dl, dh
		AND		dl, 0x7
		OR		dl, 0xF0

		SHL		ax, 4
		ROL		ax, 3
		OR		al, 0xF8
		SHL		eax, 16
		
		MOV		ax, dx

		#ifdef LILENDIAN
		BSWAP	eax
		ROL		eax, 16
		#endif

		RET		4
	}
}

UINT32 findSectionIdxByName(const char* name, Elf32_Shdr *sectHdrs, char* strtab)
{
	int		i;

	for (i=0; i<oldHeader->e_shnum; i++)
	{
		if (strcmp(name, &strtab[ sectHdrs[i].sh_name ]) == 0) return i;
	}

	return 0;
}

UINT32 findSectionIdxByOldIdx(UINT32 oldidx)
{
	UINT32	i;

	if (oldidx == 0) return 0;

	for (i=0; i<PROPS_CNT; i++)
	{
		if (sProps[i].oldidx==oldidx) return sProps[i].idx;
	}

	return 0;
}


UINT32 findRefs(UINT32 addr, UINT32 *refs)
{
	UINT32		i, j=0;
	UINT32		from;
	UINT32		code, bl, op;

	//printf("findRefs: Enter, addr = x%X\n", addr);
	//find THUMB BL refs in .text section
	for ( i = 0;
		 i < sProps[SIDX_TEXT].oldhdr->sh_size;
		 i +=2 )
	{
		op = ((UINT8*)sProps[SIDX_TEXT].oldata)[i];
		if ((op & 0xF0) == 0xF0) // BL
		{
			from = sProps[SIDX_TEXT].oldhdr->sh_offset + i;
			bl = makeBL(from, addr, 'T');
			
			//printf("findRefs: BL, from = x%X, code = x%X\n", from, E32(code));
			code = *(UINT32*)((UINT8*)sProps[SIDX_TEXT].oldata+i);
			if (code == bl)
			{
				//printf("findRefs: find, i=x%X, j=%d\n",i,j+1);
				refs[j++] = i;
			}
		}
	}

	return j;
}


// returns index in strtab
UINT32 addStrData( char* str )
{
	static UINT32 index = 1;
	UINT32	res = index;

	strcpy( &((char*)sProps[SIDX_DYNSTR].data)[index], str);
	index += (UINT32)strlen(str)+1;
	sProps[SIDX_DYNSTR].newhdr.sh_size = index;

	return res;
}

UINT32 addSHStrData( char* str )
{
	static UINT32 index = 1;
	UINT32	res = index;

	strcpy( &((char*)sProps[SIDX_SHSTRTAB].data)[index], str);
	index += (UINT32)strlen(str)+1;
	sProps[SIDX_SHSTRTAB].newhdr.sh_size = index;

	return res;
}

// Adds the symbol from the old dynsym table to the specified section by old index
// returns the new symbol index
UINT32 addSymbol(UINT32 oldindex, UINT32 section, BOOL named)
{
	UINT32			index = sProps[section].value; // Здесь временно хранится текущее кол-во символов в таблице
	UINT32			val;
	UINT32			i;

	Elf32_Sym		*newsym = (Elf32_Sym*)sProps[section].data;		// Новая таблица символов
	Elf32_Sym		*oldsym = &((Elf32_Sym*)sProps[SIDX_DYNSYM].oldata)[oldindex];		// Нужный символ в старой таблице

	// ToDo: Optional
	// NULL symbol initialization
	if (index == 0) 
	{
		memset(newsym, 0, sizeof(Elf32_Sym));
		index++;
		sProps[section].newhdr.sh_size += sizeof(Elf32_Sym);
			INC_OA(sizeof(Elf32_Sym));
	}

	newsym++;

	if (oldindex == 0) return 0;

	//Надо проверить, нет ли уже нужного символа в списке
	if ( (oldsym->st_value != 0) && (oldsym->st_value != 1) )
	{
		val = oldsym->st_value;
		Relocate(&val, RM_PTR);
		for (i=1; i<index; i++)
		{
			if ( val == newsym->st_value )
			{
				if ( (named) && (newsym->st_name==0))
				{
#ifdef _DEBUG
					printf("addSymbol: (%i), :=name, %s\n", section, &((char*)sProps[SIDX_DYNSTR].oldata)[oldsym->st_name]);
#endif
					newsym->st_name = addStrData( &((char*)sProps[SIDX_DYNSTR].oldata)[oldsym->st_name] );
				}
				
				return i;
			}
			newsym++;
		}
	}
	else
	{
		for (i=1; i<index; i++)
		{
			if ( strcmp(&((char*)sProps[SIDX_DYNSTR].oldata)[oldsym->st_name], &dynstr[newsym->st_name]) == 0 )
				return i;
			newsym++;
		}
	}

	memcpy(newsym, oldsym, sizeof(Elf32_Sym));

	newsym->st_shndx = (Elf32_Half)findSectionIdxByOldIdx(newsym->st_shndx);
	if (named) newsym->st_name = addStrData( &((char*)sProps[SIDX_DYNSTR].oldata)[oldsym->st_name] );
	else newsym->st_name = 0;
	Relocate( (UINT32*)&newsym->st_value, RM_PTR );

	sProps[section].newhdr.sh_size += sizeof(Elf32_Sym);

		INC_OA(sizeof(Elf32_Sym));

	sProps[section].value = index+1;

	return index;
}


UINT32 Relocate(UINT32 *target, REL_MODE_T	mode)
{
	UINT32	val = *target;
	UINT32	off = 0;
	int		i,j;

	if (mode == RM_PTR_ENDIAN)
		CONV32(val);

	//Ищем, к какой кодовой секции относится val
	for (i=SIDX_TEXT; i<=SIDX_BSS; i++)
	{
		if (sProps[i].oldata == NULL)
			continue;
		off = val - sProps[i].oldhdr->sh_addr;
		if ( (off >= 0) && (off <= sProps[i].oldhdr->sh_size) )
		{
			/* Для GCC нужно обрабатывать особый члучай, когда ссылка указывает сразу после
				данных секции. Возникает такой случай при оптимизации циклов заполнения данных,
				когда счётчик итераций подменяется счётчиком адресов. */
			if ( off == sProps[i].oldhdr->sh_size )
			{	
				// Найдём соответствующую секцию
				for (j=0; j<PROPS_CNT; j++)
				{
					if (sProps[j].oldata == NULL)
						continue;

					/* Если секция числится среди обрабатываемых и является ALLOC,
						то символ нельзя приписывать к предыдущей секции */
					if ( (sProps[j].oldhdr->sh_addr == val) && ((sProps[j].flags & SHF_ALLOC) !=0 ) )
						break;
					/* Иначе это означает, что ссылка указывает на нефункциональную секцию, и мы
						делаем релокацию её относительно секции, за которой она лежит. */
				}

				if (j != PROPS_CNT)
					continue;

				// Алгоритм не проверен, необходимо отслеживать данный случай
				printf("\tWARNING: Special case for GCC after-section reloc occured at 0x%X\n", val);
			}

			off = sProps[i].newhdr.sh_addr + off;

			if (mode == RM_PTR_ENDIAN)
				CONV32(off);

			*target = off;
			break;
		}


	}

	return off;
}

// Relocates a value by a given virtual address
UINT32 Relocate(UINT32 offset)
{
//	UINT32	val;
	UINT32	off = 0;
	int i;

	//Ищем, к какой кодовой секции относится val
	for (i=SIDX_TEXT; i<=SIDX_BSS; i++)
	{
		if (sProps[i].oldata == NULL)
			continue;
		off = offset - sProps[i].oldhdr->sh_addr;
		if ( (off >= 0) && (off < sProps[i].oldhdr->sh_size) )
		{
			Relocate( (UINT32*)((UINT8*)sProps[i].data + off), RM_PTR_ENDIAN );
			break;
		}
	}

	return off;
}


UINT32 getPLTReference(PLT_ENTRY_OLD_T *plt, UINT32 offset)
{
	UINT32				immed;
	UINT8				shift;

	if (plt->bxpc != PLT_BXPC) offset -= 4; // Если нет перехода из thumb, адрес смещается
	
	immed = (plt->add&0x00FF); // immed_8
	shift = (UINT8)((plt->add&0x0F00)>>8)*2; // shifter

	//ROR
	__asm {
		MOV	eax, immed
		MOV cl,  shift
		ROR eax, cl
		MOV	immed, eax
	}

	// calc the offset in the old image
	return sProps[SIDX_PLT].oldhdr->sh_addr + offset + 12 + (plt->ldr&0x0FFF) + immed;
	
}

UINT32 InitStructs()
{
	char			*shstrtabold;
	Elf32_Shdr		*oldSectionHeaders;
	UINT32			i, j, index;

	// Prepare all the new sections
	oldSectionHeaders = (Elf32_Shdr*)&file[oldHeader->e_shoff];

	#ifndef LILENDIAN
	for (i=1; i<oldHeader->e_shnum; i++)
		switchEndian(&oldSectionHeaders[i]);
	#endif

	// Get the shstrtab
	shstrtabold = (char*)&file[oldSectionHeaders[oldHeader->e_shstrndx].sh_offset];

	j = 1;
	for (i=0;i<PROPS_CNT;i++)
	{
		index = findSectionIdxByName(sProps[i].name, oldSectionHeaders, shstrtabold);
		if (index==0)
		{
			if (sProps[i].prop == SPROP_LEAVE)
			{
				//sProps[i].data = NULL;
				sProps[i].idx = 0xFF;
			} else {
				sProps[i].newhdr.sh_flags = sProps[i].flags;
				sProps[i].newhdr.sh_type = sProps[i].type;
			}
	
			sProps[i].oldidx = 0;
			sProps[i].oldata = NULL;
			sProps[i].newhdr.sh_size = 0;
			continue;
		}

		sProps[i].oldhdr = (Elf32_Shdr*)&oldSectionHeaders[index];
		sProps[i].oldata = (void*)&file[oldSectionHeaders[index].sh_offset];
		sProps[i].newhdr.sh_flags = sProps[i].flags;
		sProps[i].newhdr.sh_type = sProps[i].type;
		sProps[i].newhdr.sh_entsize = sProps[i].oldhdr->sh_entsize;
		sProps[i].oldidx = index;
		//sProps[i].newhdr.sh_addralign = 4;
		//sProps[i].idx = j++;

		//sProps[i].offset = (UINT32)(sProps[i].data - elfSectionHeaders[index].sh_offset);

		/*printf("Section #%d  %s\n  type: %d\n  newidx: %d\n offset: 0x%X\n\n", 
					index, sProps[i].name, elfSectionHeaders[index].sh_type,
					sProps[i].newidx, sProps[i].offset);*/
	}

	dynstr[0] = 0; // null string
	sProps[SIDX_DYNSTR].data = dynstr;
	sProps[SIDX_DYNSTR].newhdr.sh_size = 0;

	shstrtab[0] = 0; // null string
	sProps[SIDX_SHSTRTAB].data = shstrtab;
	sProps[SIDX_SHSTRTAB].newhdr.sh_size = 0;


	// Temporary fix
	sProps[SIDX_IMPORTS].newhdr.sh_entsize = 0x10;


	// Endianness
	#ifndef LILENDIAN
	Elf32_Sym 			*sym = (Elf32_Sym*)sProps[SIDX_DYNSYM].oldata;
	Elf32_Rel			*rel;
	for ( i = 0; i < sProps[SIDX_DYNSYM].oldhdr->sh_size; i+=sizeof(Elf32_Sym), sym++ )
		switchEndian(sym);


	rel = (Elf32_Rel*)sProps[SIDX_REL_PLT].oldata;
	if (rel != NULL)
		for ( i=0; i<sProps[SIDX_REL_PLT].oldhdr->sh_size; i+=sizeof(Elf32_Rel), rel++ )
			switchEndian(rel);

	rel = (Elf32_Rel*)sProps[SIDX_REL_DYN].oldata;
	if (rel != NULL)
		for (i=0; i<sProps[SIDX_REL_DYN].oldhdr->sh_size; i+=sizeof(Elf32_Rel), rel++ )
			switchEndian(rel);

	#endif

	return 1;

}

// .text, .data and .rodata init
UINT32 AddCode()
{
	int i;
	int aligned_size;
	
	for (i=SIDX_TEXT; i<=SIDX_DATA; i++)
	{
		if (sProps[i].oldata == NULL)
			continue;
		sProps[i].newhdr.sh_addr = Addr;
		sProps[i].newhdr.sh_offset = Off;
		sProps[i].data = (void*)&result[Off];
		sProps[i].idx = curIdx++;

		// секции с данными могут сбить выравнивание
		aligned_size = (sProps[i].oldhdr->sh_size+3)&(~3);

			INC_OA(aligned_size);

		// .text size is not affected
		sProps[i].newhdr.sh_size = aligned_size;

		memcpy(sProps[i].data, &file[sProps[i].oldhdr->sh_offset], aligned_size);
		

	}

	return 1;
}

// .bss is an imagainary section, meaning it is not present in the file, only in the final image
// We should place it in the very end of the allocated image
UINT32 AddBSS()
{

	if (sProps[SIDX_BSS].oldata == NULL)
		return 1;

	sProps[SIDX_BSS].newhdr.sh_addr = Addr;
	sProps[SIDX_BSS].newhdr.sh_offset = Off;
	sProps[SIDX_BSS].newhdr.sh_size = sProps[SIDX_BSS].oldhdr->sh_size;
	sProps[SIDX_BSS].data = (void*)&result[Off];
	sProps[SIDX_BSS].idx = curIdx++;

	// We dont need to increase offset and/or address anymore

	return 1;
}

// Counts the useful symbols for .imports and .dynsym sections reservations
UINT32 CountSymbols()
{
	Elf32_Rel			*relDYN, *relPLT;
	Elf32_Sym			*sym = NULL, *sym2, *symtab;
	PLT_ENTRY_OLD_T		*oldplt;
	UINT32				*oldata;
	UINT32				offset, refnum=0;
	UINT32				reflist[256];
	UINT32      		i,j;
	UINT32				ldroff;

	//Подсчитаем кол-во внешних символов и внутренних с филтрацией по релокациям (для обычных эльфов, см. Notes.txt)
	 
	// Всем записям в .plt соответствуют внешние символы, онако могут быть записи, не используемые
	// в коде (см. Notes.txt, п. Relocations)
	// Подсчитаем кол-во используемых записей в PLT
	pltCnt = 0; internalCnt = 0; externalCnt = 0; extRelCnt = 0; extPltCnt = 0;

	symtab = (Elf32_Sym*)sProps[SIDX_DYNSYM].oldata; // Старая таблица символов для релокаций (+лишнего куча)
	relPLT = (Elf32_Rel*)sProps[SIDX_REL_PLT].oldata;
	oldata = (UINT32*)sProps[SIDX_PLT].oldata;
	j = 0;
	if (oldata != NULL)
	{
		oldplt = (PLT_ENTRY_OLD_T*)((UINT8*)oldata + PLT_INIT_SIZE);
		offset = PLT_INIT_SIZE;

		while (offset != sProps[SIDX_PLT].oldhdr->sh_size)
		{
			if (E32(oldplt->bxpc) == INVALID_BXPC) //no bxpc
				//define INVALID_BXPC 0xE28FC600
			{
				oldplt = (PLT_ENTRY_OLD_T*)((UINT32*)oldplt-1);
			}

			#ifndef LILENDIAN
			switchEndian(oldplt);
			#endif

			ldroff = getPLTReference(oldplt, offset);

			// find the RELPLT entry for thr ldroff
			for (i=0; i<sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel); i++)
			{		
				sym = &symtab[ELF32_R_SYM( relPLT[i].r_info )];
#ifdef _DEBUGV_
	if ( j==76 && i==76) {
	printf("CountSymbols: REL_PLT0[%i], name= %s, val=%x, ldroff = x%X, r_offset = x%X, oldplt=x%X, offset=x%X \n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value, ldroff, relPLT[i].r_offset, oldplt, offset );
//	getch(); }
#endif
				if (relPLT[i].r_offset == ldroff)
				{
					break;
				}
			}
				
			assert( i>=0 && i<sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel) );

			refnum = findRefs( offset + sProps[SIDX_PLT].oldhdr->sh_offset, reflist);

#ifdef _DEBUG
	printf("CountSymbols: REL_PLT[%i,%i], name= %s, val=%x, refnum = %d\n", j,i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value, refnum );
#endif

			if ( refnum != 0 )
			{
				if ( sym->st_value > 1 ) internalCnt++;
				else externalCnt++;
				pltCnt++;	// используемый внешний импорт
			}
			else	// неиспользуемый 
			{
				//if (oldplt->bxpc != 0x7847C046) offset -= 4; // Если нет перехода из thumb, адрес смещается
				// Найдена неиспользуемая запись PLT, и надо пометить соответствующий ей символ
				// В неиспользуемых 

				sym->st_size = 1; //метка для неиспользуемого импорта в PLT
			}

			j++;
			oldplt++;
			offset = (UINT32)((UINT8*)oldplt - (UINT8*)oldata);
		}
	}

	extRelCnt = extPltCnt = externalCnt;


	// Ищем релокации в .rel.dyn, которым соответствуют внешние символы
	relDYN = (Elf32_Rel*)sProps[SIDX_REL_DYN].oldata;

	// REL_DYN может и не быть в простейших эльфах
	if ( relDYN != NULL )
 	for (i=0; i<sProps[SIDX_REL_DYN].oldhdr->sh_size / sizeof(Elf32_Rel); i++)
	{
		j = ELF32_R_SYM(relDYN[i].r_info);
		if (j==0)
		{
			symLsRelCnt++;
			continue;
		}
		sym = &symtab[j];

#ifdef _DEBUG
		printf("CountSymbols: REL_DYN[%i], name= %s, val=%x\n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value );
#endif

		// Импортируется ли символ
		if ( (sym->st_value == 0) || (sym->st_value == 1) )
		{
			// И нет ли его уже в .rel.plt-символах
			if (sym->st_size != 1)
			{
				for (j=0; j < sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel); j++)
				{
					sym2 = &symtab[ELF32_R_SYM( relPLT[j].r_info )];
					if (sym2->st_name == sym->st_name)
						externalCnt--;
				}
			} else sym->st_size = 0;

			externalCnt++;
			extRelCnt++;

		}
		else //if (Config.shared==0) 
				internalCnt++;
	}

#ifdef _DEBUG
	printf("CountSymbols: extRelCnt = %d, externalCnt = %d, pltCnt = %d, internalCnt = %d, symLsRelCnt = %d, extPltCnt = %d\n",
			extRelCnt, externalCnt, pltCnt, internalCnt, symLsRelCnt, extPltCnt);
#endif


	return 0;
}


// Builds the .plt, .got, .rel.plt section
UINT32 BuildPLT()
{
	UINT32				i,j,g;
	UINT32				index;
	UINT32				*oldata;
	UINT32				offset, refnum=0;
	UINT32				ldroff, newoff;
	UINT32				reflist[256];
//	UINT32				immed;
//	UINT8				shift;

	Elf32_Rel			*oldrelDYN, *oldrelPLT, *newrel;
	Elf32_Sym			*sym = NULL, *symtab;

	PLT_ENTRY_OLD_T		*oldplt;
	PLT_ENTRY_NEW_T		*newplt;

	if (sProps[SIDX_PLT].oldata==NULL)
	{
		ReserveDynamicSection();
		return 1;
	}

	// newoff - глобальный оффсет для GOT entry

	// Инициализация .plt
	sProps[SIDX_PLT].newhdr.sh_addr = Addr;
	sProps[SIDX_PLT].newhdr.sh_offset = Off;

	sProps[SIDX_PLT].data = (void*)&result[Off];
	sProps[SIDX_PLT].idx = curIdx++;
	
	oldata = (UINT32*)sProps[SIDX_PLT].oldata;

	newplt = (PLT_ENTRY_NEW_T*)sProps[SIDX_PLT].data;
	oldplt = (PLT_ENTRY_OLD_T*)((UINT8*)oldata + PLT_INIT_SIZE);
	offset = PLT_INIT_SIZE;

	sProps[SIDX_PLT].newhdr.sh_size = pltCnt * sizeof(PLT_ENTRY_NEW_T);
		
		INC_OA( sProps[SIDX_PLT].newhdr.sh_size );


	// Инициализация .got
	sProps[SIDX_GOT].newhdr.sh_addr = Addr;
	sProps[SIDX_GOT].newhdr.sh_offset = Off;
	sProps[SIDX_GOT].data = (void*)&result[Off];
	sProps[SIDX_GOT].idx = curIdx++;
	sProps[SIDX_GOT].newhdr.sh_size = pltCnt * sizeof(UINT32);

		// GOT присутствует в образе, но отсутствует в файле!
		INC_A( sProps[SIDX_GOT].newhdr.sh_size );

		// tim apple: пока засунул в файл. для некоторых локальных записей там хранятся вирт-адреса
		// потом переделаю через абсолютные релокации (сейчас относительные)
		//INC_OA( sProps[SIDX_GOT].newhdr.sh_size ); // !!!

	// Инициализация .bss
	AddBSS();

	// Резервирование .dynamic
	ReserveDynamicSection();

	// Инициализация .rel.plt
	sProps[SIDX_REL_PLT].newhdr.sh_addr = 0;
	sProps[SIDX_REL_PLT].newhdr.sh_offset = Off;
	sProps[SIDX_REL_PLT].data = (void*)&result[Off];
	sProps[SIDX_REL_PLT].idx = curIdx++;
	sProps[SIDX_REL_PLT].newhdr.sh_size = extRelCnt * sizeof(Elf32_Rel); // Записям в .rel.plt соответствуют только импорты
	
		INC_O( sProps[SIDX_REL_PLT].newhdr.sh_size );

	// Все символы, относящиеся к релокациям в .rel.plt точно будут импортами
	sProps[SIDX_IMPORTS].newhdr.sh_addr = 0;
	sProps[SIDX_IMPORTS].newhdr.sh_offset = Off;
	sProps[SIDX_IMPORTS].data = (void*)&result[Off];
	sProps[SIDX_IMPORTS].idx = curIdx++;
	sProps[SIDX_IMPORTS].newhdr.sh_size = PROP_SZ_UNKNOWN; //Размер прибавляется в addSymbol

	symtab = (Elf32_Sym*)sProps[SIDX_DYNSYM].oldata;
	oldrelPLT = (Elf32_Rel*)sProps[SIDX_REL_PLT].oldata;
	oldplt = (PLT_ENTRY_OLD_T*)((UINT8*)oldata + PLT_INIT_SIZE);

	newrel = (Elf32_Rel*)sProps[SIDX_REL_PLT].data;

	j = 0; // счетчик используемых PLT
	g = 0; // счетчик используемых GOT
	offset = PLT_INIT_SIZE;
	while (offset != sProps[SIDX_PLT].oldhdr->sh_size)
	{
		if (oldplt->bxpc == INVALID_BXPC) //no bxpc
		{
			oldplt = (PLT_ENTRY_OLD_T*)((UINT32*)oldplt-1);
		}		

		ldroff = getPLTReference(oldplt, offset);

		// find the RELPLT entry for thr ldroff
		for (i=0; i<sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel); i++)
		{
			index = ELF32_R_SYM(oldrelPLT[i].r_info);
			sym = &symtab[ index ];

			if ( oldrelPLT[i].r_offset == ldroff )
			{
				break;
			}
		}
		assert( i>=0 && i <sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel)  );

		refnum = findRefs( offset + sProps[SIDX_PLT].oldhdr->sh_addr, reflist);

#ifdef _DEBUG
		printf("BuildPLT: REL_PLT[%i], name = %s, val = %x, refs = %d\n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value, refnum );
#endif

		// начинаем формировать наш .imports
		// только используемые 
		if ( refnum != 0 )
		{
			
			newoff = sProps[SIDX_GOT].newhdr.sh_addr + g*sizeof(UINT32);

			// только импортируемые
			if ( sym->st_value == 0 )
			{
				newrel->r_offset = newoff;
				index = addSymbol( index, SIDX_IMPORTS );
				newrel->r_info = ELF32_R_INFO(index, ELF32_R_TYPE(oldrelPLT[i].r_info));
				newrel++;
			}

			// оффсет для LDR
			newoff -= sProps[SIDX_PLT].newhdr.sh_addr 
						+ ((UINT8*)newplt - (UINT8*)sProps[SIDX_PLT].data) + 4 + 8; 
			//+4 - смещение внутри PLT entry
			//-8 - учёт, что смещение идёт через PC
			

			assert((newoff<=0xFFF) && (newoff>=0));

			newplt->bxpc = PLT_BXPC;
			newplt->bxlr = E32(0xE12FFF1C);
			newplt->ldr =  E32(0xE59FC000 | newoff); //we need to calc got off here!

			//Правим референсы
			for (i=0; i<refnum; i++)
			{
				/* В reflist содержатся оффсеты внутри старого .text */
				ldroff = (UINT32)sProps[SIDX_TEXT].data + reflist[i];
				#ifdef LILENDIAN
				UINT32	bl = makeBL( 
										   sProps[SIDX_TEXT].newhdr.sh_addr + reflist[i],
										   sProps[SIDX_PLT].newhdr.sh_addr + ((UINT32)newplt-(UINT32)sProps[SIDX_PLT].data),
										   'T' 
										 );

				
				*(UINT32*)ldroff = E16(bl) | (E16(bl>>16)<<16);
				#else
				*(UINT32*)ldroff = makeBL( 
										   sProps[SIDX_TEXT].newhdr.sh_addr + reflist[i],
										   sProps[SIDX_PLT].newhdr.sh_addr 
												+ (UINT32)((UINT8*)newplt-(UINT8*)sProps[SIDX_PLT].data),
										   'T' 
										 );
				#endif
			}

			newplt++;
			j++; g++;
		}
		
		oldplt++;
		offset = (UINT32)((UINT8*)oldplt - (UINT8*)oldata);
	}


	// Теперь нужно дополнить внешними символами и релокациями, ранее бывшими в .dynsym, .rel.dyn
	oldrelDYN = (Elf32_Rel*)sProps[SIDX_REL_DYN].oldata;

	// REL_DYN может и не быть в простейших эльфах
	if ( oldrelDYN != NULL )
	{
		for (i=0; i<sProps[SIDX_REL_DYN].oldhdr->sh_size / sizeof(Elf32_Rel); i++)
		{
			j = ELF32_R_SYM(oldrelDYN[i].r_info);
			if (j==0) continue;
			sym = &symtab[j];

			// Импортируется ли символ
			if ( (sym->st_value == 0) || (sym->st_value == 1) )
			{
#ifdef _DEBUG
		printf("BuildPLT: REL_DYN[%i], name= %s, val=%x\n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value );
#endif

				index = addSymbol(j, SIDX_IMPORTS);
				newrel->r_info  = ELF32_R_INFO( index, ELF32_R_TYPE(oldrelDYN[i].r_info) );
				newrel->r_offset = oldrelDYN[i].r_offset;
				Relocate( (UINT32*)&newrel->r_offset, RM_PTR );
				newrel++;
			}
		}
	}

	return 1;
}

// Строит .rel.dyn и .dynsym для внутренних символов
UINT32 BuildLocalRels()
{
	UINT32				i, j, g;
	Elf32_Rel			*oldrel, *newrel;
	UINT32				index;
	Elf32_Sym			*sym, *symtab;
	UINT32				*oldata;
	Elf32_Rel			*relPLT;
	PLT_ENTRY_OLD_T		*oldplt;
	UINT32				offset, refnum=0;
	UINT32				ldroff, newoff;
	UINT32				reflist[256];

	
	//oldrel = (Elf32_Rel*)sProps[SIDX_REL_DYN].oldata;

	// REL_DYN может и не быть в простейших эльфах
	//if (oldrel == NULL) return 1;
	if ( internalCnt+symLsRelCnt == 0 ) return 1;

	// Инициализация .rel.dyn
	sProps[SIDX_REL_DYN].data = (void*)&result[Off];
	sProps[SIDX_REL_DYN].idx = curIdx++;
	sProps[SIDX_REL_DYN].newhdr.sh_offset = Off;
	sProps[SIDX_REL_DYN].newhdr.sh_addr   = 0;
	sProps[SIDX_REL_DYN].newhdr.sh_size = (internalCnt+symLsRelCnt)*sizeof(Elf32_Rel); //??

		INC_O((internalCnt+symLsRelCnt)*sizeof(Elf32_Rel));
		
	// Инициализация .dynsym
	sProps[SIDX_DYNSYM].newhdr.sh_addr = 0;
	sProps[SIDX_DYNSYM].newhdr.sh_offset = Off;
	sProps[SIDX_DYNSYM].data = (void*)&result[Off];
	sProps[SIDX_DYNSYM].idx = curIdx++;
	sProps[SIDX_DYNSYM].newhdr.sh_size = PROP_SZ_UNKNOWN;

	oldrel = (Elf32_Rel*)sProps[SIDX_REL_DYN].oldata;
	newrel = (Elf32_Rel*)sProps[SIDX_REL_DYN].data;
	symtab = (Elf32_Sym*)sProps[SIDX_DYNSYM].oldata;

	// Добавляем безсимвольные релокации, а также локальные неимпортируемые
	if (oldrel != NULL) 
	for (i=0; i<sProps[SIDX_REL_DYN].oldhdr->sh_size / sizeof(Elf32_Rel); i++, oldrel++)
	{
		index = ELF32_R_SYM(oldrel->r_info);

		if (index == 0) // Безсимвольные релокации
		{
			newrel->r_info = ELF32_R_INFO( 0, ELF32_R_TYPE(oldrel->r_info) );
		}
		else 
		{
			sym = &symtab[index];

#ifdef _DEBUG
		printf("BuildLocalRels: RELDYN_SYM[%i], name= %s, val=%x\n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value );
#endif

			if ( (sym->st_value != 0) && (sym->st_value != 1) ) // Локальные неимпортируемые
			{
				index = addSymbol(index, SIDX_DYNSYM, FALSE);
				newrel->r_info = ELF32_R_INFO( index, ELF32_R_TYPE(oldrel->r_info) );
			} else
				continue;
		}

		newrel->r_offset = oldrel->r_offset;
		Relocate( (UINT32*)&newrel->r_offset, RM_PTR );

#ifdef _DEBUGV
		printf("BuildLocalRels: REL_DYN[%i], r_offset = x%X, r_info = x%X\n", i, 
				newrel->r_offset, newrel->r_info );
#endif

		if (ELF32_R_TYPE(oldrel->r_info) == R_ARM_RELATIVE)
		{
			Relocate( oldrel->r_offset );
		}
		
		newrel++;
 	}

	// релокации для локальных функций у которых создана PLT (хз зачем GCC так делает)
	oldata = (UINT32*)sProps[SIDX_PLT].oldata;	
	relPLT = (Elf32_Rel*)sProps[SIDX_REL_PLT].oldata;

	g = 0;
	j = 0;
	if (oldata != NULL)
	{
		oldplt = (PLT_ENTRY_OLD_T*)((UINT8*)oldata + PLT_INIT_SIZE);
		offset = PLT_INIT_SIZE;

		while (offset != sProps[SIDX_PLT].oldhdr->sh_size)
		{
			// !!! switchEndian() already used in CountSymbols()
			if ((oldplt->bxpc) == INVALID_BXPC)
			{
				oldplt = (PLT_ENTRY_OLD_T*)((UINT32*)oldplt-1);
			}

			ldroff = getPLTReference(oldplt, offset);

			// find the RELPLT entry for thr ldroff
			for (i=0; i<sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel); i++)
			{		
				index = ELF32_R_SYM(relPLT[i].r_info);
				sym =  &symtab[index];

#ifdef _DEBUGV
	if ( j==76 && i==76)
	printf("BuildLocalRels: REL_PLT0[%i], name= %s, val=%x, ldroff = x%X, r_offset = x%X, oldplt=x%X, offset=x%X \n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value, ldroff, relPLT[i].r_offset, oldplt, offset );
#endif
				if ( relPLT[i].r_offset == ldroff )
				{
#ifdef _DEBUG
	printf("BuildLocalRels: REL_PLT[%i], name= %s, val=%x\n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value );
#endif
					break;
				}
			}
				
			assert( i>=0 && i<sProps[SIDX_REL_PLT].oldhdr->sh_size / sizeof(Elf32_Rel) );

			refnum = findRefs( offset + sProps[SIDX_PLT].oldhdr->sh_offset, reflist);

#ifdef _DEBUG
	printf("BuildLocalRels: REL_PLT[%i], name = %s, val = x%X, refnum = %d\n", i, &((char*)sProps[SIDX_DYNSTR].oldata)[sym->st_name], sym->st_value, refnum );
#endif

			// local function in PLT
			if ( refnum != 0 && sym->st_value > 1 ) 
			{
				newoff = sProps[SIDX_GOT].newhdr.sh_addr + g*sizeof(UINT32);
				index = addSymbol(index, SIDX_DYNSYM, FALSE);
				newrel->r_info = ELF32_R_INFO( index, R_ARM_ABS32 );
				newrel->r_offset = newoff;

#ifdef _DEBUG
		printf("BuildLocalRels: REL_DYN_GOT[%i], ind = %d, oldval = x%X, newoff = x%X, newval = x%X\n", i, 
			g, sym->st_value, newoff, ((Elf32_Sym*)sProps[SIDX_DYNSYM].data)[index].st_value );
#endif

				// в GOT для локальной функции запишем ее вирт-адресс
				/*newoff = (UINT32)sProps[SIDX_GOT].data + g*sizeof(UINT32);
				*(UINT32*)newoff = E32(sym->st_value);
				Relocate( (UINT32*)newoff, RM_PTR_ENDIAN );

#ifdef _DEBUG
		printf("BuildLocalRels: REL_DYN_GOT[%i], oldoff = x%X, oldval = x%X, newval = x%X\n", i, 
			sProps[SIDX_GOT].oldhdr->sh_addr + j*sizeof(UINT32), sym->st_value, E32(*(UINT32*)newoff) );
#endif*/

				// и добавим безсимвольную релокацию в этот GOT
				/*newoff = sProps[SIDX_GOT].newhdr.sh_addr + g*sizeof(UINT32);
				newrel->r_info = ELF32_R_INFO( 0, R_ARM_RELATIVE );
				newrel->r_offset = newoff;
				
#ifdef _DEBUG
		printf("BuildLocalRels: REL_DYN_GOT[%i], r_offset = x%X, r_info = x%X\n", i, 
				newrel->r_offset, newrel->r_info );
#endif*/

				newrel++;
			}

			if ( refnum != 0 ) { g++; j++; }
			oldplt++;
			offset = (UINT32)((UINT8*)oldplt - (UINT8*)oldata);
		}
	}


	if (sProps[SIDX_DYNSYM].newhdr.sh_size == 0)
	{
		sProps[SIDX_DYNSYM].data = NULL;
		curIdx--;
	}

	return 1;
}

UINT32 BuildSHStrTab()
{
	int i;//, j=0;
	for (i=0; i<PROPS_CNT; i++)
	{
		if (sProps[i].data == NULL) continue;
		//sProps[i].idx = j++;
		sProps[i].newhdr.sh_name = addSHStrData( (char*)sProps[i].name );
	}

	return PlaceStrTab(SIDX_SHSTRTAB);
}


UINT32 BuildSectionHeaders()
{
	int i, j=1;
	Elf32_Shdr	*shdr = (Elf32_Shdr*)&result[Off];

	sProps[SIDX_DYNSTR].idx = curIdx++;
	sProps[SIDX_SHSTRTAB].idx = curIdx++;

	// ToDo: normal linking maybe?
	sProps[SIDX_REL_PLT].newhdr.sh_link = sProps[SIDX_IMPORTS].idx;
	// !!! Temporary !!! SIDX_DYNSYM->SIDX_IMPORTS
	if (sProps[SIDX_DYNSYM].data == NULL) i = SIDX_IMPORTS;
		else i = SIDX_DYNSYM;
	sProps[SIDX_REL_DYN].newhdr.sh_link = sProps[i].idx;
	sProps[SIDX_DYNSYM].newhdr.sh_link = sProps[SIDX_DYNSTR].idx;
	sProps[SIDX_IMPORTS].newhdr.sh_link = sProps[SIDX_DYNSTR].idx;


	// NULL section
	memset(shdr, 0, sizeof(Elf32_Shdr));
	for (i=0; i<PROPS_CNT; i++)
	{
		if (sProps[i].data == NULL) continue;

		shdr[j] = sProps[i].newhdr;
		#ifndef LILENDIAN
		switchEndian(&shdr[j]);
		#endif
		j++;
	}

	newHeader->e_shoff = Off;
	newHeader->e_shnum = (Elf32_Half)j;

	newHeader->e_shstrndx = (Elf32_Half)sProps[SIDX_SHSTRTAB].idx;
		
		INC_OA(j*sizeof(Elf32_Shdr));

	return 1;
}

UINT32 ReserveDynamicSection()
{
	UINT32			i = 0;
	char			*str;
	UINT32			num = STATIC_TAGS_NUM;

	// Инициализация dynamic секции
	sProps[SIDX_DYNAMIC].data = (void*)&result[Off];
	sProps[SIDX_DYNAMIC].idx = curIdx++;
	sProps[SIDX_DYNAMIC].newhdr.sh_offset = Off;
	sProps[SIDX_DYNAMIC].newhdr.sh_addr   = 0;

	//Elf32_Dyn	*dyn = (Elf32_Dyn*)sProps[SIDX_DYNAMIC].data;
	Elf32_Dyn	*dyn = (Elf32_Dyn*)sProps[SIDX_DYNAMIC].oldata;

	#ifndef LILENDIAN
	while (dyn[i].d_tag != 0)
	{
		CONV32(dyn[i].d_tag);
		CONV32(dyn[i].d_val);
		i++;
	}
	#endif

	// Подсчитываем NEEDED теги
	for (i=0; i<sProps[SIDX_DYNAMIC].oldhdr->sh_size/sizeof(Elf32_Dyn); i++)
	{
		if (dyn->d_tag == DT_NEEDED)
		{
			str = removePath( &((char*)sProps[SIDX_DYNSTR].oldata)[dyn->d_val] );
			if (strcmp(str, DEFLIBNAME) != 0) num++;
		}
		dyn++;
	}

	sProps[SIDX_DYNAMIC].newhdr.sh_size = num*sizeof(Elf32_Dyn);
	
		INC_O(num*sizeof(Elf32_Dyn));

	return 1;
}


// Заполнение .dynamic секции
UINT32 BuildDynamicTags()
{
	UINT32			i;
	char			*str;

/*	//инициализация dynamic секции
	sProps[SIDX_DYNAMIC].data = (void*)&result[Off];
	sProps[SIDX_DYNAMIC].newhdr.sh_offset = Off;
	sProps[SIDX_DYNAMIC].newhdr.sh_addr   = 0;*/

	Elf32_Dyn		*dyn = (Elf32_Dyn*)sProps[SIDX_DYNAMIC].data;
	Elf32_Dyn		*olddyn = (Elf32_Dyn*)sProps[SIDX_DYNAMIC].oldata;

	// Копируем NEEDED теги
	for (i=0; i<sProps[SIDX_DYNAMIC].oldhdr->sh_size/sizeof(Elf32_Dyn); i++)
	{
		if (olddyn->d_tag == DT_NEEDED)
		{
			str = &((char*)sProps[SIDX_DYNSTR].oldata)[olddyn->d_val]; // paths already removed, if any
			if (strcmp(str, DEFLIBNAME) != 0)
			{
				dyn->d_tag = DT_NEEDED;
				dyn->d_val = addStrData(str);
				dyn++;
			}
		}
		olddyn++;
	}

	//SYMTAB PLTREL JMPREL PLTRELSZ REL RELSZ STRTAB
	// Total: 7 + 1
	if (sProps[SIDX_DYNSYM].oldata != NULL)
	{
		dyn->d_tag = DT_SYMTAB;
		dyn->d_val = sProps[SIDX_DYNSYM].newhdr.sh_offset;
		dyn++;
	}
	
	if (sProps[SIDX_REL_PLT].oldata != NULL)
	{
		dyn->d_tag = DT_PLTREL;
		dyn->d_val = DT_REL;
		dyn++;

		dyn->d_tag = DT_JMPREL;
		dyn->d_val = sProps[SIDX_REL_PLT].newhdr.sh_offset;
		dyn++;

		dyn->d_tag = DT_PLTRELSZ;
		dyn->d_val = sProps[SIDX_REL_PLT].newhdr.sh_size;
		dyn++;
	}

	if (sProps[SIDX_REL_DYN].oldata != NULL)
	{
		dyn->d_tag = DT_REL;
		dyn->d_val = sProps[SIDX_REL_DYN].newhdr.sh_offset;
		dyn++;

		dyn->d_tag = DT_RELSZ;
		dyn->d_val = sProps[SIDX_REL_DYN].newhdr.sh_size;
		dyn++;
	}

	dyn->d_tag = DT_NONABI_SYMTABNO;
	dyn->d_val = sProps[SIDX_DYNSYM].newhdr.sh_size/sizeof(Elf32_Sym);
	dyn++;

	dyn->d_tag = DT_NONABI_IMPORTS;
	dyn->d_val = sProps[SIDX_IMPORTS].newhdr.sh_offset;
	dyn++;
	
	dyn->d_tag = DT_STRTAB;
	dyn->d_val = Off;
	dyn++;

	dyn->d_tag = DT_STRSZ;
	dyn->d_val = sProps[SIDX_DYNSTR].newhdr.sh_size;
	dyn++;

	dyn->d_tag = DT_NULL;
	dyn->d_val = 0;
	//dyn++;

	#ifndef LILENDIAN
	while (dyn >= (Elf32_Dyn*)sProps[SIDX_DYNAMIC].data)
	{
		CONV32(dyn->d_tag);
		CONV32(dyn->d_val);
		dyn--;
	}
	#endif


	return 1;
}

UINT32 PlaceStrTab(UINT32 index)
{
	sProps[index].newhdr.sh_offset = Off;
	sProps[SIDX_DYNSTR].newhdr.sh_addr = 0;

	memcpy(&result[Off], sProps[index].data, sProps[index].newhdr.sh_size);

		INC_O(sProps[index].newhdr.sh_size);

	return 1;
}

UINT32 InitProgramHeaders()
{
	Elf32_Phdr	*phdr = newPhdr;

	//PHDR
/*	phdr->p_type = PT_PHDR;
	phdr->p_offset = sizeof(Elf32_Ehdr);
	phdr->p_paddr = phdr->p_vaddr = 0;
	phdr->p_filesz = PHDRS_NUM*sizeof(Elf32_Ehdr);
	phdr->p_memsz = 0;
	phdr->p_flags = PF_R;
	phdr->p_align = 4;
	phdr++;
*/
	//INTERP

	//LOAD
	phdr->p_type = PT_LOAD;
	phdr->p_offset = sProps[SIDX_TEXT].newhdr.sh_offset;
	phdr->p_paddr = phdr->p_vaddr = sProps[SIDX_TEXT].newhdr.sh_addr;
	phdr->p_filesz = sProps[SIDX_TEXT].newhdr.sh_size + 
					 sProps[SIDX_RODATA].newhdr.sh_size + 
					 sProps[SIDX_DATA].newhdr.sh_size + 
					 sProps[SIDX_PLT].newhdr.sh_size;
	// Imaginary sections were not counted in p_filesz, but need memory to be allocated for
	phdr->p_memsz = phdr->p_filesz + sProps[SIDX_GOT].newhdr.sh_size + sProps[SIDX_BSS].newhdr.sh_size;
	phdr->p_flags = PF_R | PF_W | PF_X;
	phdr->p_align = 4;
	#ifndef LILENDIAN
	switchEndian(phdr);
	#endif
	phdr++;

	//DYNAMIC
	phdr->p_type = PT_DYNAMIC;
	phdr->p_offset = sProps[SIDX_DYNAMIC].newhdr.sh_offset;
	phdr->p_paddr = phdr->p_vaddr = 0;
	phdr->p_filesz = sProps[SIDX_DYNAMIC].newhdr.sh_size +
					 sProps[SIDX_REL_PLT].newhdr.sh_size +
					 sProps[SIDX_IMPORTS].newhdr.sh_size +
					 sProps[SIDX_REL_DYN].newhdr.sh_size +
					 sProps[SIDX_DYNSYM].newhdr.sh_size +
					 sProps[SIDX_DYNSTR].newhdr.sh_size;
	phdr->p_memsz = 0;
	phdr->p_flags = PF_W;
	phdr->p_align = 0;
	#ifndef LILENDIAN
	switchEndian(phdr);
	#endif
	//phdr++;

	newHeader->e_phnum = PHDRS_NUM;

	return 1;
}


int	printWelcome()
{
	printf("PostLinker utility v1.2.31\n by Andy51 2009-2010\n");
	return 0;
}

int printHelp()
{
	printf(	"Usage: postlink [options] input_file\n"
			"Options:\n");
	printf( "  -o <filename>\t\tSpecifies the output file name (default: \"res.elf\")\n" );
	printf( "  -shared, -s\t\tSpecifies that the input file is a Shared library.\n"
				"\t\t\tDo not forget this flag when you want to build an *.so!\n"
		);
	printf(	"  -stub\t\t\t(not working) Generates a stub library (.sa) along with\n"
				"\t\t\ta shared lib. Use it only with -shared\n"
		);
	printf(	"  -def <def file>\tSpecifies the exported symbols list file. Use it with\n"
				"\t\t\t-shared key\n"
		);
	printf(	"  -def <def file>\tSpecifies the loader API symbols list file. Use it with\n"
				"\t\t\t-stdlib key\n"
		);
	printf(	"  -stdlib\t\tPostlinker will generate a standard elfloader library,\n"
			"\t\t\tusing input files. Use *.sym files either in ADS-like,\n"
			"\t\t\tor in GCC-like format\n"
		);
	printf(	"  -fw <firmware>\tTarget firmware for the library (string < 24 symbols\n"
			"\t\t\tlong)\n"
		);
	printf(	"  -v <version>\t\tVersion of the library (string < 8 symbols long)\n"
		);
	printf( "  -header <path>\tYou should specify an *.h file from ELF SDK that will\n"
			"\t\t\tbe used to resolve const names to IDs\n\n"
		);

	return 0;
}

void initConfig()
{
	strcpy(Config.output, "res.elf");
}

bool fileExists(char *path)
{
	FILE		*f;

	f = fopen(path, "r");
	
	if(f == NULL)
		return false;

	fclose(f);

	return true;
}


int parseOption(char** argv, int index)
{
	
	if (argv[index][0]!='-')
	{
		if (fileExists(argv[index]))
		{
			path  = argv[index];

			return index+1;
		}
		else
		{
			printf("*ERROR: input file %s not found\n", argv[index]);

			return -1;
		}
	}

	if (strcmp(argv[index], "-shared")==0 || strcmp(argv[index], "-s")==0)
	{
		printf("Shared library build mode\n");
		Config.shared = 1;
		return index+1;
	}

	if (strcmp(argv[index], "-def")==0)
	{
		if (fileExists(argv[index+1]))
		{
			strcpy(Config.deffile, argv[index+1]);
			return index+2;
		}
		else
		{
			printf("*ERROR: input DEF %s not found\n", argv[index+1]);
			return -1;
		}
	}

	if (strcmp(argv[index], "-stdlib")==0)
	{
		Config.stdlib = 1;
		return index+1;
	}

	if (strcmp(argv[index], "-fw")==0)
	{
		strcpy(Config.firmware, argv[index+1]);
		return index+2;
	}

	if (strcmp(argv[index], "-v")==0)
	{
		strcpy(Config.version, argv[index+1]);
		return index+2;
	}

	if (strcmp(argv[index], "-header")==0)
	{
		if (fileExists(argv[index+1]))
		{
			strcpy(Config.header, argv[index+1]);
			return index+2;
		}
		else
		{
			printf("*ERROR: input consts.h header %s not found\n", argv[index+1]);
			return -1;
		}
	}

	if (strcmp(argv[index], "-o")==0)
	{
		strcpy(Config.output, argv[index+1]);
		return index+2;
	}

	// Unknown option
	printf("*WARNING: unknown option \"%s\", ignored\n", argv[index]);
	
	return index+1;
}

UINT32 findInStrTab(char *strtab, UINT32 size, char *str)
{
	UINT32	i=0;

	while ( i<size )
	{
		if (strcmp(&strtab[i], str) == 0) return i;
		while (strtab[i++]!=0);
	}
	return (UINT32)-1;
}

// Строит .dynsym с экспортами по *.def файлу
UINT32 BuildSharedExports()
{
	UINT32			i;
	UINT32			index, ind2;
	Elf32_Sym		*oldSym;
	char			*strtab;
	

	if (Config.shared == 0) return 1;
	if (sProps[SIDX_DYNSYM].data == NULL)
	{
			// Инициализация .dynsym
		sProps[SIDX_DYNSYM].newhdr.sh_addr = 0;
		sProps[SIDX_DYNSYM].newhdr.sh_offset = Off;
		sProps[SIDX_DYNSYM].data = (void*)&result[Off];
		sProps[SIDX_DYNSYM].idx = curIdx++;
		sProps[SIDX_DYNSYM].newhdr.sh_size = PROP_SZ_UNKNOWN;
	}

	// Загружаем def
	i = prepareDef( Config.deffile, &def );
	if ( !i ) return 2;

	oldSym = (Elf32_Sym*)sProps[SIDX_DYNSYM].oldata;
	strtab =(char*)sProps[SIDX_DYNSTR].oldata;

	for (i=0; i<sProps[SIDX_DYNSYM].oldhdr->sh_size/sizeof(Elf32_Sym); i++, oldSym++)
	{
		if (oldSym->st_name==0) continue;
		//index = findInStrTab(def, size, &strtab[oldSym->st_name]);
		index = findInDef( &def, &strtab[oldSym->st_name]);
		if (index != (-1))
		{
			ind2 = addSymbol(i, SIDX_DYNSYM);

#ifdef _DEBUG
		printf( "export: name = '%s', oldval = x%X, newval = x%X\n", &strtab[oldSym->st_name], 
				oldSym->st_value,
				((Elf32_Sym*)sProps[SIDX_DYNSYM].data)[ind2].st_value );
#endif
		}
	}

	freeDef( &def );

	return 1;

}

char* removePath(char *path)
{
	size_t		i = strlen(path)-1;

	while ( (path[i] != '\\') && (path[i] != '/') && (i != 0) ) i--;

	if (i) strcpy( path, &path[i+1] );

	return path;
}

void switchEndian(Elf32_Ehdr *hdr)
{
	CONV16(hdr->e_type);
	CONV16(hdr->e_machine);
	CONV32(hdr->e_version);
	CONV32(hdr->e_entry);
	CONV32(hdr->e_phoff);
	CONV32(hdr->e_shoff);
	CONV32(hdr->e_flags);
	CONV16(hdr->e_ehsize);
	CONV16(hdr->e_phentsize);
	CONV16(hdr->e_phnum);
	CONV16(hdr->e_shentsize);
	CONV16(hdr->e_shnum);
	CONV16(hdr->e_shstrndx);
}

void switchEndian(Elf32_Phdr *hdr)
{

	CONV32(hdr->p_type);
	CONV32(hdr->p_offset);
	CONV32(hdr->p_vaddr);
	CONV32(hdr->p_paddr);
	CONV32(hdr->p_filesz);
	CONV32(hdr->p_memsz);
	CONV32(hdr->p_flags);
	CONV32(hdr->p_align);
}

void switchEndian(Elf32_Shdr *hdr)
{
	CONV32(hdr->sh_name);
	CONV32(hdr->sh_type);
	CONV32(hdr->sh_flags);
	CONV32(hdr->sh_addr);
	CONV32(hdr->sh_offset);
	CONV32(hdr->sh_size);
	CONV32(hdr->sh_link);
	CONV32(hdr->sh_info);
	CONV32(hdr->sh_addralign);
	CONV32(hdr->sh_entsize);
}

void switchEndian(PLT_ENTRY_OLD_T *plt)
{
//	CONV32(plt->bxpc);
	CONV32(plt->adr);
	CONV32(plt->add);
	CONV32(plt->ldr);
}

void switchEndian(Elf32_Rel *rel)
{
	CONV32(rel->r_offset);
	CONV32(rel->r_info);
}

void switchEndian(Elf32_Sym *sym)
{
	CONV32(sym->st_name);
	CONV32(sym->st_value);
	CONV32(sym->st_size);
	CONV16(sym->st_shndx);
}

void switchEndian(Elf32_Dyn *dyn)
{
	CONV32(dyn->d_tag);
	CONV32(dyn->d_val);
}


UINT32	prepareDef( const char * filename, STR_TABLE_T * def )
{
	FILE			*f;
	UINT32			size;
	UINT32			i;
	INT32			count, n=1;

	def->count = 0;
	def->strings = 0;
	def->strTable = 0;


	f = fopen( filename, "rb" );
	if ( f==0 ) 
	{
		printf( "can't open def '%s'\n", filename );
		return 0; //error
	}
	size = getFileSize( f );
	def->strTable = (char*)malloc( size+1 );
	if ( !def->strTable ) return 0;
	fread( def->strTable, size, 1, f );
	fclose( f );

	count = 0;
	// Преобразуем в null-terminated строки
	for ( i=0; i<size; i++ )
	{
		if ( def->strTable[i] == '\n' || def->strTable[i] == '\r' ) 
			def->strTable[i] = 0;

		if ( n || (def->strTable[i-1] == 0 && def->strTable[i] != 0) )
		{
			n = 0;
			//def->strings[ def->count++ ] = &def->strTable[i];
			count++;
		}
	}

	def->strTable[ size ] = 0;

	def->strings = (char**)malloc( count * sizeof(char*) );
	if ( f==0 )
	{
		printf( "can't allocate mem for def '%s'\n", filename );
		free( def->strTable );
		def->strTable = 0;
		return 0; //error
	}

	def->count = 0;
	n = 1;
	for ( i=0; i<size; i++ )
	{
		if ( n || (def->strTable[i-1] == 0 && def->strTable[i] != 0) )
		{
			n = 0;
			def->strings[ def->count++ ] = &def->strTable[i];
		}
	}

#ifdef _DEBUG
	printf( "names from def '%s':\n", filename );
	for ( i=0; i<def->count; i++ )
		printf( "%s\n", def->strings[i] );
	printf( "\n" );
#endif

	return 1;
}


UINT32	freeDef( STR_TABLE_T * def )
{
	if ( def->strTable ) free( def->strTable );
	def->strTable = 0;

	if ( def->strings ) free( def->strings );
	def->strings = 0;

	def->count = 0;

	return 1;
}

UINT32 findInDef( STR_TABLE_T * def, char * str )
{
	UINT32	i=0;

	while ( i < def->count )
	{
		if ( strcmp( def->strings[i], str ) == 0 ) 
			return i;
		i++;
	}

	return (UINT32)-1;
}

