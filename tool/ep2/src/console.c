
#include "console.h"
#include <stdarg.h>
#include <mem.h>
#include <utilities.h>
#include <ATI.h>
#include "dd.h"
#include "utils.h"
#include "elfloaderApp.h"
#include "config.h"
#include "dbg.h"


UINT16				conAbsCurLine;		// absolute cur line number 0..?
UINT16				conAbsTopLine;		// absolute top line number 0..?
UINT16				conTopLine;
UINT16				conCurLine;			// 0..BUFLINENUM-1
UINT16				conPos;				// col in curLine 0..LINENUM-1
char				*conData = NULL;

UINT32				conLastStride;
COL_LINE_T			*coloring = NULL;

//INT8				conAlpha;
UINT8				conType;
INT8				conStep;
UINT8				conLogo;
BOOL				conAutoHide;
UINT8				conAction;

BOOL				conInited;

extern const unsigned char ElfPackLogo[];
extern const unsigned char font_bits[];
extern const UINT16 palette[];


const RECT_T		conMainRect = { WINDOW_X, WINDOW_Y, 
									WINDOW_X+WINDOW_W+1, WINDOW_Y+WINDOW_H+1 };


static void ConsoleUpdate( BOOL forceShow );


BOOL ConsoleInit( void )
{
	INT32			err;

	dbg( "enter\n" );

	conInited = FALSE;
	
	if ( !ahiInited )
		if ( !ATI_Init() ) return FALSE;
	
	conTopLine = conCurLine = 0;
	conAbsTopLine = conAbsCurLine = 0;
	conPos = 0;
	
	conData = suAllocMem( STRLEN*BUFLINENUM * sizeof(char), &err );
	if ( !conData ) return FALSE;
	
	coloring = suAllocMem( BUFLINENUM * sizeof(COL_LINE_T), &err );
	if ( !coloring ) 
	{
		suFreeMem( conData );
		return FALSE;
	}
	
	__rt_memclr( conData, STRLEN*BUFLINENUM * sizeof(char) );
	__rt_memclr( coloring, BUFLINENUM * sizeof(COL_LINE_T) );
	
	conLastStride = 1;
	
	coloring[0].strides[0].len = 0;
	coloring[0].strides[0].color = CONSOLE_TEXT_COLOR;
	
	conType = CON_TYPE_ALPHA;
	conStep = 0;
	conLogo = 0;
	conAutoHide = TRUE;
	conAction = CON_ACTION_HIDE;
	
	conInited = ldrDisplayCbkReg( ConsoleRender, &conMainRect, TRUE );
	
	dbgf( "exit, %d\n", conInited );
	
	return TRUE;
}


void ConsoleDone( void )
{
	if ( conInited )
		ldrDisplayCbkUnReg( ConsoleRender );
	
	if ( conData )
		suFreeMem( conData );
	
	if ( coloring )
		suFreeMem( coloring );
}


void printLogo( void )
{
	cprint( (char*)ElfPackLogo );
	conLogo = 1;
}


void cprintf( const char *format, ... )
{
	char		buffer[128];
	va_list		vars;
	
	if ( !conInited )
		return;
	
	va_start( vars, format );
	vsnprintf( buffer, 127, format, vars );
	va_end( vars );
	
	cprint( buffer );
}


void cscroll( int step )
{
	int top = conAbsTopLine + step;
	int ub = conAbsCurLine - min( BUFLINENUM, conAbsCurLine );
	int bb = conAbsCurLine - min( LINENUM, conAbsCurLine );
	
	// reset top
	if ( step == 0 )
	{
		conAbsTopLine = bb;
		conTopLine = conAbsTopLine % BUFLINENUM;
		return;
	}
	
	// top upper bound
	if ( top < ub ) top = ub;
	
	// top bottom bound
	if ( top > bb ) top = bb;
	
	conAbsTopLine = top;
	conTopLine = conAbsTopLine % BUFLINENUM;
	
	ConsoleUpdate( FALSE );
}


