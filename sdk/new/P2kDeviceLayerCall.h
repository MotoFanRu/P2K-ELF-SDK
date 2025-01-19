/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_CALLS_H
#define P2K_SDK_DEVICE_LAYER_CALLS_H

#include "P2kT.h"
#include "P2kDeviceLayerT.h"
#include "P2kDeviceLayerCallT.h"

#ifdef __cplusplus
extern "C" {
#endif

// Таймеры разговора
typedef struct {
	UINT16 hours;
	UINT8 minutes;
	UINT8 seconds;
} CALL_TIME_T;

// время последнего звонка
#define C_TM S_TM - 3

/****************************
  Звонки
****************************/

#define MAX_CALLS 7

typedef struct {
	UINT16 call_id;
	UINT8 call_state;
} CALL_ID_T;

typedef struct {
	UINT8 number_of_calls;
	UINT8 overall_call_state;
	CALL_ID_T call_state_info[MAX_CALLS];
} CALL_STATES_T;

typedef struct {
	CALL_ID_T call;
	UINT8 unk;
	WCHAR number[69];
} CALL_ATT_T;

// если number_of_calls == 0, то вызово нет
void DL_SigCallGetCallStates(CALL_STATES_T *call_states);

// Ответ на входящий вызов
void DL_SigCallReleaseReq(IFACE_DATA_T *iface_data, UINT16 call_id, DL_SIG_CALL_CAUSE_T cause);

// TRUE - звовнок (входящий/исходящий)
BOOL APP_MMC_Util_IsVoiceCall(void);  // если FALSE, то вызовов нет

// TRUE - вывео звовнок (входящий/исходящий)
BOOL APP_MMC_Util_IsVideoCall(void);  // если FALSE, то вызовов нет

BOOL AlmclkReminderUtilCallInProgress(void);  // если FALSE, то никакого активного вызова нет

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_CALLS_H */

/** @} */ /* end of P2K_Types */
