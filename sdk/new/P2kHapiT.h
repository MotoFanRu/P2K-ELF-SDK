/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_HAPI_T_H
#define P2K_SDK_HAPI_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************
   Аккумулятор
*******************************/

#define HAPI_BATTERY_ROM_BYTE_SIZE      128
#define HAPI_BATTERY_ROM_UNIQUE_ID_SIZE 6

enum {
	HAPI_BATTERY_ROM_NONE = 0,
	HAPI_BATTERY_ROM_W_DATA,
	HAPI_BATTERY_ROM_WO_DATA,
	HAPI_BATTERY_ROM_INVALID
};
typedef UINT8 HAPI_BATTERY_ROM_T;

typedef struct HAPI_CAP_ACCESS HAPI_CAP_ACCESS;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_HAPI_T_H */

/** @} */ /* end of P2K_Types */