void cprint( const char * str )
{
	int				i=0;//,j;
	char			*dst;
	UINT32			lastStridePos = conPos;
	UINT8			color;
	COL_LINE_T		*cline;
	COL_STRIDE_T	*cstride;
	
	
	if ( !Config.EnableConsole ) return;
	if ( !str ) return;
	dbgf( "str = '%s'\n", str );
	
	if ( !conInited )
		return;
	
	dst = &conData[conCurLine*STRLEN];
	
	cline = &coloring[conCurLine];
	cstride = cline->strides;
	
	i = 0;
	while ( str[i] != 0 )
	{
		// Обычный символ
		if ( ( str[i]>=0x20 && str[i]<=0x7F ) || ( str[i]>=0x90 && str[i]<=0xFF ) )
		{
			dst[conPos] = str[i] - 0x20;
			conPos++;
			
			// Проверка на заполнение строки
			if ( conPos == STRLEN )
			{
				// Необходимо закончить stride для текущей строки
				cstride[conLastStride-1].len = conPos - lastStridePos;
				color = cstride[conLastStride-1].color;
				cline->num = conLastStride;
				conLastStride = 1;
				lastStridePos = 0;
				
				conPos = 0;
				
				if ( conCurLine == BUFLINENUM-1)
				{
					conCurLine = 0;
					// Переводим указатель
					dst = conData;
					cline = coloring;
				} 
				else 
				{
					conCurLine++;
					dst += STRLEN;
					cline++;
				}
				
				conAbsCurLine++;
				
				// Необходимо продолжить stride на новой строке
				cstride = cline->strides;
				cstride[0].color = color;

				//j = conCurLine - conTopLine;
				//if ( (j >= LINENUM) || (j<0)) conTopLine++;
				if ( conAbsTopLine == conAbsCurLine - LINENUM - 1 ) 
					conAbsTopLine++;
				conTopLine = conAbsTopLine % BUFLINENUM;
			} // if ( conPos == STRLEN )
		} // if ( Обычный символ )

		// Перевод строки
		if (str[i] == '\n')
		{
			// Необходимо закончить stride для текущей строки
			cstride[conLastStride-1].len = conPos - lastStridePos;
			cline->num = conLastStride;
			conLastStride = 1;
			lastStridePos = 0;
			
			conPos = 0;
			
			if (conCurLine == BUFLINENUM-1)
			{
				conCurLine = 0;
				dst = conData;
				cline = coloring;
			}
			else 
			{
				conCurLine++;
				dst += STRLEN;
				cline++;
			}
			
			conAbsCurLine++;
			
			// stride на новой строке должен быть дефолтным
			cstride = cline->strides;
			cstride[0].color = CONSOLE_TEXT_COLOR;
			
			//j = conCurLine - conTopLine;
			//if ( (j > LINENUM) || (j<0)) conTopLine++;
			if ( conAbsTopLine == conAbsCurLine - LINENUM - 1 ) 
				conAbsTopLine++;
			conTopLine = conAbsTopLine % BUFLINENUM;
		}
		
		// Инструкции по раскраске - [0x80, 0x8F]
		if ( (str[i]>=0x80) && (str[i]<=0x8F) )
		{
			cstride[conLastStride-1].len = conPos - lastStridePos;
			cstride[conLastStride++].color = str[i] & 0xF;
			lastStridePos = conPos;
		}
		
		i++;
	}

	// Проверка на незаконченную строку
	if ( i > 0 && str[i-1] != '\n' )
	{
		// Необходимо закончить stride для текущей строки
		cstride[conLastStride-1].len = conPos - lastStridePos;
		cstride[conLastStride].color = cstride[conLastStride-1].color;
		conLastStride++;
		cline->num = conLastStride-1;
	}
	
	ConsoleUpdate( Config.AutoShowConsole );
	
	//dbgf( "ctimer_iface.handle = 0x%08x\n", ctimer_iface.handle );
	//dbgf("at=%d, ac=%d,t=%d, c=%d\n", conAbsTopLine, conAbsCurLine,
	//		conTopLine, conCurLine);
}


void ConsoleUpdate( BOOL forceShow )
{
	if ( conAction == CON_ACTION_HIDE ) 
		conType = CON_TYPE_ALPHA;
	
	if ( ( forceShow && conAction == CON_ACTION_HIDE ) || 
			conAction == CON_ACTION_FADEOUT )
	{
		// show console
		conAction = CON_ACTION_FADEIN;
		StopTimer( ctimer_iface.handle );
		ctimer_iface.handle = CreateTimer( TID_FADEIN, TIME_FADEIN, 
				ttCyclical, ctimer_iface.port );
		conAutoHide = TRUE;
	}
	else if ( conAction == CON_ACTION_STAY && conAutoHide == TRUE )
	{
		// reset STAY-timer
		StopTimer( ctimer_iface.handle );
		ctimer_iface.handle = CreateTimer( TID_STAY, TIME_STAY, 
				ttSimple, ctimer_iface.port );
	}
	
	if ( conAction == CON_ACTION_STAY )
	{
		// update console
		ConsoleRender( NULL );
	}
}


