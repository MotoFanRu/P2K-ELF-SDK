
#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef WIN32

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "..\parser_win32\wrapper.h"

	#define WIN32_TEST_AUTORUN
	//#define WIN32_TEST_CONFIG

#else
	
	#include <typedefs.h>
	
#endif



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

typedef enum
{
	PARSE_TYPE_BYTE = 0,	// UINT8
	PARSE_TYPE_HALF,		// UINT16
	PARSE_TYPE_WORD,		// UINT32
	
	PARSE_TYPE_STRING,
	PARSE_TYPE_WSTRING,
	
	PARSE_TYPE_MAX

} PARSER_DATA_TYPE;


typedef struct
{
	WCHAR *uri, *params;

} ELFSTART_T;

typedef struct
{
	char				*name;
	PARSER_DATA_TYPE	type;
	UINT16				off;

} PARSER_FIELD_T;

#define STRUCT_OFFSET(tp, member) \
   ((UINT16) (((char*)&((tp*)0)->member)-(char*)0))

#define DEFINE_FIELD( _field_name, _type ) \
	{ #_field_name, _type, STRUCT_OFFSET(LDR_CONFIG_T, _field_name) }

#define DEFINE_NAMED_FIELD( _field_text, _field_name, _type ) \
	{ _field_text, _type, STRUCT_OFFSET(LDR_CONFIG_T, _field_name) }

#define FIELD_TABLE_END \
	{ NULL, 0, 0 }


#define SkipLine( _buf, _index ) \
	while( _buf[_index] != '\n' ) _index++; \
	_index++

#define SkipWhitespaces( _buf, _index ) \
	while( (_buf[_index] == ' ') || (_buf[_index] == '\t') ) _index++


typedef PARSER_RESULT_T (*fnLineParser)( char*, int*, void* );

extern const PARSER_FIELD_T configFields[];


#ifdef __cplusplus
extern "C" {
#endif


PARSER_ERROR_T		ParseFile( const WCHAR * path, fnLineParser parser, void * pdata );
PARSER_RESULT_T		ParserAutorunLine( char *buf, int *index, void *pdata );
PARSER_RESULT_T		ParserConfigLine( char *buf, int *index, void *pdata );
PARSER_RESULT_T		ParserConfigValue( char *buf, int *index, void *pdata, int field );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PARSER_H__ */
