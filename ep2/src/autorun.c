
#include "autorun.h"
#include "config.h"
#include "parser.h"
#include "console.h"
#include "elfloader.h"
#include "dbg.h"


void loadAutorun()
{
	PARSER_ERROR_T		result;
	WCHAR				uri[64+1] = L"", params[64+1] = L"";
	ELFSTART_T			parser_data;
	
	if ( Config.SkipAutorun == TRUE )
		return;
	
	parser_data.uri = uri;
	parser_data.params = params;
	
	cprint("\x87""Autorun ...\n");
	
	result = ParseFile( Config.AutorunPath, ParserAutorunLine, &parser_data );
	
	switch( result )
	{
		case PARSE_SUCCESS:
			cprint("\x87""Autorun processed\n");
		break;
		
		case PARSE_FILE_EMPTY:
			cprint("\x87""Autorun is empty\n");
		break;
		
		case PARSE_FILE_NOT_EXIST:
			cprint("\x8C""Autorun does not exist\n");
		break;
		
		case PARSE_NO_MEMORY:
			cprint("\x8C""Not enough memory for autorun\n");
		break;
		
		default:
			dbgf( "ERROR = %d\n", result );
		break;
	}
}


