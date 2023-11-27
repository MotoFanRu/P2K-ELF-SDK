
#include "common.h"

BOOL					gCrawlStarted;

void* __stdcall libpatLoadBinary(char *path, u32 *size)
{
	FILE	*f;

	f = fopen(path, "rb");

	if(f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	gBufLength = ftell(f);
	fseek(f, 0, SEEK_SET);

#if defined(WIN32)
	gBuffer = (u8*)_aligned_malloc(gBufLength, LENGTH_ALIGNMENT);
#else
	gBuffer = (u8*)_mm_malloc(gBufLength, LENGTH_ALIGNMENT);
#endif

	fread(gBuffer, 1, gBufLength, f);

	fclose(f);

	if(size != NULL)
		*size = gBufLength;

	return gBuffer;
}



/*LIBPAT_LDSTAT libpatLoadPatterns(char *path)
{
	LIBPAT_LDSTAT	result = {0,0,0,0};
	FILE			*f;
	long			size;
	int				i,j;
	char			*buf;
	float			avglen = 0;
	long			maxlen = 0, len, len16=0;
	int				offs[2] = {0,0};

	f = fopen(path, "rb");

	if(f == NULL)
		return result;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = (char*)malloc(size);

	fread(buf, 1, size, f);

	fclose(f);



	i = j = 0;
	while(j < size)
	{
		
		while(buf[j] != '\r')
			j++;

		buf[j] = '\0';	

		len = addPattern("", &buf[i], 'D', 0, FALSE, offs);
		avglen += (float)len;
		
		if(len > maxlen) maxlen = len;
		if(len==16) len16++;

		j += 2;
		i = j;
	}

	avglen /= gPatCount;

	result.avglen = avglen;
	result.maxlen = maxlen;
	result.patcount = gPatCount;
	result.len16cnt = len16;

	//printf("Patterns count: %d\nAverage pattern length: %f\nMax pattern length: %d\n16-byte length pattern count: %d", gPatCount, avglen, maxlen, len16);

	free(buf);

	return result;
}*/


void __stdcall libpatGetStats(LIBPAT_LDSTAT *result)
{
	
	gStats.avglen = gStatTotalLen / (float)gPatCount;

	*result = gStats;

}


BOOL __stdcall libpatLoadPatterns(char *path)
{
	if(parsePatFile(path) != PARSE_SUCCESS)
		return FALSE;

	return TRUE;
}



BOOL __stdcall libpatAddPattern(char *pattern)
{
	PARSER_RESULT_T		result;
	PARSED_DATA_T		pdata;
	char				tmpbuf[256];
	u32					len;
	u32					index = 0;

	strcpy(tmpbuf, pattern);

	len = strlen(tmpbuf);
	tmpbuf[len++] = '\r';
	tmpbuf[len++] = '\n';
	tmpbuf[len] = '\0';

	result = parseLine( tmpbuf, &index, &pdata );

	if(result != PARSE_DONE)
		return FALSE;

	addPattern(&pdata);

	return TRUE;
}



void __stdcall libpatSaveSymfile(char *path)
{
	FILE		*f;
	int			i;
	char		buf[256];

	f = fopen(path, "wb");

    strcpy(buf, "#<SYMDEFS>#symdef-file\r\n");
	fwrite(buf, strlen(buf), 1, f);

	for(i=0; i<gPatCount; i++)
	{
		if(gPatterns[i].flags)
			sprintf(buf, "0x%.8X %c %s\r\n", gPatterns[i].value, gPatterns[i].mode, gPatterns[i].name);
		else {
			sprintf(buf, "%s not found!\r\n", gPatterns[i].name);
			fprintf(stderr, "Warning! Function \"%s\" not found!\r\n", gPatterns[i].name);
		}

		fwrite(buf, strlen(buf), 1, f);
	}

	fclose(f);
}



void __stdcall libpatSetOffset(u32 off)
{
	gCG1Offset = off;
}


u32* __stdcall libpatFindPattern(char *pattern, u32 *count)
{
	PARSER_RESULT_T		presult;
	PARSED_DATA_T		pdata;
	PATTERN_T			pat;
	u32					*found;
	u32					resSize;
	char				tmpbuf[256];
	u32					len;
	u32					index = 0;
	int					i;

	if(gBuffer == NULL)
		return NULL;

	if(count == NULL)
		return NULL;

	*count = 0;

	strcpy(tmpbuf, pattern);

	len = strlen(tmpbuf);
	tmpbuf[len++] = '\r';
	tmpbuf[len++] = '\n';
	tmpbuf[len] = '\0';

	presult = parseLine( tmpbuf, &index, &pdata );

	if(presult != PARSE_DONE)
		return NULL;

	createPattern(&pat, &pdata);

	found = findPatternSSEa(&pat);

	resSize = sizeof(long)*pat.found;

	if(gFindPatternResult != NULL)
		free(gFindPatternResult);

	gFindPatternResult = (long*)malloc(resSize);

	memcpy(gFindPatternResult, found, resSize);

	for(i=0; i<pat.found; i++)
	{
		translateValue(&pat, &gFindPatternResult[i]);
	}

	if(gRamTransEnabled == TRUE)
	{
		for(i=0; i<pat.found; i++)
		{
			RamTrans(&gFindPatternResult[i]);
		}
	}

	*count = pat.found;

	freePattern(&pat);

	return gFindPatternResult;

}


u32 __stdcall libpatFindPatternSingle(char *pattern)
{
	u32			*result;
	u32			count;

	result = libpatFindPattern(pattern, &count);

	if( count != 1 )
		return 0;

	return result[0];
}


int	__stdcall libpatFindAllPatterns()
{
	int		result;
	int		i;

	if(gBuffer == NULL)
		return 0;

	result = findAllPatternsSSEa();

	for(i=0; i<gPatCount; i++)
	{
		translateValue(&gPatterns[i], &gPatterns[i].value);
	}

	if(gRamTransEnabled == TRUE)
	{
		for(i=0; i<gPatCount; i++)
		{
			RamTrans(&gPatterns[i].value);
		}
	}

	return result;
}


void __stdcall libpatSetCallback( libpatCallback fn )
{
	gStatusCallback = fn;
}


void __stdcall libpatResetPatterns()
{
	int				i;

	for(i=0; i<gPatCount; i++)
	{
		freePattern(&gPatterns[i]);
	}

	gPatCount = 0;

	memset(&gStats, 0, sizeof(LIBPAT_LDSTAT));
}


void __stdcall libpatEnableRamTrans(BOOL enable)
{
	gRamTransEnabled = enable;
}


void __stdcall libpatCrawlStart()
{
	int				i;

	gCrawlStarted = TRUE;

	for(i=0; i<gPatCount && gCrawlStarted; i++)
	{
		gStatusCallback( &gPatterns[i], i+1);
	}
}


void __stdcall libpatCrawlStop()
{
	gCrawlStarted = FALSE;
}