void ConsoleRender( const RECT_T * updatedRect )
{
	AHIRECT_T		rect;
	AHIPOINT_T		pt;
	int				i, j, k;
	AHICHAR_T		chars[STRLEN];
	char			*str;
	UINT32			curLine = conTopLine;
	COL_LINE_T		*cline;
	COL_STRIDE_T	*cstride;
	UINT32			alpha;
	INT16			shift;
	RECT_T			conRect;
	RECT_T			drawRect;
	
	if ( !Config.EnableConsole ) return;
	if ( conStep == 0 && updatedRect != NULL ) return;
	
	
	if ( conType == CON_TYPE_ALPHA )
	{
		conRect = conMainRect;
	}
	else // CON_TYPE_SHIFT
	{
		shift = (INT32)conStep * WINDOW_H / TARGET_STEPS;
		
		conRect.x1 = WINDOW_X;
		conRect.y1 = WINDOW_Y - WINDOW_H + shift;
		conRect.x2 = conRect.x1 + WINDOW_W;
		conRect.y2 = conRect.y1 + WINDOW_H;
	}
	
	if ( !updatedRect ) // screen
		UtilRectIntersect( &drawRect, &conRect, &conMainRect );
	else // 
		UtilRectIntersect( &drawRect, &conRect, updatedRect );
	
	
	//Render the console background
	if ( !updatedRect )
	{
		// by timer
		// required restore background
		// т.к. java не использует sDraw, то портит фон :(
		
		rect.x1 = conMainRect.x1;
		rect.y1 = conMainRect.y1;
		rect.x2 = conMainRect.x2 + 1;
		rect.y2 = conMainRect.y2 + 1;
		
		pt.x = rect.x1;
		pt.y = rect.y1;
		
		AhiDrawSurfSrcSet( devCx, sDraw, 0 );
		AhiDrawSurfDstSet( devCx, sDisp, 0 );
		AhiDrawClipDstSet( devCx, NULL );
		
		//AhiDispWaitVBlank( devCx, 0 );
		AhiDrawBitBlt( devCx, &rect, &pt );
	}
	
	
	if ( conStep == 0 )
		return;
	
	
	rect.x1 = drawRect.x1;
	rect.y1 = drawRect.y1;
	rect.x2 = drawRect.x2;
	rect.y2 = drawRect.y2;
	AhiDrawClipDstSet( devCx, &rect );
	
	
	//Draw Background
	
	AhiDrawSurfDstSet( devCx, sDisp, 0 );
	
	AhiDrawBrushFgColorSet( devCx, CONSOLE_BG_COLOR );
	AhiDrawBrushSet( devCx, NULL, NULL, 0x0, AHIFLAG_BRUSH_SOLID );
	AhiDrawRopSet( devCx, AHIROP3(AHIROP_PATCOPY) );
	
	alpha = conStep | (conStep<<8) | (conStep<<16);
	AhiDrawAlphaSet( devCx, alpha );
	
	AhiDrawAlphaBlt( devCx, &rect, NULL, NULL, NULL, AHIFLAG_ALPHA_SOLID );
	
	
	//Render the console contents
	
	AhiDrawRopSet( devCx, AHIROP3(AHIROP_SRCCOPY) );
	
	pt.y = conRect.y1 + TEXT_Y;
	
	for ( j=0; j<LINENUM; j++ )
	{
		// conTopLine - номер верхней строки (скролл)
		str = &conData[curLine * STRLEN];
		
		pt.x = conRect.x1 + TEXT_X;
		
		cline = &coloring[curLine];
		cstride = cline->strides;
		for ( i=0; i < cline->num; i++ )
		{
			AhiDrawFgColorSet( devCx, palette[cstride->color] );
			for ( k=0; k<cstride->len; k++ )
			{
				chars[k].stridex = GLYPH_W;
				chars[k].stridey = 0;
				chars[k].w = GLYPH_W;
				chars[k].h = GLYPH_H;
				chars[k].image = (void*)&font_bits[ (UINT8)str[k]*8];
			}
			
			AhiDrawChar( devCx, &pt, chars, cstride->len, 0 );
			
			pt.x += cstride->len * GLYPH_W;
			str += cstride->len;
			cstride++;
		}
		
		pt.y += GLYPH_H;
		if ( curLine == BUFLINENUM-1 ) 
			curLine = 0;
		else 
			curLine++;
	
	}
	
	
	AhiDrawClipDstSet( devCx, NULL );
}


/* EOF */
