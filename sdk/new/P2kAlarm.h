/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_ALARM_H
#define P2K_SDK_ALARM_H

#include "P2kAlarmT.h"
#include "P2kTimeDateT.h"
#include "P2kDlT.h"
#include "P2kSuapiT.h"

#include "P2kTimeDate.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DL_ALARM_DATA_T {
	CLK_TIME_T time;
	CLK_DATE_T date;
	UINT32 unk;
};

struct ALMCLK_RECORD_T {
	UINT8 index;
#if defined(EM1) || defined(EM2) || defined(EA1)
	UINT8 unk;
#endif
	WCHAR name[ALARM_RECORD_NAME_LEN + 1];
	CLK_TIME_T time;
	UINT64 alert_tone;
	BOOL is_alarm_enabled;
	UINT8 alarm_id;
	UINT8 ringer_volume;
	BOOL is_wake_up_alarm;
};


// создать запись
UINT32 DL_DbAlmclkCreateRecord(ALMCLK_RECORD_T *record);

// Изменить запись
UINT32 DL_DbAlmclkModifyRecord(ALMCLK_RECORD_T *record);

// получить кол-во записей
UINT32 DL_DbAlmclkGetNumberOfRecords(UINT8 unk, UINT8 *count);  // unk = 0

// получить запись по её индексу
UINT32 DL_DbAlmclkGetRecordByIndex(UINT8 index, ALMCLK_RECORD_T *record);

// удалить запись
UINT32 DL_DbAlmclkDeleteRecordByIndex(UINT8 index);

// удалить все будильники
UINT32 DL_DbAlmclkDeleteAllRecords(void);

// установить Alarm
BOOL DL_ClkStoreIndividualEvent(IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data);

// удалить Alarm
BOOL DL_ClkDeleteIndividualEvent(
	IFACE_DATA_T *data,
	DL_ALARM_DATA_T alarm_data,
	UINT8 mask
);  // 1 - time, 2- date....  63 - полное удаление?

// регистрируем приложение для того, чтобы при изменениии будильников приложению было послоно оповещение на обновление
// данных
//  оповещениепридёт на ивент EV_ALMCLK_CHANGE
UINT32 DL_DbAlmclkRegisterApplication(SU_PORT_T su_port);
UINT32 DL_DbAlmclkUnregisterApplication(SU_PORT_T su_port);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_ALARM_H */

/** @} */ /* end of P2K_Types */
