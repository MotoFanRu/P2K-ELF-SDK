// UTF-8 w/o BOM

/*
** передача аргументов (...) отличается для ADS и GNU C
*/

// ADS (ElfPack v1.x)
#if defined(__arm)

#include <stdarg1.h>

// GCC (ElfPack v2.x)
#elif defined(__GNUC__)
#if defined(EA1) || defined(USE_GCC_VA_ARGS) || defined(EP2)
#include <stdarg2.h>
#else
// TODO!
// typedef int *va_list[1];
// #define va_start(ap, parmN) (void)(*(ap) = (int*)(&parmN + 1))
// #define va_end(ap) ((void)(*(ap) = 0))

// TODO
typedef __builtin_va_list *va_list;
#define va_start(a, b) \
	{ \
		__builtin_va_list tmp; \
		__builtin_va_start(tmp, b); \
		a = &tmp; \
	}
#define va_end(a) __builtin_va_end(*a)
#endif

// Windows (EmuElf)
#elif defined(WIN32)

#include <stdarg.h>

#endif
