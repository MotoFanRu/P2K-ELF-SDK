#ifndef ELFLOADERAPP_H
#define ELFLOADERAPP_H

#include "ElfLoader.h"

const char app_name_string[APP_NAME_LEN + 1] = "ELF Loader";

const char LIBRARY_URI[] = "/a/Elf/elfloader.lib";

typedef struct
{ 
    APPLICATION_T           apd;
    UINT32                        reserve;
    void                         *Library;
} ELFLOADER_INSTANCE_DATA_T;

void  ElfLoaderStart(void);
static SYN_RETURN_STATUS_T Handle_LoadELF( EVENT_STACK_T *p_evg,  APPLICATION_T *p_apd );
static SYN_RETURN_STATUS_T Handle_UnloadELF( EVENT_STACK_T *p_evg,  APPLICATION_T *p_apd );

static SYN_RETURN_STATUS_T Start( EVENT_STACK_T *p_evg,  REG_ID_T reg_id,  APPLICATION_T *reg_hdl );
static SYN_RETURN_STATUS_T Exit( EVENT_STACK_T *p_evg,  APPLICATION_T *p_apd );

SYN_RETURN_STATUS_T LoadLibrary( ELFLOADER_INSTANCE_DATA_T *p_app_data );

typedef enum
{
    STATE_ANY,
    STATE_MAX
} STATE_T;

static const AFW_EVENT_CODE_T event_reg_table[] =
{
    EVCODE_STARTLDR
};

#define NUMBER_OF_STARTUP_EVENTS sizeof(event_reg_table)/sizeof(AFW_EVENT_CODE_T)

static const EVENT_HANDLER_ENTRY_T state_any_ev_table[] =
{  
    { EVCODE_LOADELF,           Handle_LoadELF            },
    { EVCODE_UNLOADELF,         Handle_UnloadELF          },
    { STATE_HANDLERS_END,         SYN_NULL  },
};

static const STATE_HANDLERS_ENTRY_T state_trans_table[] =
{
    { 
        STATE_ANY,
        SYN_NULL,
        SYN_NULL,
        state_any_ev_table
    }
};

static const APP_STATE_NAME_TABLE_T* const state_name_table = SYN_NULL;

#endif
