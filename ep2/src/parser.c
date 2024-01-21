
#include "parser.h"

#ifndef WIN32

#include <mem.h>
#include <filesystem.h>
#include <utilities.h>
#include "utils.h"
#include "elfloader.h"
//#include "API.h"
#include "console.h"

#endif


PARSER_ERROR_T ParseFile( const WCHAR * path, fnLineParser parser, void *pdata )
{
	UINT32					count = 0;
	INT32					err = 0;
	FILE_HANDLE_T			f;
	UINT8					fresult;
	char					*buf;
	PARSER_RESULT_T			result = PARSE_SKIP;
	int						i=0;
	UINT32					filesize;
	

	f = DL_FsOpenFile(path, FILE_READ_MODE, 0);
	if( f == FILE_HANDLE_INVALID )
		return PARSE_FILE_NOT_EXIST;

	filesize = DL_FsGetFileSize(f);
	if ( filesize == 0 )
	{
		DL_FsCloseFile(f);
		return PARSE_FILE_EMPTY;
	}

	buf = (char*)suAllocMem( filesize+2, &err );
	if ( err != 0 )
	{
		DL_FsCloseFile(f);
		return PARSE_NO_MEMORY;
	}


	fresult = DL_FsReadFile( buf, sizeof(UINT8), filesize, f, &count );
	if ( fresult != 0 || count < filesize )
	{
		DL_FsCloseFile(f);
		return PARSE_FILE_READ_ERROR;
	}
	
	DL_FsCloseFile(f);

	buf[filesize] = '\n';
	buf[filesize+1] = PARSE_EOF_MARKER;

	do
	{
		result = (parser)( buf, &i, pdata );
		
		//if( result == PARSE_DONE) printf("%s:%s\n", uri, params);

	} while( result != PARSE_EOF );
	

	suFreeMem( buf );

	return PARSE_SUCCESS;

}


#define ELF_NAME_MAX (STRLEN - 2 - 5 - 4)

PARSER_RESULT_T  ParserAutorunLine( char *buf, int *index, void *pdata )
{
	ELFSTART_T	*data = pdata;
	int			i = *index;
	int			j = 0;
	char		delimiter;

#ifndef WIN32
	UINT32		result;
	WCHAR		*wname;
	char		name[ELF_NAME_MAX+1];
	char		fmt[ELF_NAME_MAX+1];
#endif

	if ( buf[i] == PARSE_EOF_MARKER )
		return PARSE_EOF;

	SkipWhitespaces(buf, i);

	if ( (buf[i] == ';') || (buf[i] == '\r') || (buf[i] == '\n') )
	{
		SkipLine( buf, i );
		*index = i;
		return PARSE_SKIP; 
	}

	// Quoted path support
	if ( buf[i] == '\"' )
	{
		delimiter = '\"';
		i++;
	} else
		delimiter = ' ';

	// Get path

	j = i; 
	while ( buf[i] != delimiter && buf[i] != '\t' && buf[i] != '\r' && buf[i] != '\n' ) i++;

	delimiter = buf[i];
	buf[i] = '\0';
	u_natou( &buf[j], data->uri, 64 );
	buf[i] = delimiter;

	SkipWhitespaces( buf, i );
	delimiter = buf[i];

	if ( delimiter == '\r' || delimiter == '\n' )
	{
		data->params[0] = '\0';
	}
	else
	{
		// Get params

		j = i;

		while( (buf[i] != '\r') && (buf[i] != '\n') ) i++;
		buf[i] = '\0';

		u_natou( &buf[j], data->params, 64 );
	}

	// Process the retrieved data

	#ifdef WIN32
		// Output the debug data
		printf("%s:%s\n", data->uri, data->params);
	
	#else
		// Load the elf
		if ( DL_FsFFileExist( data->uri ) )
		{
			result = loadElf( data->uri, data->params );
		}
		else
		{
			result = LDR_RESULT_FAIL;
		}
		
		wname = u_strrchr( data->uri, L'/' ) + 1;
		if ( u_strlen( wname ) <= ELF_NAME_MAX )
			u_nutoa( wname, name, ELF_NAME_MAX );
		else
		{
			u_nutoa( wname, name, ELF_NAME_MAX-3 );
			strcat( name, "..." );
		}
		
		snprintf( fmt, ELF_NAME_MAX, "\x89> \x8E""%%s%%%ds %%s\n", 20-strlen(name) );
		switch ( result )
		{
			case LDR_RESULT_OK:
				cprintf( fmt, name, " ", "\x8A""OK" );
				break;
			
			case LDR_RESULT_ERROR_EABI_VERSION2:
				cprintf( fmt, name, " ", "\x8E""EP1" );
				break;
			
			default:
				cprintf( fmt, name, " ", "\x8C""FAIL" );
		}
	#endif
	
	*index = i+1;

	return PARSE_DONE;
}


