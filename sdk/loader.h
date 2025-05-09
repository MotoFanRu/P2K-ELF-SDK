// UTF-8 w/o BOM

#ifdef EP2
#include <loader2.h>
#else
#include <loader1.h>
#define ldrGetConstVal(cid) (cid)
#define cprint              dbg
#define cprintf             dbgf
#endif

#if defined(__arm) /* ADS */
#define GET_DATA_FROM_LIB(x) (x)
#elif defined(__GNUC__) /* GCC */
#define GET_DATA_FROM_LIB(x) (&x)
#else /* ??? */
#error "Sorry, unknown or undetermined compiler."
#endif
