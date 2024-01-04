#ifndef __DD_H__
#define __DD_H__

#include <typedefs.h>
#include <ATI.h>


extern BOOL				ahiInited;

extern AHIDEVICE_T		ahiDev;
extern AHIDRVINFO_T		*ahiDrvInfo;

extern AHISURFACE_T		sDisp, sDraw;
extern AHIDEVCONTEXT_T	devCx;


#ifdef __cplusplus
} /* extern "C" */
#endif


BOOL ATI_Init( void );
BOOL ATI_Done( void );


#ifdef __cplusplus
extern "C" {
#endif


#endif /* __DD_H__ */
