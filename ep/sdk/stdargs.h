// UTF-8 w/o BOM

/* 
** передача аргументов (...) отличается для ADS и GNU C
*/

// ADS (ElfPack v1.0)
#if defined( __arm )

	#include <stdarg1.h>

// GCC (ElfPack v2.0)
#elif defined( __GNUC__ )

	#include <stdarg2.h>

// Windows (EmuElf)
#elif defined( WIN32 )

	#include <stdarg.h>

#endif
