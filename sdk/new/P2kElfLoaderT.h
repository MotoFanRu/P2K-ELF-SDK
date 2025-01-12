/**
 * @file P2kUisTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_ELF_LOADER_T_H
#define P2K_SDK_ELF_LOADER_T_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(EP1) || defined(EG1)
	#include "P2kElfLoaderEp1T.h"
#elif defined(EP2)
	#include "P2kElfLoaderEp2T.h"
#elif defined(EM1)
	#include "P2kElfLoaderEm1T.h"
#elif defined(EM2)
	#include "P2kElfLoaderEm2T.h"
#else
	#error "Unknow ElfPack/ElfLoader flavor. Please set 'EP1', 'EG1', 'EP2', 'EM1', or 'EM2' defines."
#endif

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_ELF_LOADER_T_H */

/** @} */ /* end of P2K_Types */
