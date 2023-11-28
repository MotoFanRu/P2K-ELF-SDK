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
	int size = 0;

	const char *mode = &curPat->mode;

	strncpy(buf, mode, 1);
	strncpy(buf + 1, " ", 1);
	strncpy(buf + 2, curPat->name, 76);

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
	printf("%03d/%03d, %06.2f%%", count, stat.patcount, count*100/(float)stat.patcount);
#else
	fprintf(stderr, "%06.2f%% | %03ld/%03ld | %s\n", count*100/(float)stat.patcount, count, stat.patcount, buf);
#endif
}


void printHelp()
{
	printf(
		"pat v1.1 by Andy51, EXL, (c) 2010-2023\r\n\r\n"
		"Usage:\r\n\tpat [-ram-trans|-no-ram-trans] <CG1.smg> <functions.pat> [<CG1 offset>]\r\n\r\n"
		"Example:\r\n\tpat -ram-trans E1_R373_G_0E.30.49R.smg functions.pat 0x10080000\r\n"
		"\tpat -no-ram-trans L7_R4513_G_08.B7.ACR_RB.smg functions.pat 0x10092000\r\n"
		"\tpat -no-ram-trans V3i_R4441D_G_08.01.03R.smg functions.pat 0x100A0000\r\n\r\n"
	);
}


int main(int argc, char* argv[])
{
	u32			match;
	u32			*list;
	u32			i;
	u32			cgoff;
	BOOL		ram_trans = FALSE;

#if !defined(WIN32)
	libpatInit();
#endif

	if((argc != 4) && (argc != 5))
	{
		printHelp();
		return 0;
	}

	if(argc == 5)
	{
		cgoff = strtol(argv[4], NULL, 16);
		
		/*
		 * Allow zero addresses!
		if(cgoff == 0)
		{
			printHelp();
			return 0;
		}
		*/

		libpatSetOffset(cgoff);
	}

	if(!strncmp(argv[1], "-ram-trans", strlen("-ram-trans")))
	{
		ram_trans = TRUE;
	}
	if(!strncmp(argv[1], "-no-ram-trans", strlen("-no-ram-trans")))
	{
		ram_trans = FALSE;
	}

	if(libpatLoadBinary(argv[2], NULL) == NULL)
	{
		printf("ERROR: Binary file not found: %s\r\n", argv[2]);
		return 0;
	}

	if(libpatLoadPatterns(argv[3]) == FALSE)
	{
		printf("ERROR: Invalid patterns file: %s\r\n", argv[3]);
		return 0;
	}

	libpatGetStats(&stat);

	printf("Patterns count: %ld\nAverage pattern length: %f\nMax pattern length: %ld\n16-byte length pattern count: %ld (%ld%% of total)\r\n", stat.patcount, stat.avglen, stat.maxlen, stat.len16cnt, stat.len16cnt*100/stat.patcount);

	libpatSetCallback(callback);

	libpatEnableRamTrans(ram_trans);

	match = libpatFindAllPatterns();

	libpatSaveSymfile("functions.sym");

	printf("\r\n\r\nFinished! matches = %ld\r\n\r\n", match);

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
