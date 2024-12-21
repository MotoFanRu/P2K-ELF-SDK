
#include "ElfLoaderApp.h"

static SYN_RETURN_STATUS_T Handle_LoadELF( EVENT_STACK_T *p_evg,  APPLICATION_T *p_apd )
{
    EVENT_T             *p_event = AFW_GetEv(p_evg);
    ELFLOADER_INSTANCE_DATA_T   *p_app_data = (ELFLOADER_INSTANCE_DATA_T*) p_apd;
    UINT32                  status;
    //char logbuf[68];
    UtilLogStringData(" *** ELFLOADER *** LoadELF  current reserve = 0x%X", p_app_data->reserve);

    status = loadELF( p_event->data.start_params.uri,  p_event->data.start_params.params,  p_app_data->Library, p_app_data->reserve );
    
    p_app_data->reserve += EVCODE_RESERVE;

    UtilLogStringData(" *** ELFLOADER *** LoadELF status = %d", status);
    
    APP_ConsumeEv(p_evg, p_apd);

    if(status > ELDR_SUCCESS) return SYN_FAIL;
    return SYN_SUCCESS;
}

static SYN_RETURN_STATUS_T Handle_UnloadELF( EVENT_STACK_T *p_evg,  APPLICATION_T *p_apd )
{
    EVENT_T             *p_event = AFW_GetEv(p_evg);
    UtilLogStringData(" *** ELFLOADER *** UnloadELF");

    //AFW_FreeAllocatedMemory(*((void**)p_event->ev_data.AFW_UNION_DATA_T.afw_data_union.AFW_EV_DATA_U.generic_short_data)); //ptr
    AFW_FreeAllocatedMemory(*((void**)(p_event->data.pad))); //ptr
    
    APP_ConsumeEv(p_evg, p_apd);

    return SYN_SUCCESS;
}

static SYN_RETURN_STATUS_T
Start (EVENT_STACK_T     *p_evg,
       REG_ID_T  reg_id,
       APPLICATION_T                  *reg_hdl)
{
    APP_ID_T          app_id;
    SYN_RETURN_STATUS_T            app_status = SYN_SUCCESS;
    ELFLOADER_INSTANCE_DATA_T      *p_app_data = SYN_NULL;
   
    /* Check that application was not already started */
    if( AFW_InquireRoutingStackByRegId(reg_id) == SYN_SUCCESS )
    {
        app_status = SYN_FAIL;
    }
    else
    {
          /*
       * Allocate the Idle app's instance data, initialize it, request a user
       * interaction token, and add the app to the routing stack.
       *//*
         p_app_data = (IDLE_INSTANCE_DATA_T *)APP_InitAppData(
          (void *)APP_HandleEvent, sizeof(IDLE_INSTANCE_DATA_T), reg_id, 0,
          IDLE_MAX_STATE_LEVELS, USERINTER_TOKEN_PRIORITY,
          AFW_APP_CENTRICITY_SECONDARY, AFW_FOCUS, AFW_POSITION_BOTTOM);
          */
        app_id = AFW_GenAppInstanceId();
      
        /* Initialize application instance data */
        p_app_data = (ELFLOADER_INSTANCE_DATA_T*) APP_GetInstData ( sizeof(ELFLOADER_INSTANCE_DATA_T),
                                                                    STATE_ANY,
                                                                    reg_id,
                                                                    app_id,
                                                                    state_trans_table );

        app_status = (p_app_data != SYN_NULL ? SYN_SUCCESS : SYN_FAIL);
      
        /* If the instance data was allocated successfully, continue */
        if (app_status == SYN_SUCCESS)
        {
            p_app_data->apd.exit_fn = Exit;
        
            /* 
             * Store the Battery_main application information within the routing stack
             * internal to the Application Manager, to subscribe the Battery_main application
             * to receive events.
             */
             
            p_app_data->apd.app_name = app_name_string;
            p_app_data->apd.state_names = SYN_NULL;

            p_app_data->reserve = EVCODE_BASE + EVCODE_RESERVE;
            
            LoadLibrary(p_app_data);

            app_status = AFW_AddAppToRoutingStack( (void *)APP_HandleEventPrepost,
                                                   1,
                                                   AFW_PREPROCESSING,
                                                   AFW_POSITION_TOP,
                                                   (UINT32)p_app_data,
                                                   app_id,
                                                   reg_id );

           
            
            UtilLogStringData(" *** ELFLOADER ***  Start  %d", app_status);
           /*if (app_status == SYN_SUCCESS )
            {         
               
                app_status = APP_Start (p_evg, &(p_app_data->apd),
                                        STATE_ANY,        
                                        state_trans_table,
                                        Exit,               
                                        app_name_string,       
                                        SYN_NULL);                 
            }  
        */
        } /* If the instance data was allocated successfully */
    } /* If the application is not already running AND the event is a startup event */
   
    return app_status;
}

static SYN_RETURN_STATUS_T Exit( EVENT_STACK_T *p_evg,  APPLICATION_T *p_apd )
{
    SYN_RETURN_STATUS_T       app_status = SYN_SUCCESS;

    app_status = APP_Exit( p_evg, p_apd, SYN_NULL );
    UtilLogStringData(" *** ELFLOADER *** Exit");

    return app_status;
}

SYN_RETURN_STATUS_T LoadLibrary( ELFLOADER_INSTANCE_DATA_T *p_app_data )
{
    DL_FS_HANDLE_T      f;
    DL_FS_COUNT_T       read;
    UINT32              size;
    W_CHAR              uri[64];
    
    u_atou((char*)LIBRARY_URI, uri);
    
    f = DL_FsOpenFile(uri, DL_FS_READ_MODE, 0);
    
    size = DL_FsGetFileSize(f);
    
    p_app_data->Library = AFW_AllocateMemory(size);
    DL_FsReadFile(p_app_data->Library, size, 1, f, &read);
    
    DL_FsCloseFile(f);
    
    return SYN_SUCCESS;
}

void  ElfLoaderStart(void)
{
    SYN_RETURN_STATUS_T       status;
    status = APP_Register( event_reg_table, 
                  NUMBER_OF_STARTUP_EVENTS,
                  state_trans_table, 
                  STATE_MAX, 
                  (void *)Start );
                  
    UtilLogStringData(" *** ELFLOADER *** Register %d", status);
                   
    LdrStartApp( EVCODE_STARTLDR );
    //AFW_AddEvNoD(p_evg, EVCODE_STARTLDR);
}
