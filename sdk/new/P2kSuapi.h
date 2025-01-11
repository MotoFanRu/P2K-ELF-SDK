/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_SUAPI_H
#define P2K_SDK_SUAPI_H

#include "P2kSuapiT.h"

#ifdef __cplusplus
extern "C" {
#endif


// Получить системное время в тиках
UINT64 suPalReadTime(void);
// Перевести время из тиков в миллисекунды
UINT64 suPalTicksToMsec(UINT64 ticks);


#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_SUAPI_H */

/** @} */ /* end of P2K_Types */
