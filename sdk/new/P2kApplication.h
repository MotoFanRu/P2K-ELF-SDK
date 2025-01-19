/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_APPLICATION_H
#define P2K_SDK_APPLICATION_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

UINT32 APP_UtilGetDataVolume(UINT16 feature_id, UINT64 *data_vol);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_APPLICATION_H */

/** @} */ /* end of P2K_Types */
