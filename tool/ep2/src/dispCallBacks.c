
#include "API.h"
#include "utils.h"
#include <mem.h>


#define MAX_DISPCBK 16

static dispCbk		dispCallbacks[MAX_DISPCBK];
static RECT_T		dispCbkRects[MAX_DISPCBK];
static UINT16		dispCbkCount;
static UINT16		dispCbkPostFlags;



void cbkDisplayInit()
{
	__rt_memclr( dispCallbacks, MAX_DISPCBK*sizeof(dispCbk) );
	__rt_memclr( dispCbkRects, MAX_DISPCBK*sizeof(RECT_T) );
	dispCbkCount = 0;
	dispCbkPostFlags = 0;
}


BOOL ldrDisplayCbkReg( dispCbk cbk, const RECT_T *target, BOOL postflush )
{
	UINT32			i = 0;
	const RECT_T	null_rect = {0,0,0,0};
	
	if ( cbk == NULL ) return FALSE;
	
	while ( (dispCallbacks[i] != NULL) && (i < MAX_DISPCBK) ) 
		i++;
	
	if ( i == MAX_DISPCBK ) return FALSE;
	
	dispCallbacks[i] = cbk;
	
	if ( target == NULL ) /* caller want to register CB on any display update */
		dispCbkRects[i] = null_rect;
	else
		dispCbkRects[i] = *target;
	
	if ( postflush )
		dispCbkPostFlags |= ( 1<<i );
	else
		dispCbkPostFlags &= ~( 1<<i );
	
	return TRUE;
}


BOOL ldrDisplayCbkUnReg( dispCbk cbk )
{
	UINT32		i = 0;
	
	if ( cbk == NULL ) return FALSE;
	
	for ( i=0; i<MAX_DISPCBK; i++ )
		if ( dispCallbacks[i] == cbk )
		{
			dispCallbacks[i] = NULL;
			//return TRUE;
			/* Breaking unnecessary, since we can register a single CB on multiple rects.
			   Thus, this function will unreg all registered entries of the function specified. */
		}
	
	return TRUE;
}


void cbkDisplayHookPre( const RECT_T * updatedRect, void * src, UINT8 displayID )
{
	UINT32		i, bit;
	
	/*dbg( "cbkDisplayHook source: 0x%X",   (UINT32)src );
	dbg( " CALLBACK: Start: %d, %d, %d, %d", updatedRect->x1,
														  updatedRect->y1,
														  updatedRect->x2,
														  updatedRect->y2 );*/
	
	for ( i=0, bit=1; i<MAX_DISPCBK; i++, bit<<=1 )
	{
		if (	dispCallbacks[i] != NULL && 
				(dispCbkPostFlags & bit) == 0 && 
				UtilIsRectIntersect( updatedRect, &dispCbkRects[i] )	)
		{
			(dispCallbacks[i])( updatedRect );
		}
	}
}


void cbkDisplayHookPost( const RECT_T * updatedRect, void * src, UINT8 displayID )
{
	UINT32		i, bit;
	RECT_T		clipped;
	
	for ( i=0, bit=1; i<MAX_DISPCBK; i++, bit<<=1 )
	{
		if (	dispCallbacks[i] != NULL && 
				(dispCbkPostFlags & bit) != 0 && 
				UtilRectIntersect( &clipped, updatedRect, &dispCbkRects[i] )	)
		{
			(dispCallbacks[i])( &clipped );
		}
	}
}


/* EOF */
