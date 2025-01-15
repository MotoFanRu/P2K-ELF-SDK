/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_KEYPAD_T_H
#define P2K_SDK_DEVICE_LAYER_KEYPAD_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

// Маски различных клавиш для обработки результата функции DL_KeyKjavaGetKeyState()
// Например, DL_KeyKjavaGetKeyState()&MULTIKEY_STAR проверяет, была ли нажата звёздочка */

#define MULTIKEY_0          (0x00000001)
#define MULTIKEY_1          (0x00000002)
#define MULTIKEY_2          (0x00000004)
#define MULTIKEY_3          (0x00000008)
#define MULTIKEY_4          (0x00000010)
#define MULTIKEY_5          (0x00000020)
#define MULTIKEY_6          (0x00000040)
#define MULTIKEY_7          (0x00000080)
#define MULTIKEY_8          (0x00000100)
#define MULTIKEY_9          (0x00000200)
#define MULTIKEY_STAR       (0x00000400)
#define MULTIKEY_POUND      (0x00000800)
#define MULTIKEY_SOFT_LEFT  (0x00001000)
#define MULTIKEY_SOFT_RIGHT (0x00002000)
#define MULTIKEY_MENU       (0x00004000)
#define MULTIKEY_UP         (0x00008000)
#define MULTIKEY_DOWN       (0x00010000)
#define MULTIKEY_LEFT       (0x00020000)
#define MULTIKEY_RIGHT      (0x00040000)
#define MULTIKEY_JOY_OK     (0x00080000)

enum {
	/* 0 */
	KEY_0 = 0,  // 0
	KEY_1,      // 1
	KEY_2,      // 2
	KEY_3,      // 3
	KEY_4,      // 4
	KEY_5,      // 5
	KEY_6,      // 6
	KEY_7,      // 7
	KEY_8,      // 8
	KEY_9,      // 9
	/* 10 */
	KEY_STAR,         // 10
	KEY_POUND,        // 11
	KEY_SCROLL_UP,    // 12
	KEY_SCROLL_DOWN,  // 13
	KEY_SOFT_LEFT,    // 14
	KEY_SOFT_RIGHT,   // 15
	KEY_SOFTKEY_3,    // 16
	KEY_RED,          // 17
	KEY_GREEN,        // 18
	KEY_POWER,        // 19
	/* 20 */
	KEY_MENU,         // 20
	KEY_SMART,        // 21
	KEY_VOLUME_ONE,   // 22
	KEY_VOL_UP,       // 23
	KEY_VOL_DOWN,     // 24
	KEY_VOL_UP_NS,    // 25
	KEY_VOL_DOWN_NS,  // 26
	KEY_MESSAGES,     // 27
	KEY_VOICEMAIL,    // 28
	KEY_PHONEBOOK,    // 29
	/* 30 */
	KEY_MENU_BACK,      // 30, 0x1E
	KEY_MUTE,           // 31
	KEY_CHANGE_BAND,    // 32
	KEY_VA_RECORD,      // 33
	KEY_SHOW_SERVICES,  // 34
	KEY_OPTION,         // 35
	KEY_FAST_ACCESS,    // 36
	KEY_CLEAR,          // 37, C 0x25
	KEY_INVALID,        // 38
	KEY_NOSCROLL_STAR,  // 39
	/* 40 */
	KEY_NOSCROLL_POUND,  // 40
	KEY_FORWARD,         // 41
	KEY_BACKWARD,        // 42
	KEY_VOICE,           // 43, 0x2B
	KEY_UP,              // 44
	KEY_DOWN,            // 45
	KEY_LEFT,            // 46
	KEY_RIGHT,           // 47
	KEY_MENU_FORWARD,    // 78, 0x30
	KEY_SPEAKER,         // 49
	/* 50 */
	KEY_HEADSET_SP,      // 50  headset with single position
	KEY_HEADSET_DP,      // 51  headset with double position // KEY_HANDSFREE 0x33
	KEY_WILD_DIGIT_0_9,  // 52  any digit, i.e. 0 thru 9
	KEY_WILD_DIGIT_1_9,  // 53  any digit, i.e. 1 thru 9 */
	KEY_FM_RADIO,        // 54
	KEY_WILD_ANY_CCITT,  // 55  any CCITT key, i.e. 0 thru 9, + #,* */
	KEY_GENERIC,         // 56
	KEY_LONG,            // 57
	KEY_CARKIT1,         // 58
	KEY_CARKIT2,         // 59
	/* 60 */
	KEY_CARKIT3,     // 60
	KEY_JOY_OK,      // 61  Joystick centre key for 5-way navigation. */
	KEY_ACC_POWER,   // 62
	KEY_CAM,         // 63  Dedicated Camera Key */ 0x3F
	KEY_PTT,         // 64  Push-To_talk Switch key */
	KEY_HIGH_AUDIO,  // 65
	KEY_CARRIER_1,   // 66  OLD KEY_ITUNES (WTF?) 0x42
	KEY_CARRIER_2,   // 67
	KEY_VIDEO_CALL,  // 68

	KEY_HEADSET = 0x46,     // 70
	KEY_POUND_STAR = 0x80,  // 128 locks the phone's keypad */

	KEY_MULTIKEYS_PRESSED = 0xFF
};
typedef UINT8 KEY_CODES_T;

enum {
	KEY_KEYPAD_ENABLED = 0,
	KEY_KEYPAD_DISABLED
};
typedef UINT8 KEY_KEYPAD_STATUS_T;

enum {
	KEY_PRESS = 0,
	KEY_RELEASE
};
typedef UINT8 KEY_STATE_T;

enum {
	KEY_VALID_REQUEST = 0,
	KEY_INVALID_DEVICE_ID,
	KEY_INVALID_KEY_CODE,
	KEY_INVALID_DEVICE_ID_AND_KEY_CODE
};
typedef UINT8 KEY_RESULT_CODE_T;

enum {
	KEY_HOOKSWITCH_ON = 0,
	KEY_HOOKSWITCH_OFF,
	KEY_HOOKSWITCH_UNKNOWN_STATE
};
typedef UINT8 KEY_HOOKSWITCH_STATE_T;

enum {
	KEY_PORTABLE_ID = 0,
	KEY_EXTERNAL_HANDSET_ID,
	KEY_IP_ID
};
typedef UINT8 KEY_DEVICE_IDS_T;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_KEYPAD_T_H */

/** @} */ /* end of P2K_Types */
