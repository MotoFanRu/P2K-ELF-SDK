/**
 * @file P2kUisTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_LDR_T_H
#define P2K_SDK_LDR_T_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(EP1) || defined(EG1)
	#include "P2kLdrEp1T.h"
#elif defined(EP2)
	#include "P2kLdrEp2T.h"
#elif defined(EM1)
	#include "P2kLdrEm1T.h"
#elif defined(EM2)
	#include "P2kLdrEm2T.h"
#else
	#error "Unknow ElfPack/ElfLoader flavor. Please set 'EP1', 'EG1', 'EP2', 'EM1', or 'EM2' define."
#endif

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_LDR_T_H */

/** @} */ /* end of P2K_Types */
