// UTF-8 w/o BOM
/* ***********************************
 *	Функции, входщие в elfloader
 */
 
#ifndef SDK_ELFLOADER_H
#define SDK_ELFLOADER_H

#include <typedefs.h>
#include <stdargs.h>

#ifdef __cplusplus
extern "C" {
#endif


/*typedef struct
{
    char uri[64];
    char params[64];
} ELF_PARAMS_T;*/

extern void * Lib;

UINT32  LdrStartApp( EVENT_CODE_T ev_start );
UINT32  LdrLoadELF( WCHAR *uri,  WCHAR *params );
UINT32  LdrUnloadELF( void *elf_ptr );

const char* LdrGetPhoneName( void );
const char* LdrGetPlatformName( void );
const char* LdrGetFirmwareMajorVersion( void );
const char* LdrGetFirmwareMinorVersion( void );

void UtilLogStringData( const char*  format,  ... );	// for p2k Data loger

// Quick compare strings
UINT32 namecmp( const char * str1, const char * str2 ); //returns 1 if equal


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // SDK_ELFLOADER_H