PARSER_RESULT_T  ParserConfigLine( char *buf, int *index, void *pdata )
{
	int			i = *index;
	int			j = 0;
	int			k = 0;
	int			currentField = -1;

	#ifdef WIN32
		char *ptr;
	#endif


	if( buf[i] == PARSE_EOF_MARKER )
		return PARSE_EOF;

	SkipWhitespaces(buf, i);

	if( (buf[i] == ';') || (buf[i] == '\r') || (buf[i] == '\n') )
	{
		SkipLine( buf, i );
		*index = i;
		return PARSE_SKIP; 
	}

	// -- Get parameter name --

	j = i; 
	while( (buf[i] != ' ') && (buf[i] != '\t') && (buf[i] != '\r') && (buf[i] != '\n') && (buf[i] != '=' ) ) i++;

	if( (buf[i] == '\r') || (buf[i] == '\n') )
	{
		*index = i+1;
		return PARSE_SKIP;
	}

	buf[i++] = '\0';

	// -- Parse the parameter name --
	k = 0;
	while ( configFields[k].name != NULL )
	{
		if( strcmp(&buf[j], configFields[k].name) == 0 )
		{
			currentField = k;
			break;
		}
		k++;
	}

	// Field is not defined
	if( currentField < 0 )
	{
		SkipLine( buf, i );
		*index = i;
		return PARSE_SKIP; 
	}


	// -- Get params --


	// Skip to parameter(s)
	while( (buf[i] == ' ') || (buf[i] == '\t') || (buf[i] == '=') ) i++;

	if( (buf[i] == '\r') || (buf[i] == '\n') )
	{
		*index = i+1;
		return PARSE_SKIP; 
	}

	k = ParserConfigValue(buf, &i, pdata, currentField);

	SkipLine( buf, i );
	

	// Process the retrieved data

	#ifdef WIN32
		// Output the debug data
		ptr = (char*)pdata + configFields[currentField].off;
		
		if( configFields[currentField].type == PARSE_TYPE_BYTE )
			printf("%s:%d\n", configFields[currentField].name, *(UINT8*)ptr);
		else
			printf("%s:%s\n", configFields[currentField].name, *(char**)ptr);

	#endif
	
	*index = i;

	return k;
}


PARSER_RESULT_T ParserConfigValue( char *buf, int *index, void *pdata, int field )
{
	INT32		err;
	UINT32		i = *index, j, val;
	void		*ptr = (char*)pdata + configFields[field].off;
	char		*str;
	char		delimiter;

	if( buf[i] == '\"' )
	{
		delimiter = '\"';
		i++;
	}
	else
		delimiter = ' ';

	j = i;
	
	while( (buf[i] != delimiter ) && (buf[i] != '\t' ) && (buf[i] != '\r') && (buf[i] != '\n')) i++;
	buf[i] = '\0';

	switch ( configFields[field].type )
	{
		case PARSE_TYPE_BYTE:
		case PARSE_TYPE_HALF:
		case PARSE_TYPE_WORD:
			if ( buf[j+1] == 'x' )
			{
				val = strtoul( &buf[j], 0, 16 );
			}
			else // dec
			{
				val = strtol( &buf[j], 0, 10 );
			}
			
			if ( configFields[field].type == PARSE_TYPE_BYTE )
				*(UINT8*)ptr = (UINT8)val;
			if ( configFields[field].type == PARSE_TYPE_HALF )
				*(UINT16*)ptr = (UINT16)val;
			if ( configFields[field].type == PARSE_TYPE_WORD )
				*(UINT32*)ptr = (UINT32)val;
		break;
		
		case PARSE_TYPE_STRING:
			val = strlen( &buf[j] ) + 1;
			str = (char*)suAllocMem( val, &err );
			if ( str )
			{
				strcpy( str, &buf[j] );
				*(UINT32*)ptr = (UINT32)str;
			}
			else
			{
				*(UINT32*)ptr = (UINT32)0;
			}
		break;
		
		case PARSE_TYPE_WSTRING:
			// this is not unicode string in file
			// we must convert to U-ctring
			
			val = 2 * strlen( &buf[j] ) + 2;
			str = (char*)suAllocMem( val, &err );
			if ( str )
			{
				u_atou( &buf[j], (WCHAR*)str );
				*(UINT32*)ptr = (UINT32)str;
			}
			else
			{
				*(UINT32*)ptr = (UINT32)0;
			}
		break;
	}

	*index = i+1;

	return PARSE_DONE;
}


#ifdef WIN32
	#include "config.h"

	#ifdef WIN32_TEST_AUTORUN
		const WCHAR file_uri[] = "auto.run";
	#else //WIN32_TEST_CONFIG
		const WCHAR file_uri[] = "elfpack.cfg";
	#endif

	int main(int argc, char* argv[])
	{
		#ifdef WIN32_TEST_AUTORUN

			WCHAR	uri[64], params[64];
			ELFSTART_T Elfstart;

			Elfstart.uri = uri;
			Elfstart.params = params;

			ParseFile((WCHAR*)file_uri, ParserAutorunLine, &Elfstart);

		#else //WIN32_TEST_CONFIG

			CONFIG_T Config;

			memset(&Config, 0, sizeof(CONFIG_T));

			ParseFile((WCHAR*)file_uri, ParserConfigLine, &Config);

		#endif

		return 0;
	}

#endif
