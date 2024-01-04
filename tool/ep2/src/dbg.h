#ifndef __DBG_H__
#define __DBG_H__

#include <typedefs.h>
#include <utilities.h>
#include "utils.h"

#include <dl.h>

#define LOG_FILE_URI L"file://b/elfpack.txt"

#define PRELOG "EP2> " __func__ ": "
//  "$" __MODULE__"@" __LINE__

#ifdef DEBUG
	#define dbg( format ) UtilDebug( "%s" format, PRELOG )
	#define dbgf( format, ... ) UtilDebug( "%s" format, PRELOG, ## __VA_ARGS__ )
#else
	#define dbg(...)
	#define dbgf(...)
#endif




#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __DBG_H__ */
