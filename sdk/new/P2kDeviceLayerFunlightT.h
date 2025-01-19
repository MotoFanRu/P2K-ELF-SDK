/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DL_FUNLIGHT_T_H
#define P2K_SDK_DL_FUNLIGHT_T_H

//#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FL_MAX_REGIONS      (8)

#define FL_REGION1_MASK     (0x01)
#define FL_REGION2_MASK     (0x02)
#define FL_REGION3_MASK     (0x04)
#define FL_REGION4_MASK     (0x08)
#define FL_REGION5_MASK     (0x10)
#define FL_REGION6_MASK     (0x20)
#define FL_REGION7_MASK     (0x40)
#define FL_REGION8_MASK     (0x80)
#define FL_ALL_REGIONS_MASK (0xff)

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DL_FUNLIGHT_T_H */

/** @} */ /* end of P2K_Types */
