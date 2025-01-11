/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_TIME_DATE_T_H
#define P2K_SDK_TIME_DATE_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT32 CLK_CLOCK_T;  // кол-во  секунд с  Jan. 1, 1970, GMT

typedef struct CLK_DATE_T CLK_DATE_T;
typedef struct CLK_TIME_T CLK_TIME_T;

typedef enum {
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

typedef struct CLK_PARSED_CLOCK_T CLK_PARSED_CLOCK_T;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_TIME_DATE_T_H */

/** @} */ /* end of P2K_Types */
