/**
 * @file P2kUisTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_UIS_H
#define P2K_SDK_UIS_H

#include "P2kT.h"
#include "P2kUisT.h"
#include "P2kEventT.h"

#ifdef __cplusplus
extern "C" {
#endif

UINT32 UIS_SetBacklight(UINT8 P1);

// Регулировка яркости дисплея
UINT32 UIS_SetBacklightWithIntensity(
	UINT8 color,     // = 255
	UINT8 intensity  // = 0...6
);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_UIS_H */

/** @} */ /* end of P2K_Types */
