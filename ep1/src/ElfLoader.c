
#include "ElfLoader.h"

extern const char n_phone[];
extern const char n_platform[];
extern const char n_majorfw[];
extern const char n_minorfw[];

UINT32 loadELF( char *file_uri,  char *params,  void *Library, UINT32 reserve )
{
    UINT32            i,j;
    DL_FS_HANDLE_T    f;
    DL_FS_COUNT_T     read;
	
    Elf32_Ehdr        elfHeader;
    Elf32_Phdr        elfProgramHeaders[NUM_HEADERS];
    
    //char             logbuf[128];

    Elf32_Addr        virtBase   = SYN_NULL, 
                      physBase   = SYN_NULL, 
                      upperAddr  = SYN_NULL, 
                      dynSegment = SYN_NULL;

    UINT32            sumMem  = 0,
                      sumSize = 0;

    Elf32_Word	      dynTags[DT_BIND_NOW+1];
    Elf32_Dyn         tag;
    Elf32_Word	      relType;
    Elf32_Rel	        *relTable;
    
    UINT32            ldrNumSymbols;
    UINT32            elfNumSymbols = 0;
    //UINT32            ldrStrTableSize;
    UINT32            elfStrTableSize = 0;
    Ldr_Sym		       *ldrSymTable;
    Elf32_Sym	       *elfSymTable = SYN_NULL;
    char         		 *ldrStrTable;
    char	           *elfStrTable = SYN_NULL;
    Elf32_Shdr	      elfSectionHeader;
    W_CHAR            wstr[64];

    
    UtilLogStringData("ElfLdr Load Request %s", file_uri);
    
    u_atou((char*)file_uri, wstr);

    f = DL_FsOpenFile(wstr, DL_FS_READ_MODE, 0);
    if(f == DL_FS_HANDLE_INVALID) return ELDR_OPEN_FAILED;

    if( DL_FsReadFile((void*)&elfHeader, sizeof(Elf32_Ehdr), 1, f, &read) != DL_FS_RESULT_SUCCESS )
        return ELDR_READ_HEADER_FAILED;
	
    UtilLogStringData("Elf is loading...\n\nELF header:\n  e_entry  0x%X\n  e_phoff  0x%X\n  e_phnum  %d\n\n",
                      elfHeader.e_entry,
                      elfHeader.e_phoff,
                      elfHeader.e_phnum );
    
    upperAddr = 0;
    virtBase = ~0;

    for( i=0; i<elfHeader.e_phnum; i++ )
    {
        if( DL_FsFSeekFile(f, elfHeader.e_phoff + i*elfHeader.e_phentsize, DL_FS_SEEK_SET) != DL_FS_RESULT_SUCCESS )
            return ELDR_SEEK_FAILED;

        if( DL_FsReadFile((void*)&elfProgramHeaders[i], sizeof(Elf32_Phdr), 1, f, &read) != DL_FS_RESULT_SUCCESS )
            return ELDR_READ_FAILED;

        UtilLogStringData("Segment #%d:\n  p_type  %d\n  p_vaddr  0x%X\n  p_memsz  0x%X\n",
                          i,
                          elfProgramHeaders[i].p_type,
                          elfProgramHeaders[i].p_vaddr,
                          elfProgramHeaders[i].p_memsz );

        if(elfProgramHeaders[i].p_type == PT_LOAD)
        {
            if(elfProgramHeaders[i].p_vaddr < virtBase) 
                virtBase = elfProgramHeaders[i].p_vaddr;

            if((elfProgramHeaders[i].p_vaddr + elfProgramHeaders[i].p_memsz) > upperAddr)
                upperAddr = elfProgramHeaders[i].p_vaddr + elfProgramHeaders[i].p_memsz;
        }
        
        sumMem  += elfProgramHeaders[i].p_memsz;
        sumSize += elfProgramHeaders[i].p_filesz;
    }

	
    physBase = (Elf32_Addr)AFW_AllocateMemory(upperAddr-virtBase);

    memclr((void*)physBase, upperAddr-virtBase);

    UtilLogStringData("\nFinished analysis:\n  Virtual Base  0x%X\n  Mem Needed  0x%X\n  Upper Address  0x%X\n  Summary Mem  0x%X\n  Summary Size  0x%X\n  Physical Base  0x%X\n\n",
                      virtBase,
                      upperAddr-virtBase,
                      upperAddr,
                      sumMem,
                      sumSize,
                      physBase);

    for( i=0; i<elfHeader.e_phnum; i++ )
    {
        switch(elfProgramHeaders[i].p_type)
        {
            case PT_LOAD:
                if( DL_FsFSeekFile(f, elfProgramHeaders[i].p_offset, DL_FS_SEEK_SET) != DL_FS_RESULT_SUCCESS )
                {    
                    AFW_FreeAllocatedMemory((void*)physBase);
                    return ELDR_SEEK_FAILED;
                }

                if( DL_FsReadFile( (void*)(physBase + elfProgramHeaders[i].p_vaddr - virtBase), 
                                   elfProgramHeaders[i].p_filesz, 1, f, &read ) != DL_FS_RESULT_SUCCESS )
                {    
                    AFW_FreeAllocatedMemory((void*)physBase);
                    return ELDR_READ_FAILED;
                }

                UtilLogStringData("  Segment #%d loading  0x%X  0x%d\n\n",   
                                  i,
                                  physBase + elfProgramHeaders[i].p_vaddr - virtBase,
                                  elfProgramHeaders[i].p_filesz );
            break;
            case PT_DYNAMIC:
                dynSegment = (Elf32_Addr)AFW_AllocateMemory(elfProgramHeaders[i].p_filesz);

                if( DL_FsFSeekFile(f, elfProgramHeaders[i].p_offset, DL_FS_SEEK_SET) != DL_FS_RESULT_SUCCESS )
                {
                    AFW_FreeAllocatedMemory((void*)physBase);
                    AFW_FreeAllocatedMemory((void*)dynSegment);
                    return ELDR_SEEK_FAILED;
                }
                    
                if( DL_FsReadFile((void*)dynSegment, elfProgramHeaders[i].p_filesz, 1, f, &read) != DL_FS_RESULT_SUCCESS )
                {
                    AFW_FreeAllocatedMemory((void*)physBase);
                    AFW_FreeAllocatedMemory((void*)dynSegment);
                    return ELDR_READ_FAILED;
                }

                j = 0;
                
                // Загрузим теги из динамического сегмента
                do
                {
                    tag = ((Elf32_Dyn*)dynSegment)[j++];

                    if(tag.d_tag<=DT_BIND_NOW)
                    {
                        dynTags[tag.d_tag] = tag.d_val;
                    }
                }while(tag.d_tag);

                UtilLogStringData("Relocation start\n DT_REL  0x%X\n DT_RELSZ  %d\n", dynTags[DT_REL], dynTags[DT_RELSZ]);

                relTable = (Elf32_Rel*)(dynSegment + dynTags[DT_REL] - elfProgramHeaders[i].p_vaddr);

                j = 0;

                while(j*sizeof(Elf32_Rel) < dynTags[DT_RELSZ])
                {
                    relType = ELF32_R_TYPE(relTable[j].r_info);
                    
                    UtilLogStringData(" Reloc #%d\n  Type  %d\n  Off  0x%X\n",
                                      j,
                                      relType,
                                      relTable[j].r_offset);

                    if(relType==R_ARM_RABS32)
                    {
                        UtilLogStringData(" R_ARM_RABS32\n  Old  0x%X\n  New  0x%X\n", 
                                          *((UINT32*)(physBase + relTable[j].r_offset - virtBase)), 
                                          *((UINT32*)(physBase + relTable[j].r_offset - virtBase))+physBase-virtBase);

                        *((UINT32*)(physBase + relTable[j].r_offset - virtBase)) += physBase-virtBase;
                    }
                    
                    j++;
                }

            break;
        }
    }

    if(dynSegment != SYN_NULL)	AFW_FreeAllocatedMemory((void*)dynSegment);

// Правка API вызовов

/*/////WIN32 Chunk/////////
	FILE		*flib;
	flib = fopen("library.bin", "rb");
	fread(&ldrNumSymbols, sizeof(int), 1, flib);
	fread(&ldrStrTableSize, sizeof(int), 1, flib);

	ldrSymTable = (Ldr_Sym*)malloc(sizeof(Ldr_Sym)*ldrNumSymbols);
	fread(ldrSymTable, sizeof(Ldr_Sym), ldrNumSymbols, flib);

	ldrStrTable = (char*)malloc(sizeof(char)*ldrStrTableSize);
	fread(ldrStrTable, sizeof(char), ldrStrTableSize, flib);
	fclose(flib);
*//////////////////////////

    ldrNumSymbols = ((UINT32*)Library)[0];
    //ldrStrTableSize = ((UINT32*)Library)[1];
    ldrSymTable = (Ldr_Sym*)&((UINT32*)Library)[1];
    ldrStrTable = (char*)&ldrSymTable[ldrNumSymbols];

///////////////////////////


// Загружаем elfSymTable и elfStrTable
    for( i=0; i<elfHeader.e_shnum; i++ )
    {
        if( DL_FsFSeekFile(f, elfHeader.e_shoff + i*elfHeader.e_shentsize, DL_FS_SEEK_SET) != DL_FS_RESULT_SUCCESS )
        {
            AFW_FreeAllocatedMemory((void*)physBase);
            return ELDR_SEEK_FAILED;
        }
        if( DL_FsReadFile((void*)&elfSectionHeader, sizeof(Elf32_Shdr), 1, f, &read) != DL_FS_RESULT_SUCCESS )
        {
            AFW_FreeAllocatedMemory((void*)physBase);
            return ELDR_READ_FAILED;
        }

        UtilLogStringData("Section #%d:\n  sh_type %d\n  sh_offset 0x%X\n  sh_size  0x%X\n",
                          i,
                          elfSectionHeader.sh_type,
                          elfSectionHeader.sh_offset,
                          elfSectionHeader.sh_size );

#define SHT_SYMTAB 2 //The section holds a symbol table.
#define SHT_STRTAB 3 //The section holds a string table.
        if(elfSectionHeader.sh_type == SHT_SYMTAB)
        {
            elfSymTable = (Elf32_Sym*)AFW_AllocateMemory(elfSectionHeader.sh_size);
            if( DL_FsFSeekFile(f, elfSectionHeader.sh_offset, DL_FS_SEEK_SET) != DL_FS_RESULT_SUCCESS )
            {
                AFW_FreeAllocatedMemory((void*)physBase);
                if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
                if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
                return ELDR_SEEK_FAILED;
            }
            if( DL_FsReadFile(elfSymTable, elfSectionHeader.sh_size, 1, f, &read) != DL_FS_RESULT_SUCCESS )
            {
                AFW_FreeAllocatedMemory((void*)physBase);
                if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
                if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
                return ELDR_READ_FAILED;
            }

            elfNumSymbols = elfSectionHeader.sh_size >> 4; // / sizeof(Elf32_Sym);
        }
        else if( (elfSectionHeader.sh_type == SHT_STRTAB) && (i != elfHeader.e_shstrndx) )
        {
            elfStrTable = (char*)AFW_AllocateMemory(elfSectionHeader.sh_size);
            if( DL_FsFSeekFile(f, elfSectionHeader.sh_offset, DL_FS_SEEK_SET) != DL_FS_RESULT_SUCCESS )
            {
                AFW_FreeAllocatedMemory((void*)physBase);
                if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
                if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
                return ELDR_SEEK_FAILED;
            }
            if( DL_FsReadFile(elfStrTable, elfSectionHeader.sh_size, 1, f, &read) != DL_FS_RESULT_SUCCESS )
            {
                AFW_FreeAllocatedMemory((void*)physBase);
                if(elfSymTable) AFW_FreeAllocatedMemory(elfSymTable);
                if(elfStrTable) AFW_FreeAllocatedMemory(elfStrTable);
                return ELDR_READ_FAILED;
            }

            elfStrTableSize = elfSectionHeader.sh_size;
        }
    }

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

#define STT_OBJECT 1
#define STT_FUNC 2


    UtilLogStringData("API Calls Fix Start:  ldrStrTable 0x%X  ldrSymTable 0x%X  ldrNumSymbols %d", 
                      ldrStrTable,
                      ldrSymTable,
                      ldrNumSymbols);

// Ищем совпадения и подменяем адреса
    for( i=0; i<elfNumSymbols; i++ )
    {
        if( (ELF32_ST_BIND(elfSymTable[i].st_info)==STB_GLOBAL) &&
            ( (ELF32_ST_TYPE(elfSymTable[i].st_info)==STT_FUNC) || 
            ( (ELF32_ST_TYPE(elfSymTable[i].st_info)==STT_OBJECT) && (elfSymTable[i].st_size==0) ) ) )
        {
            UtilLogStringData("Possible entry:  i %d  st_name %d  name %s", i, elfSymTable[i].st_name, &elfStrTable[elfSymTable[i].st_name]);
            for(j=0; j<ldrNumSymbols; j++ )
            {
                //UtilLogStringData("FOR:  j %d st_name", j, ldrSymTable[j].st_name);
                if(namecmp(&elfStrTable[elfSymTable[i].st_name], &ldrStrTable[ldrSymTable[j].st_name]) == 1)
                {
                    UtilLogStringData("API Call #%d:\n  addr 0x%X\n  old 0x%X\n  new 0x%X\n", 
                                      i, 
                                      elfSymTable[i].st_value,
                                      *((UINT32*)(physBase + elfSymTable[i].st_value - virtBase)),
                                      ldrSymTable[j].st_value );

                    if(ldrSymTable[j].st_value>0xC0000000) *((UINT32*)(physBase + elfSymTable[i].st_value - virtBase)) = ldrSymTable[j].st_value-0xC0000000;
                      else *((UINT32*)(physBase + elfSymTable[i].st_value - virtBase + 0xC)) = ldrSymTable[j].st_value;
                }
            }
        }
    }

    AFW_FreeAllocatedMemory(elfStrTable);
    AFW_FreeAllocatedMemory(elfSymTable);
    DL_FsCloseFile(f);

/*/////WIN32 Chunk/////////
    f = fopen("loaded.bin", "wb");
    fwrite((void*)physBase, upperAddr-virtBase, 1, f);
    fclose(f);

    free((void*)physBase);
*/////////////////////////
    // START IT!
    
    UtilLogStringData(" Starting ELF at 0x%X with reserve = 0x%X", physBase + elfHeader.e_entry - virtBase, reserve);
    ((Entry)(physBase + elfHeader.e_entry - virtBase))( file_uri, params, reserve );
    
    /*if( ((Entry)(physBase + elfHeader.e_entry - virtBase))(file_uri, *reserve, param) == 1)
    { //Start immediately
        AFW_AddEvNoD(p_evg, *evcode_base);
    }*/
    UtilLogStringData(" ELF returned");

    return ELDR_SUCCESS;
}

