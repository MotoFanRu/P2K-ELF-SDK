
#include "AutoRun.h"

const W_CHAR          autorun_uri[] = {'/','a','/','E','l','f','/','a','u','t','o','.','r','u','n',0};

extern void APP_SyncML_MainRegister(void);

#pragma arm section code = "Header"
void AutorunMain(void)
{
	DL_FS_HANDLE_T          f;
    DL_FS_COUNT_T           count;
    UINT32					filesize;
    char                  	*buf;
    UINT32                	i=0, result=PARS_SKIP;
	W_CHAR					uri[64], params[64];

    // Placeholder for replaced RegFn
    //UtilLogStringData("ElfLdr start pre");
    //DL_AudPlayTone(10, 3);
    PFprintf(" ****** ELFPack test ******");
//    return;
    
    APP_SyncML_MainRegister();
    // ------------------------------

    ElfLoaderStart();    //Запускаем запускателя

    f = DL_FsOpenFile(autorun_uri, DL_FS_READ_MODE, 0);
   
    if(f==DL_FS_HANDLE_INVALID) return;

	filesize = DL_FsGetFileSize(f);

    if(filesize==0) return;

    buf = (char*)suAllocMem(filesize+3, 0);

    DL_FsReadFile(buf, filesize, sizeof(UINT8), f, &count);
	DL_FsCloseFile(f);

    if(buf[filesize-1]!='\n')
    {
        buf[filesize++]='\r';
        buf[filesize++]='\n';
    }

    buf[filesize]=(char)0xFF;

    do
    {
        result = ParseString(&buf[i], &i, uri, params);
		if(result == PARS_DONE)
		{
			UtilLogStringData(" Load ELF");
            LdrLoadELF(uri, params);
		}
    }while(result != PARS_EOF);
    
    suFreeMem(buf);

    return;
}


void SkipLine(char *buf, UINT32 *pindex)
{
    UINT32 i=0;
    while( buf[i]!='\n' ) i++;
    *pindex += i+1;
}


UINT32  ParseString(char* buf, UINT32* pindex, W_CHAR *uri, W_CHAR *params)
{
    UINT32		i=0, j=0;

    if(buf[0]==(char)0xFF) return PARS_EOF;

	while(buf[i]==' ') i++;

    if( (buf[i]==';') || (buf[i]=='\r') )
    {
        SkipLine(buf, pindex);
        return PARS_SKIP; 
    }

	//get uri
	j=i;
	while((buf[i]!=' ')&&(buf[i]!='\r')) i++;
	buf[i++]=0;
	u_atou((char*)&buf[j], uri);	//strcpy(uri, &buf[j]);

	if(buf[i]=='\n') 
	{
		*pindex += i+1;
		params[0]=0;
		return PARS_DONE;
	}

	//get params
	while(buf[i]==' ') i++;
	j=i;
	while(buf[i]!='\r') i++;
	buf[i]=0;
	u_atou((char*)&buf[j], params);	//strcpy(params, &buf[j]);
	*pindex += i+2;

	return PARS_DONE;
}




