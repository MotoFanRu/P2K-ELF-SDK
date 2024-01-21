// UTF-8 w/o BOM

#ifdef EP2
	#include <loader2.h>
#else
	#include <loader1.h>
	#define ldrGetConstVal(cid)		(cid)
	#define cprint dbg
	#define cprintf dbgf
#endif
