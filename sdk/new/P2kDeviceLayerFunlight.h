/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_FUNLIGHT_H
#define P2K_SDK_DEVICE_LAYER_FUNLIGHT_H

#include "P2kT.h"
#include "P2kDeviceLayerFunlightT.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Начать мигалку(funlight) под номером fl_id

     E398 ONLY! */
UINT32 DL_AudStartFunlight(UINT8 fl_id, UINT32 p2, UINT32 p3);  // p2 = 1, p3 = 0

/* Останавливает мигалку(funlight) под номером fl_id
    E398 ONLY! */
UINT32 DL_AudStopFunlight(UINT8 fl_id);

/* запрос управления/освобождение регионов
** prior - желаемый приоритет, 0 - низший, 3 - высший
** mask  - сумма масок регионов. указывать те, которыми хотим управлять,
**         остальные будут освобождены
** возвращает сумму масок тех регионов, над которыми получили управление
** пример: DL_LED_FL_set_control( 3, 0x03 ); // экран и клава
*/
UINT8 DL_LED_FL_set_control(UINT8 prior, UINT8 mask);

/* установить значение регионов
** prior - указываем тоже что и в DL_LED_FL_set_control
** count - количество пар регион/значение
** ...   - тут пишем парами номера регионов и их значения
** возвращает сумму масок тех регионов, которые обновились
** пример: DL_LED_FL_update( 3, 2, 1, 5, 2, 0 ); // экран на уровень 5, клаву выключить
*/
UINT8 DL_LED_FL_update(UINT8 prior, UINT8 count, ...);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_FUNLIGHT_H */

/** @} */ /* end of P2K_Types */
