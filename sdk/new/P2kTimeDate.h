/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_TIME_DATE_H
#define P2K_SDK_TIME_DATE_H

#include "P2kTimeDateT.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CLK_DATE_T {
	UINT8  day;
	UINT8  month;
	UINT16 year;
};

struct CLK_TIME_T {
	UINT8 hour;
	UINT8 minute;
	UINT8 second;
};


// Получить системную дату
BOOL DL_ClkGetDate(CLK_DATE_T *date);
// установить дату
BOOL DL_ClkSetDate(CLK_DATE_T date);
// Получить системное время
BOOL DL_ClkGetTime(CLK_TIME_T *time);
// установить время
BOOL DL_ClkSetTime(CLK_TIME_T time);


UINT8 DL_ClkGetClock(CLK_CLOCK_T *clock);
UINT8 DL_ClkSetClock(CLK_CLOCK_T clock);

// устанавливает время пробуждения
UINT8 DL_ClkSetWakeupEvent(CLK_CLOCK_T wakeup_time);

struct CLK_PARSED_CLOCK_T {
	CLK_TIME_T time;
	CLK_DATE_T date;
	UINT8 weekday;
	UINT8 unk;
	UINT16 yearday;
	UINT16 timezone;
};

CLK_STATUS_T DL_ClkGetParsedClock(CLK_PARSED_CLOCK_T *parsed_clock);
CLK_STATUS_T DL_ClkSetParsedClock(CLK_PARSED_CLOCK_T parsed_clock);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_TIME_DATE_H */

/** @} */ /* end of P2K_Types */
