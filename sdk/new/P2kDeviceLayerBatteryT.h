/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_BATTERY_T_H
#define P2K_SDK_DEVICE_LAYER_BATTERY_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

// Режим зарядки батареи
enum {
	CHARGING_MODE_NONE = 0,
	CHARGING_MODE_IN_PROGRESS = 1,
	CHARGING_MODE_CHARGING_COMPLETE
};

typedef UINT8 CHARGING_MODE;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_CALLS_T_H */

/** @} */ /* end of P2K_Types */
