#ifndef __ELFLOADERAPP_H__
#define __ELFLOADERAPP_H__

#include <typedefs.h>
#include "elfloader.h"
//#include "utils.h"
#include "features.h"



#define EV_STARTLDR				(FTR_EVBASE_CODE)
#define EV_ELFS_BASE			(FTR_EVBASE_CODE + 0x100)

#define EV_CONSOLE				(FTR_CONSOLE_EVENT)
#define EV_RELOAD_LIB			(FTR_CONSOLE_EVENT+1)

#define EV_LDR_ELFLIST_CHANGED	0x1900 // must eq from const.h
#define EV_REQUEST_RENDER_		0x1104

#define TID_LKEY				0xA0010
#define TIME_LKEY				250

#define TID_RUN_DELAY			0xA0011
#define TIME_RUN_DELAY			3000


typedef enum
{
	STATE_ANY,
	STATE_MAX

} STATE_T;

extern IFACE_DATA_T		ctimer_iface;

extern UINT32			currentEventBase;
extern UINT32			eventElfsListChanged;


#ifdef __cplusplus
extern "C" {
#endif


void elfpackEntry( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __ELFLOADERAPP_H__ */
