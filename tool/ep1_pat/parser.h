#ifndef PARSER__H
#define PARSER__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#if defined(WIN32)
#include <windows.h>
#include <conio.h>
#endif
#include "libpat.h"

typedef struct
{
	char		*name;
	char		*text;
	char		mode;
	u8			load;
	u16			nmatch;
	s16			offs[2];
} PARSED_DATA_T;


typedef enum
{
	PARSE_DONE = 0,
	PARSE_SKIP,
	PARSE_EOF

} PARSER_RESULT_T;

#define PARSE_EOF_MARKER			(char)(0x01)

typedef enum
{
	PARSE_SUCCESS = 0,
	PARSE_FILE_NOT_EXIST,
	PARSE_FILE_EMPTY,
	PARSE_FILE_READ_ERROR,
	PARSE_NO_MEMORY,
	
	PARSE_ERROR_MAX
} PARSER_ERROR_T;

#ifdef __cplusplus
extern "C" {
#endif


PARSER_RESULT_T  parseLine( char *buf, int *index, PARSED_DATA_T *parsed );
u32 parsePatFile(char *path);

#ifdef __cplusplus
}
#endif




#endif
