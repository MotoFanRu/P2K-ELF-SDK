#include "dd.h"
#include <ATI.h>
#include <DAL.h>
#include <mem.h>


BOOL				ahiInited;

AHIDEVICE_T			ahiDev;
AHIDRVINFO_T		*ahiDrvInfo;

AHISURFACE_T		sDisp, sDraw;
AHIDEVCONTEXT_T		devCx;



BOOL ATI_Init( void )
{
	INT32			err;
	
	ahiInited = FALSE;
	
	ahiDrvInfo = suAllocMem( sizeof(AHIDRVINFO_T), &err );
	if ( !ahiDrvInfo || err ) return FALSE;
	
	err = AhiDevEnum( &ahiDev, ahiDrvInfo, 0 );
	if ( err != 0 ) return FALSE;
	
	err = AhiDevOpen( &devCx, ahiDev, "ElfPack2", 0 );
	if ( err != 0 ) return FALSE;
	
	ahiInited = TRUE;
	
	AhiDispSurfGet( devCx, &sDisp );
	
	sDraw = DAL_GetDrawingSurface( 0 );
	
	return TRUE;
}


BOOL ATI_Done( void )
{
	if ( ahiDrvInfo )
		suFreeMem( ahiDrvInfo );
	
	if ( ahiInited )
		AhiDevClose( devCx );
	
	ahiInited = FALSE;
	
	return TRUE;
}

/* EOF */
