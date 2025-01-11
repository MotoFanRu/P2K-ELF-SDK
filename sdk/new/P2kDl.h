/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DL_H
#define P2K_SDK_DL_H

#include "P2kT.h"
#include "P2kDlT.h"
#include "P2kSuapiT.h"

#ifdef __cplusplus
extern "C" {
#endif

struct IFACE_DATA_T {
	SU_PORT_T port;
	UINT32 handle;
};

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DL_H */

/** @} */ /* end of P2K_Types */
