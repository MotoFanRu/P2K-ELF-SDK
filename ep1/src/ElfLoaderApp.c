/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   P2K wrapper for the ELF loader application.
 *
 * Author:
 *   Andy51, 26-Aug-2007
 */

#include "ElfLoader.h"
#include "ElfLoaderApp.h"

#include <apps.h>
#include <loader1.h>
#include <utilities.h>

static UINT32 Handle_LoadELF(EVENT_STACK_T *p_evg, APPLICATION_T *p_apd);
static UINT32 Handle_UnloadELF(EVENT_STACK_T *p_evg, APPLICATION_T *p_apd);

static UINT32 Start(EVENT_STACK_T *p_evg, REG_ID_T reg_id, void *reg_hdl);
static UINT32 Exit(EVENT_STACK_T *p_evg, APPLICATION_T *p_apd);

const char app_name_string[APP_NAME_LEN + 1] = "ELF Loader";

const char LIBRARY_URI[] = "file://b/Elf/elfloader.lib";

static const AFW_EVENT_CODE_T event_reg_table[] = { EVCODE_STARTLDR };

#define NUMBER_OF_STARTUP_EVENTS (sizeof(event_reg_table) / sizeof(AFW_EVENT_CODE_T))

static const EVENT_HANDLER_ENTRY_T state_any_ev_table[] = {
	{ EVCODE_LOADELF, Handle_LoadELF },
	{ EVCODE_UNLOADELF, Handle_UnloadELF },
	{ STATE_HANDLERS_END, NULL }
};

static const STATE_HANDLERS_ENTRY_T state_trans_table[] = {
	{ STATE_ANY, NULL, NULL, state_any_ev_table }
};

static UINT32 Handle_LoadELF(EVENT_STACK_T *p_evg, APPLICATION_T *p_apd) {
	UINT32 status;
	EVENT_T *p_event;
	ELFLOADER_INSTANCE_DATA_T *p_app_data;

	p_event = AFW_GetEv(p_evg);
	p_app_data = (ELFLOADER_INSTANCE_DATA_T *) p_apd;

	UtilLogStringData(" *** ELFLOADER *** LoadELF  current reserve = 0x%X", p_app_data->reserve);

	status = loadELF(
		p_event->data.start_params.uri,
		p_event->data.start_params.params,
		p_app_data->Library,
		p_app_data->reserve
	);

	p_app_data->reserve += EVCODE_RESERVE;

	UtilLogStringData(" *** ELFLOADER *** LoadELF status = %d", status);

	APP_ConsumeEv(p_evg, (APPLICATION_T *) p_apd);

	if (status > ELDR_SUCCESS) {
		return RESULT_FAIL;
	}

	return RESULT_OK;
}

static UINT32 Handle_UnloadELF(EVENT_STACK_T *p_evg, APPLICATION_T *p_apd) {
	EVENT_T *p_event;

	p_event = AFW_GetEv(p_evg);

	UtilLogStringData(" *** ELFLOADER *** UnloadELF");

	// EXL, 22-Dec-2024: A pointer to ELF structures is sent along with the event data.
	suFreeMem(*((void **) p_event->data.pad));

	APP_ConsumeEv(p_evg, (APPLICATION_T *) p_apd);

	return RESULT_OK;
}

// EXL, 22-Dec-2024: This function is called by AFW on start/run/launch event after registration of application.
static UINT32 Start(EVENT_STACK_T *p_evg, REG_ID_T reg_id, void *reg_hdl) {
	UINT32 status;
	APP_ID_T app_id;
	ELFLOADER_INSTANCE_DATA_T *p_app_data;

	status = RESULT_OK;
	p_app_data = NULL;

	// Motorola: Check that application was not already started.
	if (AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK) {
		status = RESULT_FAIL;
	} else {
		/*
		 * Motorola:
		 *   Allocate the Idle app's instance data, initialize it, request a user
		 *   interaction token, and add the app to the routing stack.
		 */
		app_id = AFW_GenAppInstanceId();

		// Motorola:  Initialize application instance data.
		// EXL, 22-Dec-2024: Since ELF Loader application is a firmware patch, get its data structures instead of init.
		p_app_data = (ELFLOADER_INSTANCE_DATA_T*) APP_GetInstData(
			sizeof(ELFLOADER_INSTANCE_DATA_T),
			STATE_ANY,
			reg_id,
			app_id,
			state_trans_table
		);

		status = (p_app_data != NULL) ? RESULT_OK : RESULT_FAIL;

		// Motorola: If the instance data was allocated or retrieved successfully, continue.
		if (status == RESULT_OK) {
			// EXL, 22-Dec-2024: Register exit function for app.
			p_app_data->apd.exit_fn = Exit;

			/*
			 * Motorola:
			 *   Store the application information within the routing stack
			 *   internal to the Application Manager, to subscribe the application
			 *   to receive events.
			 */
			p_app_data->apd.app_name = (const char *) app_name_string;
			p_app_data->apd.state_names = NULL;

			p_app_data->reserve = EVCODE_BASE + EVCODE_RESERVE;

			// EXL, 22-Dec-2024: Load EP1 library!
			LoadLibrary(p_app_data);

			// EXL, 22-Dec-2024: APP_HandleEventPrepost indicates that application is background with no GUI.
			status = AFW_AddAppToRoutingStack(
				(void *) APP_HandleEventPrepost,
				1,
				AFW_PREPROCESSING,
				AFW_POSITION_TOP,
				(UINT32) p_app_data,
				app_id,
				reg_id
			);

			UtilLogStringData(" *** ELFLOADER ***  Start  %d", status);
		} // Motorola: If the instance data was allocated successfully.
	} // Motorola: If the application is not already running AND the event is a startup event.

	return status;
}

static UINT32 Exit(EVENT_STACK_T *p_evg, APPLICATION_T *p_apd) {
	UINT32 status;

	status = RESULT_OK;

	status = APP_Exit(p_evg, p_apd, NULL);

	UtilLogStringData(" *** ELFLOADER *** Exit");

	return status;
}

UINT32 LoadLibrary(ELFLOADER_INSTANCE_DATA_T *p_app_data) {
	UINT32 size;
	FS_COUNT_T read;
	FS_HANDLE_T file;
	W_CHAR uri[WCHAR_PARAMS_MAX];

	u_atou((const char *) LIBRARY_URI, (WCHAR *) uri);

	file = DL_FsOpenFile((const WCHAR *) uri, FILE_READ_MODE, 0);

	size = DL_FsGetFileSize(file);

	p_app_data->Library = suAllocMem(size, NULL);
	DL_FsReadFile(p_app_data->Library, size, 1, file, &read);

	DL_FsCloseFile(file);

	return RESULT_OK;
}

// EXL, 22-Dec-2024: This function calls from AutoRun.c:AutorunMain and register ELF Loader application in AFW.
void ElfLoaderStart(void) {
	UINT32 status;

	status = APP_Register(event_reg_table, NUMBER_OF_STARTUP_EVENTS, state_trans_table, STATE_MAX, (void *) Start);

	UtilLogStringData(" *** ELFLOADER *** Register %d", status);

	LdrStartApp(EVCODE_STARTLDR);

	//	AFW_AddEvNoD(p_evg, EVCODE_STARTLDR);
}