UINT32  namecmp(const char* str1, const char* str2)
{
    UINT32 i=0;
    while( str1[i] == str2[i] )
    {
        if(str1[i++] == 0) return 1;
    }
    return 0;
}

SYN_RETURN_STATUS_T LdrStartApp( AFW_EVENT_CODE_T ev_start )
{
    SYN_RETURN_STATUS_T status;
    status = AFW_CreateInternalQueuedEvAux( ev_start,
                                           AFW_BUF_FLAG_READ_ONLY,
                                           0,
                                           SYN_NULL );
    UtilLogStringData(" LdrStartApp 0x%X  status = %d", ev_start, status);

    return status;
}

SYN_RETURN_STATUS_T LdrLoadELF( W_CHAR *uri, W_CHAR *params )
{
    SYN_RETURN_STATUS_T status;
    AFW_EVENT_DATA_T    ev_data;

    u_utoa(uri, ev_data.AFW_UNION_DATA_T.start_params.uri);
    if( params!=SYN_NULL ) u_utoa(params, ev_data.AFW_UNION_DATA_T.start_params.params);
    else ev_data.AFW_UNION_DATA_T.start_params.params[0] = 0;
    
    ev_data.udata_id = AFW_UNION_EV_DATA_ID_NULL;

    UtilLogStringData( "LdrLoadELF uri: %s  params: %s", 
                       ev_data.AFW_UNION_DATA_T.start_params.uri,
                       ev_data.AFW_UNION_DATA_T.start_params.params 
                     );

    status = AFW_CreateInternalQueuedEvAuxD( EVCODE_LOADELF,
                                             &ev_data,
                                             AFW_BUF_FLAG_INVALID,
                                             0,
                                             SYN_NULL );
    return status;
}

