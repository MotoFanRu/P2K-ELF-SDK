// UTF-8 w/o BOM
/********************************
  Будильники
********************************/

#ifndef __SDK_ALARM_H__
#define __SDK_ALARM_H__

#include <typedefs.h>
#include <time_date.h>


#define ALARM_RECORD_NAME_LEN 24
typedef struct {
	UINT8		index;
	WCHAR		name[ALARM_RECORD_NAME_LEN+1];
	CLK_TIME_T	time;
	UINT64		alert_tone;
	BOOL		is_alarm_enabled;
	UINT8		alarm_id;
	UINT8		ringer_volume;
	BOOL		is_wake_up_alarm;
} ALMCLK_RECORD_T;


#ifdef __cplusplus
extern "C" {
#endif

#define EV_ALMCLK_CHANGE    0x822BA // изменение будильников

// регистрируем приложение для того, чтобы при изменениии будильников приложению было послоно оповещение на обновление данных
//  оповещениепридёт на ивент EV_ALMCLK_CHANGE
UINT32 DL_DbAlmclkRegisterApplication(SU_PORT_T su_port);
UINT32 DL_DbAlmclkUnregisterApplication(SU_PORT_T su_port);

// получить запись
UINT32 DL_DbAlmclkGetRecordByIndex(UINT8 index, ALMCLK_RECORD_T * record);

// создать запись
UINT32 DL_DbAlmclkCreateRecord(ALMCLK_RECORD_T * record);

// удалить все будильники
UINT32 DL_DbAlmclkDeleteAllRecords(void);

// удалить запись
UINT32 DL_DbAlmclkDeleteRecordByIndex(UINT8 index);


#ifdef __cplusplus
}
#endif

#endif // __SDK_ALARM_H__
