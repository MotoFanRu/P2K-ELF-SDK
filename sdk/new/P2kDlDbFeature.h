/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DL_DB_FEATURE_H
#define P2K_SDK_DL_DB_FEATURE_H

#include "P2kT.h"
#include "P2kDlDbFeatureT.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************
   DbFeature
******************************/
UINT8 DL_DbFeatureGetCurrentState(UINT16 fstate_id, UINT8 *state);  // Чтение из сима 004a
UINT8 DL_DbFeatureStoreState(UINT16 fstate_id, UINT8 state);        // Запись в сим 004а
UINT8 DL_DbFeatureStoreBlock(UINT16 id, void *data, UINT8 size);  // Сохранить блок данных data размером size в id
UINT8 DL_DbFeatureGetValue(UINT16 feature_id, UINT32 *feature_value);
UINT8 DL_DbFeatureGetValueString(UINT16 feature_id, WCHAR *feature_str);

UINT8 DL_DbFeatureStoreValueString(UINT16 feature_id, WCHAR *feature_str, UINT16 str_len);
UINT8 DL_DbFeatureGetBlock(UINT16 id, void *data);                // Прочитать блок данных id в data

UINT8 DL_DbGetFeatureAvailability(UINT16 feature_id);
// Доступность ф-ции (1 - да, 0 - нет)

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DL_DB_FEATURE_H */

/** @} */ /* end of P2K_Types */
