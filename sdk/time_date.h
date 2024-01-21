// UTF-8 w/o BOM

#ifndef __SDK_TIME_DATE_H__
#define __SDK_TIME_DATE_H__

#include <typedefs.h>

typedef struct
{
	UINT8		day;
	UINT8		month;
	UINT16		year;
} CLK_DATE_T;

typedef struct
{
	UINT8		hour;
	UINT8		minute;
	UINT8		second;
} CLK_TIME_T;

typedef struct
{
	CLK_TIME_T	time;  
	CLK_DATE_T	date;  
	UINT32		unk;  
} DL_ALARM_DATA_T;

typedef struct
{
	UINT32		time;
	UINT32		ID;
} DL_TIMER_DATA_T;

#ifdef __cplusplus
extern "C" {
#endif

//Получить системное время в тиках 
UINT64 suPalReadTime( void ); 
// Перевести время из тиков в миллисекунды  
UINT64 suPalTicksToMsec( UINT64 ticks );

// Получить системную дату
BOOL DL_ClkGetDate( CLK_DATE_T * date);
// установить дату
BOOL DL_ClkSetDate( CLK_DATE_T date );
// Получить системное время 
BOOL DL_ClkGetTime( CLK_TIME_T * time);
// установить время
BOOL DL_ClkSetTime( CLK_TIME_T time );

typedef UINT32 CLK_CLOCK_T; // кол-во  секунд с  Jan. 1, 1970, GMT 

UINT8 DL_ClkGetClock( CLK_CLOCK_T * clock );
UINT8 DL_ClkSetClock( CLK_CLOCK_T clock );

// устанавливает время пробуждения
UINT8 DL_ClkSetWakeupEvent( CLK_CLOCK_T  wakeup_time );

/***************************************
   Функции Alarm
****************************************/

// Сработал Alarm
#define EV_ALARM_EXPIRED		0x82059 //  (CLK_ALARM_DATA_T*)EVENT_T::attachment

// установить Alarm
BOOL DL_ClkStoreIndividualEvent( IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data );

// удалить Alarm
BOOL DL_ClkDeleteIndividualEvent( IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data,
								  UINT8 mask ); // 1 - time, 2- date....  63 - полное удаление?


/***************************************
   Функции Timer 
****************************************/

/* Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED, attachment которого указывает на DL_TIMER_DATA_T */

/*  Запустить таймер, который сработает через time мс */
UINT32 APP_UtilStartTimer( UINT32 time,  UINT32 ID,  APPLICATION_T * app ); // ID будет в DL_TIMER_DATA_T при обработке ивента

/*  Запустить таймер, который будет срабаывать через каждые time мс */
UINT32 APP_UtilStartCyclicalTimer( UINT32 time,  UINT32 ID,  APPLICATION_T * app );

/* Остановить таймер */
UINT32 APP_UtilStopTimer( APPLICATION_T * app );



UINT32 DL_ClkStartTimer( IFACE_DATA_T* iface, UINT32 period, UINT32 id );
UINT32 DL_ClkStartCyclicalTimer( IFACE_DATA_T* iface, UINT32 period, UINT32 id );
UINT32 DL_ClkStopTimer( IFACE_DATA_T* iface );

typedef enum
{
    RESOURCE_UNAVAILABLE = 0,
    STORE_SUCCESS = 1,
    READ_SUCCESS = 2,
    DELETE_SUCCESS = 3,
    ALARM_ALREADY_EXPIRED = 4,
    PORT_NOT_FOUND = 6,
    TIMER_NOT_FOUND = 7,
    INVALID_DATE_FORMAT = 8,
    INVALID_TIME_FORMAT = 9
} CLK_STATUS_T;

typedef struct
{
    CLK_TIME_T time;
    CLK_DATE_T date;
    UINT8      weekday;
    UINT8       unk;
    UINT16     yearday;
    UINT16    timezone;
} CLK_PARSED_CLOCK_T;

CLK_STATUS_T DL_ClkGetParsedClock(CLK_PARSED_CLOCK_T *parsed_clock);
CLK_STATUS_T DL_ClkSetParsedClock(CLK_PARSED_CLOCK_T parsed_clock);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __SDK_TIME_DATE_H__
