/* ********************************************************************
 *  Debugger/Отладчик
 * ********************************************************************
 *  (c) G-XaD 2008
 *  version: 1.01.00
 * ********************************************************************/
/* ********************************************************************
 * G-XaD	20.07.2008	Up to version 1.01. Added DEBUG_TIME
 * ********************************************************************/

#include "dbg.h"


#define smalloc(sz)				suAllocMem(sz, NULL)
#define sfree(mb)				suFreeMem(mb)

#define TRACE_MALLOC_STR		"mem_dbg, malloc: "
#define TRACE_FREE_STR			"mem_dbg, free: "
#define TRACE_DATA_STR			"data: "
#define TRACE_INIT_TIMER		"timer INIT: "
#define TRACE_TIMER				"timer: "

//static UINT32	dbg_timer = 0;


void * dbg_malloc( const char * dbg_app_name, size_t size, const char * file, int line )
{
	void * status = NULL;
	INT32 result;
	status = suAllocMem( size, &result );
	TRACE("%s: " TRACE_MALLOC_STR "\"%s\"@%d: r(0x%x) %d b @ address 0x%p\n", 
		dbg_app_name,
		file,
		line,
		result, 
		size, 
		status);
	return status;
}

void dbg_free( const char * dbg_app_name, void * memblock, const char * file, int line )
{
	TRACE("%s: " TRACE_FREE_STR "\"%s\"@%d: @ address 0x%p\n", dbg_app_name, file, line, memblock);
	suFreeMem( memblock );
}

void dbg_data( const char * dbg_app_name, const void * data, UINT16 size, const char * file, int line )
{
	const char * d = (const char *) data;
	UINT16		i		= 0;
	BYTE		b	= 0;
	TRACE("%s: " TRACE_DATA_STR "\"%s\"@%d @ address 0x%p:\n",
		dbg_app_name,
		file,
		line,
		d);
	TRACE_STRING("  [");
	for ( i=0; i<size; i++ ) {
		b = *(d+i);
		TRACE("%02x", b);
	}
	TRACE_STRING("]\n");
}

void dbg_unicode( const char * dbg_app_name, const char * format, const WCHAR * s1, const char * file, int line )
{
	char * tmp = (char *) smalloc(u_strlen(s1)+1);
	u_utoa(s1, tmp);
	TRACE(format, dbg_app_name,  line, tmp);
	sfree(tmp);
}

void dbg_timer_init( const char * dbg_app_name, const char * file, int line, unsigned int * tinit )
{
	*tinit = suPalReadTime();
	TRACE("%s: " TRACE_INIT_TIMER "\"%s\"@%d: init_time = 0x%08x\n",
			dbg_app_name,
			file,
			line,
			*tinit
	);
}

void dbg_time( const char * dbg_app_name, const char * file, int line, unsigned int tinit )
{
	UINT32 tm = (UINT32) suPalReadTime();
	UINT32 dtm = tm - tinit;
	TRACE("%s: " TRACE_TIMER "\"%s\"@%d tm = 0x%08x, dtm = +%d\n",
			dbg_app_name,
			file,
			line,
			tm,
			dtm
	);
}
