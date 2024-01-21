
#include "elfloaderApp.h"
#include <typedefs.h>
#include <dl_keypad.h>
#include <utilities.h>
#include <dl.h>
#include <mem.h>
#include "autorun.h"
#include "config.h"
#include "console.h"
#include "utils.h"
#include "lolvl.h"
#include "dd.h"
#include "dbg.h"


const char app_name[APP_NAME_LEN] = "ElfPack2"; 


IFACE_DATA_T		ctimer_iface;		// console timer
IFACE_DATA_T		ktimer_iface;		// key timer
IFACE_DATA_T		rtimer_iface;		// run-delay timer
UINT8				lkey;				// long-press key
UINT8				lcount;				// long-press count ticks
BOOL				lconsume;			// do consume key event ?

UINT32				currentEventBase;
UINT32				eventElfsListChanged;
UINT32				eventRequestRender;


static void ElfPackRegister( void );

static UINT32 ElfPackInit( APPLICATION_T *app );
static UINT32 ElfPackLoad( void );

static UINT32 ElfPackStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
static UINT32 ElfPackExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 HandleTimer( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 HandleKeyrelease( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 HandleConsole( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 HandleReloadLib( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 ActionPress( UINT8 key, UINT8 delay );
static UINT32 ActionFastRunEvent( void );
static UINT32 ActionFastRunElf( void );



static const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
//	{ EV_LOADELF,			Handle_LoadELF	},
//	{ EV_UNLOADELF,			Handle_UnloadELF },
	{ EV_TIMER_EXPIRED,		HandleTimer		},
	{ EV_KEY_PRESS,			HandleKeypress	},
	{ EV_KEY_RELEASE,		HandleKeyrelease},
	{ EV_CONSOLE,			HandleConsole	},
	{ EV_RELOAD_LIB,		HandleReloadLib	},
	{ STATE_HANDLERS_END,	NULL			}
};


static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
	{
		STATE_ANY,
		NULL,
		NULL,
		any_state_handlers
	}
};


//#pragma arm section code = "Header"
//extern void APP_SyncML_MainRegister(void);
extern void APP_SYNCMLMGR_MainRegister(void);

void elfpackEntry(void)
{
	// Placeholder for replaced RegFn
	FTR_REPLACED_REGISTER_FUNC();
	
	ElfPackRegister();		//Запускаем ElfPack2
	
	cbkDisplayInit();
}


void ElfPackRegister( void )
{
	UINT32				startev = EV_STARTLDR;
	UINT32				status;

	status = APP_Register( &startev,
						1,
						state_handling_table,
						STATE_MAX,
						(void*)ElfPackStart );

	dbgf( "status = %d\n", status );

	ldrSendEvent( startev );
}


UINT32 ElfPackInit( APPLICATION_T *app )
{
	UINT32			status = RESULT_OK;
	
	
	dbg( "enter\n" );
	
	__rt_memclr( (void*)fnLibraries, MAX_LIBRARIES * sizeof(Ldr_Lib*) );
	__rt_memclr( (void*)&cstLibrary, sizeof(CONST_LIBRARY_T) );
	fnLibCount = 0;
	
	loadedElfsList.count = 0;
	loadedElfsList.root = loadedElfsList.tail = NULL;
	eventElfsListChanged = (UINT32)(-1);
	eventRequestRender = (UINT32)(-1);
	
	currentEventBase = EV_ELFS_BASE;
	
	ktimer_iface.port = app->port;
	ktimer_iface.handle = 0;
	lkey = 0xff;
	lcount = 0;
	lconsume = FALSE;
	
	ctimer_iface.port = app->port;
	ctimer_iface.handle = 0;
	
	rtimer_iface.port = app->port;
	rtimer_iface.handle = 0;
	
	setDefaultConfig( &Config );
	
	
	status = ATI_Init();
	dbgf( "ATI_Init = 0x%x\n", status );
	
	if ( status )
	{
		status = ConsoleInit();
		dbgf( "ConsoleInit = 0x%x\n", status );
		
		conType = CON_TYPE_SHIFT;
		
		//cprint( "\x89""EP2.0>\x82 Initialisation ...\n" );
	}
	
	dbgf( "exit, 0x%X\n", status );
	
	return RESULT_OK;
}


UINT32 ElfPackLoad( void )
{
	UINT32			status = RESULT_OK;
	BOOL			asc;
#ifdef DEBUG
	char			cstr[128];
#endif
	
	dbg( "enter\n" );
	
	status = ldrLoadConfig();
	dbgf( "loadConfig = 0x%x\n", status );
	
#ifdef DEBUG
	dbgf( "loadConfig: {%d, %d, %d, %d, [%d], ...}\n", 
			Config.EnableConsole, 
			Config.AutoShowConsole, 
			Config.DebugMode, 
			Config.ShowLogo, 
			u_strlen(Config.AutorunPath) );
	u_utoa( Config.AutorunPath, cstr );
	dbgf( "AutorunPath[%d] = %s\n", strlen(cstr), cstr );
#endif
	
	cprint( "\x89""EP2.0>\x82 Initialisation ...\n" );
	conAutoHide = FALSE;
	
	status = loadDefaultLib( NULL );
	dbgf( "loadDefaultLib = 0x%x\n", status );
	
	eventElfsListChanged = ldrGetConstVal( EV_LDR_ELFLIST_CHANGED );
	eventRequestRender = ldrGetConstVal( EV_REQUEST_RENDER_ );
	
	cprint( "\x89""EP2.0>\x82 Initialisation complete\n" );
	
#if FTR_KEYPAD_TYPE != KP_NONE
	if ( FTR_NOAUTORUN_KEY != 0xFF && lolvl_check_key( FTR_NOAUTORUN_KEY ) )
	{
		cprint("\x8E""Autorun skipped\n");
	}
	else
#endif
		loadAutorun();
	
	conAutoHide = TRUE;
	conType = CON_TYPE_ALPHA;
	
	if ( Config.ShowLogo )
	{
		asc = Config.AutoShowConsole;
		Config.AutoShowConsole = 1;
		printLogo();
		Config.AutoShowConsole = asc;
	}
	
	dbgf( "exit, 0x%X\n", status );
	
	return status;
}


UINT32 ElfPackStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
	APPLICATION_T		*app = NULL;
	UINT32				status = RESULT_OK;


	dbg( "enter\n" );

	app = APP_InitAppData( (void *)APP_HandleEventPrepost,
							sizeof(APPLICATION_T),
							reg_id,
							0, 0,
							1,
							AFW_APP_CENTRICITY_NONE, 
							AFW_PREPROCESSING, 
							AFW_POSITION_P1 );


	dbgf( "app = 0x%X\n", app );
	if ( !app ) return RESULT_OK;
	
	ElfPackInit( app );
	
	status = APP_Start(	ev_st,
						app,
						STATE_ANY,
						state_handling_table,
						ElfPackExit,
						app_name,
						0 );
	
	dbgf( "APP_Start = 0x%X\n", status );
	if ( status != RESULT_OK )
	{
		APP_HandleFailedAppStart( ev_st, app, 0 );
		return RESULT_OK;
	}
	
	
	rtimer_iface.handle = CreateTimer( TID_RUN_DELAY, TIME_RUN_DELAY, 
							ttSimple, rtimer_iface.port );
	
	
	dbgf( "exit, 0x%X\n", status );
	
	return RESULT_OK;
}


UINT32 ElfPackExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT32			status;
	
	StopTimer( ctimer_iface.handle );
	ctimer_iface.handle = 0;
	
	StopTimer( ktimer_iface.handle );
	ktimer_iface.handle = 0;
	
	StopTimer( rtimer_iface.handle );
	rtimer_iface.handle = 0;
	
	status = APP_ExitStateAndApp( ev_st, app, NULL );
	
	ConsoleDone();
	
	ATI_Done();
	
	return status;
}


