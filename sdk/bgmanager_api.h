#ifndef BGMANAGER_API_H
#define BGMANAGER_API_H

#include <typedefs.h>
#include <events.h>


typedef enum
{
    BM_APP_REG,
    BM_APP_UNREG,
    BM_APP_SHOW,
    BM_APP_HIDE,
	BM_APP_ERROR_REG_FAIL
} BM_COMMANDS_T;

typedef struct
{
	BM_COMMANDS_T   command;
    char            app_name[13];
	BOOL    		appHide;
	UINT64          mid;

} BM_COMMAND_T;

#define BGMANAGER_EVENT_COMMAND	0xFF15

BM_COMMAND_T	att_data;
		
void BgManager_SendComand(BM_COMMANDS_T command, char *app_name, WCHAR *elf_uri, BOOL app_hide)
{
	UINT64			mid = 0;
	
	udbg("elf_uri=%s", elf_uri);
	if (elf_uri != NULL) 
	{
		DL_FsGetIDFromURI(elf_uri,  &mid);
		dbg("mid=0x%x%x", mid);
	}
	
	att_data.command = command;
	strncpy(att_data.app_name, app_name, 12);
	att_data.app_name[12] = 0;
	att_data.mid = mid;
	att_data.appHide = app_hide;
	AFW_CreateInternalQueuedEvAux(BGMANAGER_EVENT_COMMAND, FBF_LEAVE, sizeof(BM_COMMAND_T), &att_data); 
}

#define BgManager_RegApp(app_name, elf_uri, app_hide)   BgManager_SendComand(0, (char*)app_name, elf_uri, app_hide)
#define BgManager_UnregApp(app_name)   					BgManager_SendComand(1, (char*)app_name, NULL, false)
#define BgManager_ShowApp(app_name)   					BgManager_SendComand(2, (char*)app_name, NULL, false)
#define BgManager_HideApp(app_name)   					BgManager_SendComand(3, (char*)app_name, NULL, true)

#endif
