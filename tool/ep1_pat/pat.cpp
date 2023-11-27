// test.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "libpat.h"

void gotoxy( short x, short y )
{
#if defined(WIN32)
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y };

	SetConsoleCursorPosition( hStdout, position );
#endif
}

void getpos( short *x, short *y )
{
#if defined(WIN32)
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	
	GetConsoleScreenBufferInfo(hStdout, &SBInfo);

	*x = SBInfo.dwCursorPosition.X;
	*y = SBInfo.dwCursorPosition.Y;
#endif
}


LIBPAT_LDSTAT		stat;


void __stdcall callback(PATTERN_T *curPat, u32 count)
{
	char			buf[81];
	static BOOL		first = TRUE;
	static short	x, y;
	int i = 0;

	strncpy(buf, curPat->name, 78);

	for(i=strlen(buf); i<80; i++)
	{
		buf[i] = ' ';
	}

	buf[i] = '\0';

	if(first)
	{
		getpos(&x, &y);
		y++;
		first = FALSE;
	}

	gotoxy(x, y);

#if defined(WIN32)
	printf("%d/%d, %.2f%%", count, stat.patcount, count*100/(float)stat.patcount);
#else
	fprintf(stderr, "%d/%d, %.2f%%, %s\n", count, stat.patcount, count*100/(float)stat.patcount, buf);
#endif
}


void printHelp()
{
	printf(	"pat v1.1 by Andy51, 2010\r\n\r\n"
			"Usage: pat <CG1.smg> <functions.pat> [<CG1 offset>]\r\n");
}


int main(int argc, char* argv[])
{
	u32			match;
	u32			*list;
	u32			i;
	u32			cgoff;

#if !defined(WIN32)
	libpatInit();
#endif

	if((argc != 3) && (argc != 4))
	{
		printHelp();
		return 0;
	}

	if(argc == 4)
	{
		cgoff = strtol(argv[3], NULL, 16);
		
		if(cgoff == 0)
		{
			printHelp();
			return 0;
		}

		libpatSetOffset(cgoff);
	}

	if(libpatLoadBinary(argv[1], NULL) == NULL)
	{
		printf("ERROR: Binary file not found: %s\r\n", argv[1]);
		return 0;
	}

	if(libpatLoadPatterns(argv[2]) == FALSE)
	{
		printf("ERROR: Invalid patterns file: %s\r\n", argv[2]);
		return 0;
	}

	libpatGetStats(&stat);

	printf("Patterns count: %d\nAverage pattern length: %f\nMax pattern length: %d\n16-byte length pattern count: %d (%d%% of total)\r\n", stat.patcount, stat.avglen, stat.maxlen, stat.len16cnt, stat.len16cnt*100/stat.patcount);

	libpatSetCallback(callback);

	libpatEnableRamTrans(TRUE);

	match = libpatFindAllPatterns();

	libpatSaveSymfile("functions.sym");

	printf("\r\n\r\nFinished! matches = %d\r\n\r\n", match);

	/*
	list = libpatFindPattern("DRM_GetCurrentLanguage T 4A??230056D21C0120012A00D10047702900D0FC48??68007800", &match);

	printf("DRM_GetCurrentLanguage found at:\r\n");

	for(i=0; i<match; i++)
	{
		printf("\t0x%X\r\n", list[i]);
	}
	*/

#if !defined(WIN32)
	libpatTerm();
	RamTransTerm();
#endif

//#if defined(WIN32)
//	getch();
//#endif
	return 0;

}
