/**
 * @file P2kUisTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_ELF_LOADER_EM2_T_H
#define P2K_SDK_ELF_LOADER_EM2_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef UINT32 EXTERN_LIB(void);

// Состояние флипа. 0 - флип закрыт, 1 - флип открыт (UINT8)
EXTERN_LIB FLIP_STATE;
#define flip_state *((UINT8 *) FLIP_STATE)

// оффсет по которому лежит IMEI
EXTERN_LIB SEEM_IMEI;
#define DB_FEATURE_IMEI (UINT16) SEEM_IMEI  // WCHAR IMEI[16];

// оффсет по которому лежит IMSI
EXTERN_LIB SEEM_IMSI;
#define DB_FEATURE_IMSI (UINT16) SEEM_IMSI  // WCHAR IMSI[16];

// состояние клавиатуры: 1 - заблок. 0 - разблок
EXTERN_LIB KEYPAD_STATE;
#define DB_FEATURE_KEYPAD_STATE (UINT16) KEYPAD_STATE


#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_ELF_LOADER_EM2_T_H */

/** @} */ /* end of P2K_Types */
