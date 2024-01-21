
#include "config.h"
#include <mem.h>
#include "parser.h"
#include "console.h"
#include "features.h"
#include "dbg.h"


LDR_CONFIG_T		Config;

const PARSER_FIELD_T configFields[] =
{
	DEFINE_FIELD( EnableConsole,	PARSE_TYPE_BYTE ),
	DEFINE_FIELD( AutoShowConsole,	PARSE_TYPE_BYTE ),
	DEFINE_FIELD( DebugMode,		PARSE_TYPE_BYTE ),
	DEFINE_FIELD( SkipAutorun,		PARSE_TYPE_BYTE ),
	DEFINE_FIELD( ShowLogo,			PARSE_TYPE_BYTE ),
	DEFINE_FIELD( AutorunPath,		PARSE_TYPE_WSTRING ),
	DEFINE_FIELD( FastRunElf,		PARSE_TYPE_WSTRING ),
	DEFINE_FIELD( FastRunEvent,		PARSE_TYPE_WORD ),
	DEFINE_FIELD( DefLibraryPath,	PARSE_TYPE_WSTRING ),
	
	// LIBPATHCNT times
	DEFINE_NAMED_FIELD( "LibsPath1", LibsPaths[0], PARSE_TYPE_WSTRING ),
	DEFINE_NAMED_FIELD( "LibsPath2", LibsPaths[1], PARSE_TYPE_WSTRING ),
	DEFINE_NAMED_FIELD( "LibsPath3", LibsPaths[2], PARSE_TYPE_WSTRING ),
	
	FIELD_TABLE_END
};

#define CONFIG_FIELD( c, f ) ( (char*)(c) + configFields[f].off )


const LDR_CONFIG_T defaultConfig = 
{
	TRUE,					// EnableConsole
	FALSE,					// AutoShowConsole
	FALSE,					// DebugMode
	FALSE,					// SkipAutorun
	TRUE,					// ShowLogo
	FTR_AUTORUN_URI,		// AutorunPath
	FTR_FASTRUN_ELF,		// FastRunElf
	FTR_FASTRUN_EVENT,		// FastRunEvent
	FTR_LIBRARY_URI,		// DefLibraryPath
	FTR_LIB_PATH_1,			// LibsPaths[]
	FTR_LIB_PATH_2,
	FTR_LIB_PATH_3
};



/* устанавливается конфиг по умолчанию
** cfg заранее не очищается
*/
void setDefaultConfig( LDR_CONFIG_T * cfg )
{
	if ( !cfg ) return;
	__rt_memcpy( cfg, &defaultConfig, sizeof(LDR_CONFIG_T) );
}


/* пытается загрузить конфиг из файла
** при неудаче используется конфиг по умолчанию
*/
UINT32 loadConfig( LDR_CONFIG_T * cfg, const WCHAR * path )
{
	PARSER_ERROR_T		result;
	
	if ( !cfg || !path ) return LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	freeConfig( cfg );
	
	result = ParseFile( path, ParserConfigLine, &Config );
	
	switch( result )
	{
		case PARSE_SUCCESS:
			dbg( "Config processed\n" );
		break;
			
		default:
			dbgf( "Config error #%d, Using default\n", result );
			freeConfig( cfg );
			setDefaultConfig( cfg );
		break;
	}
	
	return result == PARSE_SUCCESS ? RESULT_OK : RESULT_FAIL;
}


/* освободить память, выделенную для полей конфига
** саму структуру cfg не удаляет
*/
UINT32 freeConfig( LDR_CONFIG_T * cfg )
{
	int			i;
	
	if ( !cfg ) return LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	for ( i=0; configFields[i].name != NULL; i++ )
	{
		if (	(configFields[i].type == PARSE_TYPE_STRING || 
				configFields[i].type == PARSE_TYPE_WSTRING ) &&
				(UINT32*)CONFIG_FIELD( cfg, i ) != NULL &&
				//*(UINT32*)CONFIG_FIELD( cfg, i ) != 0 &&
				*(UINT32*)CONFIG_FIELD( cfg, i ) != 
					*(UINT32*)CONFIG_FIELD( &defaultConfig, i ) )
		{
			suFreeMem( *(void**)CONFIG_FIELD( cfg, i ) );
			*(char**)CONFIG_FIELD( cfg, i ) = NULL;
		}
	}
	
	return RESULT_OK;
}


