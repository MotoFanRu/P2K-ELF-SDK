#include <apps.h>
#include <loader.h>
#include <utilities.h>

#if defined(__arm)  /* ADS */
#define     GET_DATA_VALUE_FROM_LIB(x) (x)
#elif defined(__GNUC__) /* GCC */
#define     GET_DATA_VALUE_FROM_LIB(x) (&x)
#else
#error "Unknown compiler flavor!"
#endif

typedef enum {
	APP_STATE_ANY,
	APP_STATE_MAX
} APP_STATE_T;

static UINT32 ApplicationStart(EVENT_STACK_T *ev_st, REG_ID_T reg_id, void *reg_hdl);

static const EVENT_HANDLER_ENTRY_T g_state_any_hdls[] = {
	{ STATE_HANDLERS_END, NULL }
};

static const STATE_HANDLERS_ENTRY_T g_state_table_hdls[] = {
	{ APP_STATE_ANY, NULL, NULL, g_state_any_hdls },
};

UINT32 Register(const char *elf_path_uri, const char *args, UINT32 ev_code) {
	UINT32 status;
	UINT32 ev_code_base;

	ev_code_base = ev_code;

	status = APP_Register(&ev_code_base, 1, g_state_table_hdls, APP_STATE_MAX, (void *) ApplicationStart);

	LOG("Register ELF entry point: '%s', args: '%s', ev_code: '%d'.\n", elf_path_uri, args, ev_code);

	LdrStartApp(ev_code_base);

	return status;
}

static UINT32 ApplicationStart(EVENT_STACK_T *ev_st, REG_ID_T reg_id, void *reg_hdl) {
	UINT32 status;

	status = RESULT_OK;

	LOG("Hello Moto! %d\n", status);

	LOG("Lib: 0x%X 0x%X\n", Lib, &Lib);
	LOG("display_source_buffer: 0x%X\n", GET_DATA_VALUE_FROM_LIB(display_source_buffer));
	LdrUnloadELF(&Lib);

	return status;
}
