#include "common.h"


u8					*gBuffer = NULL;
long				gBufLength;

PATTERN_T			*gPatterns;
int					gPatCount;

u32					gCG1Offset;

LIBPAT_LDSTAT		gStats;
u32					gStatTotalLen;
libpatCallback		gStatusCallback;

u32					*gFindPatternResult;


#if defined(WIN32)
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			libpatInit();
		break;
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			libpatTerm();
			RamTransTerm();
		break;
    }
    return TRUE;
}
#else
__attribute__((constructor)) void start_library(void) {
	libpatInit();
}

__attribute__((constructor)) void stop_library(void) {
	libpatTerm();
	RamTransTerm();
}
#endif

u32 createPattern(PATTERN_T *pattern, PARSED_DATA_T *pdata)
{
	char *name = pdata->name;
	char *text = pdata->text;

	int		i;
	int		len = strlen(text);
	int		bytelen;
	char	bytestr[3];

	bytelen = (len/2 + LENGTH_ALIGNMENT_MASK) & (~LENGTH_ALIGNMENT_MASK);

#if defined(WIN32)
	pattern->text = (u8*)_aligned_malloc(bytelen*sizeof(u8), LENGTH_ALIGNMENT);
	pattern->mask = (u8*)_aligned_malloc(bytelen*sizeof(u8), LENGTH_ALIGNMENT);
#else
	pattern->text = (u8*)_mm_malloc(bytelen*sizeof(u8), LENGTH_ALIGNMENT);
	pattern->mask = (u8*)_mm_malloc(bytelen*sizeof(u8), LENGTH_ALIGNMENT);
#endif
	pattern->length = bytelen;

	memset(pattern->mask, 0xFF, bytelen);

	for(i = 0; i < len; i++)
	{
		if(text[i] == '?')
		{
			pattern->mask[i/2] = 0x00; 
		}
	}

	// alignment gap fill
	for(i = len/2; i<bytelen; i++)
	{
		pattern->mask[i] = 0x00;
	}


	bytestr[2] = '\0';
	for(i = 0; i < len/2; i++)
	{
		bytestr[0] = text[i*2];
		bytestr[1] = text[i*2+1];
		pattern->text[i] = (u8)strtol(bytestr, NULL, 16);
	}

	// alignment gap fill
	for(i = len/2; i<bytelen; i++)
	{
		pattern->text[i] = 0x00;
	}

	pattern->name = (char*)malloc((strlen(name)+1)*sizeof(char));
	strcpy(pattern->name, name);
	pattern->load = pdata->load;
	pattern->mode = pdata->mode;
	pattern->offs[0] = pdata->offs[0];
	pattern->offs[1] = pdata->offs[1];

	pattern->flags = 0;
	pattern->nmatch = pdata->nmatch;
	pattern->value = 0;
	pattern->found = 0;


	// Statistics

	len /= 2;
	gStatTotalLen += len;

	if(bytelen == 16)
		gStats.len16cnt++;

	if((u32)len > gStats.maxlen)
		gStats.maxlen = len;

	gStats.patcount++;

	return bytelen;

}



u32 addPattern(PARSED_DATA_T *pdata)
{
	int		bytelen;

	bytelen = createPattern(&gPatterns[gPatCount], pdata);

	gPatCount++;

	return bytelen;
}

void freePattern(PATTERN_T *pattern)
{
	free(pattern->name);
#if defined(WIN32)
	_aligned_free(pattern->text);
	_aligned_free(pattern->mask);
#else
	_mm_free(pattern->text);
	_mm_free(pattern->mask);
#endif
}




long findPatternsSimple()
{
	u8		*ptr = (u8*)gBuffer;
	long	i,j,k;
	long	nmatch = 0;
	float	progress;
	long	text, mask, pat, result;

	k = 0;
	for(j=0; j<gBufLength; j++)
//	for(j=0x2968DE; j<gBufLength; j++)
//	for(j=0x28DCAC; j<gBufLength; j++)
	{
		for(k=0; k<gPatCount; k++)
		{
			result = 1;

			for(i=0; i<gPatterns[k].length && result==1; i+=4)
			{
				pat  = *(u32*)(&gPatterns[k].text[i]);
				mask = *(u32*)(&gPatterns[k].mask[i]);
				text = *(u32*)(&ptr[j+i]);

				result &= ( (text & mask) == pat );
			}

			//if(i == gPatterns[k].length)
			if(result == 1)
				nmatch++;
		}

		if((j%0x5000)==0)
		{
			progress = (j / (float)gBufLength)*100.f;
			//gotoxy(0,3);
			printf("%f%%",progress);
		}


	}

	return nmatch;
}



void translateValue(PATTERN_T *pat, u32 *value)
{
	u32		result;

	if(pat->flags == 0)
		return;


	result = *value + pat->offs[0];

	if(pat->load)
	{
		result = E32(*(u32*)&gBuffer[result]);
	}
	else
	{
		result += gCG1Offset;
	}

	result += pat->offs[1];

	*value = result;
}


void libpatInit()
{
	gPatCount = 0;

	gStatusCallback = NULL;

	gCG1Offset = CG1_OFFSET_DEFAULT;
	gPatterns = (PATTERN_T*)malloc(MAX_PATTERNS * sizeof(PATTERN_T));
	
	gFindPatternResult = NULL;

}


void libpatTerm()
{
	if(gPatterns != NULL)
	{
		libpatResetPatterns();
		free(gPatterns);
	}

	if(gBuffer != NULL)
#if defined(WIN32)
		_aligned_free(gBuffer);
#else
		_mm_free(gBuffer);
#endif

	if(gFindPatternResult != NULL)
		free(gFindPatternResult);

}





