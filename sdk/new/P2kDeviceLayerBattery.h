/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_BATTERY_H
#define P2K_SDK_DEVICE_LAYER_BATTERY_H

#include "P2kT.h"
#include "P2kDeviceLayerBatteryT.h"

#ifdef __cplusplus
extern "C" {
#endif

CHARGING_MODE DL_PwrGetChargingMode(void);

// показания батареи в процентах
 // бесполезная ф-ция
UINT8 DL_PwrGetActiveBatteryPercent(void);


//???
UINT8 DL_PwrGetActiveBattery(void);

// Низкий уровень зарядя батареи (1 - да, 0 - нет)
UINT8 DL_PwrGetLowBatteryStatus(void);

//???
UINT8 DL_PwrGetAttachedBattery(void);

//???
UINT8 DL_PwrGetMainBatteryType(void);

//???
UINT8 DL_PwrGetAuxBatteryType(void);

// true - подключено внешнее питание (зарядка, USB)
// подключено ли внешнее питание (1 - да, 0 - нет)
BOOL DL_AccIsExternalPowerActive(void);

// Тип зарядки???
UINT8 DL_AccGetChargerType(void);

// Вольтаж батареи. Значение в миливольтах
UINT32 RTIME_MAINT_get_battery_voltage(void);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_CALLS_H */

/** @} */ /* end of P2K_Types */
