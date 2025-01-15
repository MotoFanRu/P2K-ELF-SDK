/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_KEYPAD_H
#define P2K_SDK_DEVICE_LAYER_KEYPAD_H

#include "P2kT.h"
#include "P2kDeviceLayerKeypadT.h"

#ifdef __cplusplus
extern "C" {
#endif

KEY_KEYPAD_STATUS_T DL_KeyEnableDisableKeypad(BOOL keypad_state);

KEY_KEYPAD_STATUS_T DL_KeyQueryKeypadStatus(void);

UINT32 DL_KeyQueryKeypadActivity(void);

UINT8 DL_KeyGetNumberOfSoftkeys(void);

void DL_KeyInjectKeyPress(KEY_CODES_T keycode, KEY_STATE_T keystate, KEY_DEVICE_IDS_T device_id);  // device_id = 0
/*AFW_CreateInternalQueuedEvAux(0x2019, 0x65, 0, NULL); - Эмуляция нажатия левой софт клавиши*/

void DL_KeyUpdateKeypadBacklight(UINT8 backlight_setting);  // 1 - on, 0 - off

KEY_HOOKSWITCH_STATE_T DL_KeyQueryHookswitchState(void);

UINT32 DL_KeyKjavaGetKeyState(void);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_KEYPAD_H */

/** @} */ /* end of P2K_Types */
