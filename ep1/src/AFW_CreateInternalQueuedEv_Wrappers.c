/*
 * Project:
 *   ElfLoader / ElfPack for Motorola P2K platform, ver 1 (EP1).
 *
 * About:
 *   AFW_CreateInternalQueuedEvAux() and AFW_CreateInternalQueuedEvAuxD() functions wrapper for old Motorola P2K phones.
 *
 * Author:
 *   tim_apple, 11-Feb-2009
 */

#include <apps.h>

#define AFW_SEQN_GENERIC               (-1)
#define AFW_ROUTE_ID_GENERIC           (36)
#define AFW_FRAMEWORK_EV_CATG           (0)
#define AFW_LOG_ID_GENERIC              (0)
#define AFW_NON_SVC_REQ                 (0)

UINT32 AFW_CreateInternalQueuedEvAux(UINT32 ev, FREE_BUF_FLAG_T flag, UINT32 size, void *data) {
	return AFW_CreateInternalQueuedEvPriv(
		ev,
		AFW_SEQN_GENERIC,
		AFW_ROUTE_ID_GENERIC,
		AFW_FRAMEWORK_EV_CATG,
		AFW_LOG_ID_GENERIC,
		NULL,
		flag,
		size,
		data,
		AFW_NON_SVC_REQ
	);
}

UINT32 AFW_CreateInternalQueuedEvAuxD(UINT32 ev, ADD_EVENT_DATA_T *evd, FREE_BUF_FLAG_T flag, UINT32 size, void *data) {
	return AFW_CreateInternalQueuedEvPriv(
		ev,
		AFW_SEQN_GENERIC,
		AFW_ROUTE_ID_GENERIC,
		AFW_FRAMEWORK_EV_CATG,
		AFW_LOG_ID_GENERIC,
		evd,
		flag,
		size,
		data,
		AFW_NON_SVC_REQ
	);
}