UINT32 HandleTimer( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT32			tid = GET_TIMER_ID( ev_st );
	BOOL			consume = TRUE;
	
	
	switch( tid )
	{
		case TID_RUN_DELAY:
			rtimer_iface.handle = 0;
			
			ElfPackLoad();
			break;
		
		case TID_LKEY:
			if ( lkey != 0xff )
			{
				lcount++;
			}
			
			if ( conAction == CON_ACTION_STAY )
			{
				if ( lkey == KEY_VOL_UP )
					cscroll( -1 );
				else if ( lkey == KEY_VOL_DOWN )
					cscroll( +1 );
			}
			break;
		
		case TID_FADEIN:
			conStep += STEP_FADEIN;
			if ( conStep >= TARGET_STEPS )
			{
				conStep = TARGET_STEPS;
				conAction = CON_ACTION_STAY;
				
				StopTimer( ctimer_iface.handle );
				if ( conAutoHide )
					ctimer_iface.handle = CreateTimer( TID_STAY, TIME_STAY, 
							ttSimple, ctimer_iface.port );
				else
					ctimer_iface.handle = 0;
			}
			
			ConsoleRender( NULL );
			break;
		
		case TID_FADEOUT:
			conStep -= STEP_FADEOUT;
			if ( conStep <= 0 )
			{
				conStep = 0;
				conAction = CON_ACTION_HIDE;
				
				StopTimer( ctimer_iface.handle );
				ctimer_iface.handle = 0;
				
				// reset scroll
				cscroll( 0 );
			}
			
			ConsoleRender(NULL);
			
			if ( conStep == 0 )
				ldrSendEvent( eventRequestRender ); // чтоб экран обновился
			break;
		
		case TID_STAY:
			conAction = CON_ACTION_FADEOUT;
			
			ctimer_iface.handle = CreateTimer( TID_FADEOUT, TIME_FADEOUT, ttCyclical, ctimer_iface.port );
			break;
		
		default:
			consume = FALSE;
	}
	
	if ( consume )
		APP_ConsumeEv( ev_st, app );
	
	return RESULT_OK;
}


UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT8		key = GET_KEY( ev_st );;
	
	
	if ( KeypadLock() ) return RESULT_OK;
	
	if ( ktimer_iface.handle )
		StopTimer( ktimer_iface.handle );
	ktimer_iface.handle = CreateTimer( TID_LKEY, TIME_LKEY, ttCyclical, ktimer_iface.port );
	lkey = key;
	lcount = 0;
	
	switch ( key )
	{
		case KEY_VOL_UP:
			if ( conAction == CON_ACTION_STAY )
			{
				cscroll( -1 );
				if ( ctimer_iface.handle )
				{
					StopTimer( ctimer_iface.handle );
					ctimer_iface.handle = CreateTimer( TID_STAY, TIME_STAY, 
							ttSimple, ctimer_iface.port );
				}
				lconsume = TRUE;
			}
			break;
		
		case KEY_VOL_DOWN:
			if ( conAction == CON_ACTION_STAY )
			{
				cscroll( +1 );
				if ( ctimer_iface.handle )
				{
					StopTimer( ctimer_iface.handle );
					ctimer_iface.handle = CreateTimer( TID_STAY, TIME_STAY, 
							ttSimple, ctimer_iface.port );
				}
				lconsume = TRUE;
			}
			break;
		
		default:
			lconsume = FALSE;
			break;
	}
	
	if ( lconsume )
	{
		APP_ConsumeEv( ev_st, app );
		ldrSendEvent( EV_DISPLAY_ACTIVE );
	}
	
	return RESULT_OK;
}


UINT32 HandleKeyrelease( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT8		key = GET_KEY( ev_st );
	
	//dbgf( "key = 0x%X\n", key );
	
	if ( KeypadLock() ) return RESULT_OK;
	
	if ( ktimer_iface.handle )
	{
		StopTimer( ktimer_iface.handle );
		ktimer_iface.handle = 0;
	}
	
	if ( lkey != 0xff && key == lkey )
	{
		ActionPress( key, lcount );
	}
	
	lkey = 0xff;
	lcount = 0;
	
	switch ( key )
	{
		
		break;
	}
	
	if ( lconsume )
	{
		APP_ConsumeEv( ev_st, app );
		lconsume = FALSE;
	}
	
	return RESULT_OK;
}


UINT32 ActionPress( UINT8 key, UINT8 delay )
{
	if ( key == FTR_FAST_KEY )
	{
		switch ( delay )
		{
			case 0:
			//case 1:
				ActionFastRunEvent();
				break;
			
			case 1:
			case 2:
				ActionFastRunElf();
				break;
			
			case 3:
			case 4:
			//case 5:
				dbg("short FAST_ACCESS key pressed, show/hide console\n");
				ldrToggleConsole();
				break;
			
			case 6:
			case 7:
				break;
			
			case 8:
			case 9:
				dbg( "x2-long FAST_ACCESS key pressed, reload cfg and library\n" );
				ldrLoadConfig();
				loadDefaultLib( NULL );
				break;
		}
	}
	
	return RESULT_OK;
}


UINT32 ActionFastRunEvent( void )
{
	if ( Config.FastRunEvent > 0)
	{
		dbg("long FAST_ACCESS key pressed, send event\n");
		ldrSendEvent( Config.FastRunEvent );
	}
	else
	{
		dbg("long FAST_ACCESS key pressed, no action\n");
		cprint_int("\x8E""No action specified\n");
	}
	
	//dbg("long FAST_ACCESS key pressed, show/hide console\n");
	//ldrToggleConsole();
	
	return RESULT_OK;
}


UINT32 ActionFastRunElf( void )
{
	if ( u_strlen( Config.FastRunElf ) > 0 )
	{
		dbg("long FAST_ACCESS key pressed, fastrun elf\n");
		ldrLoadElf( Config.FastRunElf, NULL );
	}
	else
	{
		dbg("long FAST_ACCESS key pressed, no action\n");
		cprint_int("\x8E""No action specified\n");
	}
	
	//dbg("long FAST_ACCESS key pressed, show/hide console\n");
	//ldrToggleConsole();
	
	return RESULT_OK;
}


UINT32 HandleConsole( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	dbg( "Handle console event" );
	
	ldrToggleConsole();
	
	return RESULT_OK;
}


UINT32 HandleReloadLib( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	dbg( "Handle reload event" );
	
	ldrLoadConfig();
	loadDefaultLib( NULL );
	
	return RESULT_OK;
}


/* EOF */
