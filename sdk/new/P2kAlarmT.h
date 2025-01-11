/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_ALARM_T_H
#define P2K_SDK_ALARM_T_H

//#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EV_ALMCLK_CHANGE 0x822BA  // изменение будильников
#define EV_ALARM_EXPIRED 0x82059  //  (CLK_ALARM_DATA_T*)EVENT_T::attachment

#define ALARM_RECORD_NAME_LEN 24

typedef struct DL_ALARM_DATA_T DL_ALARM_DATA_T;
typedef struct ALMCLK_RECORD_T ALMCLK_RECORD_T;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_ALARM_T_H */

/** @} */ /* end of P2K_Types */