/* установить текущий конфиг из параметра
** cfg - образец конфига, с него делается копия
*/
UINT32 setConfig( const LDR_CONFIG_T * cfg )
{
	int		i;
	char	*s1, *s2;
	WCHAR	*w1, *w2;
	
	if ( !cfg ) return LDR_RESULT_ERROR_BAD_ARGUMENT;
	freeConfig( &Config );
	
	for ( i=0; configFields[i].name != NULL; i++ )
	{
		switch ( configFields[i].type )
		{
			case PARSE_TYPE_BYTE:
				__rt_memcpy( CONFIG_FIELD( &Config, i ), CONFIG_FIELD( cfg, i ), 1 );
				break;
			
			case PARSE_TYPE_HALF:
				__rt_memcpy( CONFIG_FIELD( &Config, i ), CONFIG_FIELD( cfg, i ), 2 );
				break;
			
			case PARSE_TYPE_WORD:
				__rt_memcpy( CONFIG_FIELD( &Config, i ), CONFIG_FIELD( cfg, i ), 4 );
				break;
			
			case PARSE_TYPE_STRING:
				s1 = *(char**)CONFIG_FIELD( &Config, i );
				s2 = *(char**)CONFIG_FIELD( cfg, i );
				
				// если строки равны, то ничего не далаем
				if ( !strcmp( s1, s2 ) ) break;
				
				// если в текущем конфиге строка не из defaultConfig, то освобождаем память
				if ( strcmp( s1, *(char**)CONFIG_FIELD( &defaultConfig, i ) ) )
					mfree( s1 );
				
				// копируем новую строку
				s1 = (char*)malloc( strlen( s2 ) + 1 );
				if ( s1 != NULL )
					strcpy( s1, s2 );
				
				*(char**)CONFIG_FIELD( &Config, i ) = s1;
				break;
			
			case PARSE_TYPE_WSTRING:
				w1 = *(WCHAR**)CONFIG_FIELD( &Config, i );
				w2 = *(WCHAR**)CONFIG_FIELD( cfg, i );
				
				// если строки равны, то ничего не далаем
				if ( !u_strcmp( w1, w2 ) ) break;
				
				// если в текущем конфиге строка не из defaultConfig, то освобождаем память
				if ( u_strcmp( w1, *(WCHAR**)CONFIG_FIELD( &defaultConfig, i ) ) )
					mfree( w1 );
				
				// копируем новую строку
				w1 = (WCHAR*)malloc( 2*u_strlen( w2 ) + 1 );
				if ( w1 != NULL )
					u_strcpy( w1, w2 );
				
				*(WCHAR**)CONFIG_FIELD( &Config, i ) = w1;
				break;
		}
	}
	
	return RESULT_OK;
}


/* делает копию текущего конфига
** cfg - куда поместить копию, переменная должна существовать
*/
UINT32 copyConfig( LDR_CONFIG_T * cfg )
{
	int		i;
	char	*s1, *s2;
	WCHAR	*w1, *w2;
	
	if ( !cfg ) return LDR_RESULT_ERROR_BAD_ARGUMENT;
	
	for ( i=0; configFields[i].name != NULL; i++ )
	{
		switch ( configFields[i].type )
		{
			case PARSE_TYPE_BYTE:
				__rt_memcpy( CONFIG_FIELD( cfg, i ), CONFIG_FIELD( &Config, i ), 1 );
				break;
			
			case PARSE_TYPE_HALF:
				__rt_memcpy( CONFIG_FIELD( cfg, i ), CONFIG_FIELD( &Config, i ), 2 );
				break;
			
			case PARSE_TYPE_WORD:
				__rt_memcpy( CONFIG_FIELD( cfg, i ), CONFIG_FIELD( &Config, i ), 4 );
				break;
			
			case PARSE_TYPE_STRING:
				s1 = *(char**)CONFIG_FIELD( cfg, i );
				s2 = *(char**)CONFIG_FIELD( &Config, i );
				
				// копируем новую строку
				s1 = (char*)malloc( strlen( s2 ) + 1 );
				if ( s1 != NULL )
					strcpy( s1, s2 );
				
				*(char**)CONFIG_FIELD( cfg, i ) = s1;
				break;
			
			case PARSE_TYPE_WSTRING:
				w1 = *(WCHAR**)CONFIG_FIELD( cfg, i );
				w2 = *(WCHAR**)CONFIG_FIELD( &Config, i );
				
				// копируем новую строку
				w1 = (WCHAR*)malloc( 2*u_strlen( w2 ) + 1 );
				if ( w1 != NULL )
					u_strcpy( w1, w2 );
				
				*(WCHAR**)CONFIG_FIELD( cfg, i ) = w1;
				break;
		}
	}
	
	return RESULT_OK;
}


/* EOF */

