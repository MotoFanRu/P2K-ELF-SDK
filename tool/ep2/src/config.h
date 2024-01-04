
#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef WIN32
	#include "..\parser_win32\wrapper.h"
#else
	#include <typedefs.h>
#endif


#define LIBPATHCNT		3

typedef struct
{
	BOOL		EnableConsole;
	BOOL		AutoShowConsole;
	BOOL		DebugMode;
	BOOL		SkipAutorun;
	BOOL		ShowLogo;
	WCHAR*		AutorunPath;
	WCHAR*		FastRunElf;
	UINT32		FastRunEvent;
	WCHAR*		DefLibraryPath;
	WCHAR*		LibsPaths[LIBPATHCNT];
	
} LDR_CONFIG_T;


extern LDR_CONFIG_T Config;


#ifdef __cplusplus
extern "C" {
#endif


void setDefaultConfig( LDR_CONFIG_T * cfg );

UINT32 loadConfig( LDR_CONFIG_T * cfg, const WCHAR * path );
UINT32 setConfig( const LDR_CONFIG_T * cfg );
UINT32 copyConfig( LDR_CONFIG_T * cfg );
UINT32 freeConfig( LDR_CONFIG_T * cfg );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __CONFIG_H__ */
