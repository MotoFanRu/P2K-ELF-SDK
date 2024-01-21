#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "API.h"
#include "config.h"


#define SCR_W			176
#define SCR_H			220


#define GLYPH_NUM		96
#define GLYPH_W			5
#define GLYPH_H			12

#define STRLEN			(SCR_W/GLYPH_W)
#define LINENUM			8

#define BUFLINENUM		(3 * LINENUM)

#define WINDOW_W		(GLYPH_W*STRLEN + 0)
#define WINDOW_H		(GLYPH_H*LINENUM + GLYPH_H/2)
#define WINDOW_X		((SCR_W-WINDOW_W)/2)
#define WINDOW_Y		16 //((SCR_H*9)/100)

#define TEXT_X			0
#define TEXT_Y			(GLYPH_H/4)

#define TID_FADEIN		0xA0001
#define TID_FADEOUT		0xA0002
#define TID_STAY		0xA0003

#define STEP_FADEIN		1
#define STEP_FADEOUT	1

#define TIME_STAY		5000
#define TIME_FADEIN		50
#define TIME_FADEOUT	50


#define TARGET_STEPS	0x09

#define CONSOLE_TEXT_COLOR		0x0007
#define CONSOLE_BG_COLOR		0x0000
#define CONSOLE_BORDER_COLOR	0xFFFF


typedef enum
{
	CON_TYPE_ALPHA,
	CON_TYPE_SHIFT,
	CON_TYPE_MAX
	
} CON_TYPE_T;

typedef enum
{
	CON_ACTION_HIDE,
	CON_ACTION_FADEIN,
	CON_ACTION_STAY,
	CON_ACTION_FADEOUT,
	CON_ACTION_MAX
	
} CON_ACTION_T;


extern UINT8			conType;
extern INT8				conStep;
extern UINT8			conLogo;
extern BOOL				conAutoHide;
extern UINT8			conAction;


typedef struct
{
	UINT8			len;
	UINT8			color;
} COL_STRIDE_T;

typedef struct
{
	UINT16			num;
	COL_STRIDE_T	strides[STRLEN];
} COL_LINE_T;


#ifdef __cplusplus
extern "C" {
#endif

BOOL ConsoleInit( void );
void ConsoleDone( void );

void ConsoleRender( const RECT_T *updatedRect );

void cprint( const char *str );
void cprintf( const char *format, ... );

void printLogo( void );
void cscroll( int step );

#define EP_PROMPT "\x82""EP2 "
// cprintf for internal use, to make control of SilentMode
#define cprint_int( format ) do if ( Config.DebugMode ) \
	cprintf( EP_PROMPT format ); while(0)
#define cprintf_int( format, ... ) do if ( Config.DebugMode ) \
	cprintf( EP_PROMPT format, ## __VA_ARGS__ ); while(0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __CONSOLE_H__ */