SYN_RETURN_STATUS_T LdrUnloadELF( void *elf_ptr )
{
    AFW_EVENT_DATA_T       ev_data;

    UtilLogStringData(" LdrUnloadELF 0x%X  0x%X", elf_ptr );

    ev_data.udata_id = AFW_UNION_EV_DATA_ID_NULL;

    //*((UINT32*)ev_data.AFW_UNION_DATA_T.afw_data_union.AFW_EV_DATA_U.generic_short_data) = (UINT32)elf_ptr;
    *((UINT32*)ev_data.AFW_UNION_DATA_T.pad) = (UINT32)elf_ptr;
    
    UtilLogStringData(" LdrUnloadELF send" );
    
    return AFW_CreateInternalQueuedEvAuxD( EVCODE_UNLOADELF,
                                           &ev_data,
                                           AFW_BUF_FLAG_READ_ONLY,
                                           0,
                                           SYN_NULL );
}

const char* LdrGetPhoneName(void)
{
    return n_phone;
}

const char* LdrGetPlatformName(void)
{
    return n_platform;
}

const char* LdrGetFirmwareMajorVersion(void)
{
    return n_majorfw;
}

const char* LdrGetFirmwareMinorVersion(void)
{
    return n_minorfw;
}

char* u_utoa(const W_CHAR* ustr, char* astr)
{
    UINT32  i=0;

    do
    {
        astr[i] = (char)ustr[i];
    }while (ustr[i++] != 0);

    return astr;
}


void UtilLogStringData(const char*  format, ...)
{  
    char      buffer[255];
    va_list   vars;
    DL_FS_HANDLE_T f;
    UINT32        written;
    UINT32        sz;
    UINT32        *reboot;

    va_start(vars, format);
    vsprintf(buffer, format, vars);
    va_end(vars);

#ifdef LOG_FILE
    sz = strlen(buffer);
    buffer[sz] = '\r';
    buffer[sz+1] = '\n';
    f = DL_FsOpenFile(L"file://a/elfpack.txt", FILE_APPEND_PLUS_MODE, 0xE);
    if(f == DL_FS_HANDLE_INVALID)
    {
        reboot = (UINT32*)0x10092000;
        *reboot = 0;
    }
    

    DL_FsWriteFile((void*)buffer, sz+2, 1, f, &written);
    DL_FsCloseFile(f);
#else
    suLogData(0, 0x5151, 1, strlen(buffer)+1, buffer);
#endif

}
