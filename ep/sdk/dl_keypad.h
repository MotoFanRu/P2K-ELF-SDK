// UTF-8 w/o BOM

#ifndef __SDK_KEYPAD_API_H__
#define __SDK_KEYPAD_API_H__ 

#include "typedefs.h"

enum
{
	/* 0 */
	KEY_0 = 0, 
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	/* 10 */
	KEY_STAR,
	KEY_POUND,
	KEY_SCROLL_UP,  
	KEY_SCROLL_DOWN,
	KEY_SOFT_LEFT,
	KEY_SOFT_RIGHT,
	KEY_SOFTKEY_3,
	KEY_RED,
	KEY_GREEN,
	KEY_POWER,
	/* 20 */
	KEY_MENU,
	KEY_SMART,
	KEY_VOLUME_ONE,
	KEY_VOL_UP,
	KEY_VOL_DOWN,
	KEY_VOL_UP_NS,
	KEY_VOL_DOWN_NS,
	KEY_MESSAGES,
	KEY_VOICEMAIL,
	KEY_PHONEBOOK,
	/* 30 */
	KEY_MENU_BACK, // 1E
	KEY_MUTE,
	KEY_CHANGE_BAND,
	KEY_VA_RECORD,
	KEY_SHOW_SERVICES,
	KEY_OPTION,
	KEY_FAST_ACCESS,
	KEY_CLEAR,		// C 0x25
	KEY_INVALID,
	KEY_NOSCROLL_STAR,
	/* 40 */
	KEY_NOSCROLL_POUND,
	KEY_FORWARD,
	KEY_BACKWARD,
	KEY_VOICE,	// 2B
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_MENU_FORWARD,	//30
	KEY_SPEAKER,
	/* 50 */
	KEY_HEADSET_SP,   /*headset with single position*/
	KEY_HEADSET_DP,   /*headset with double position*/
	KEY_WILD_DIGIT_0_9, /* any digit, i.e. 0 thru 9 */
	KEY_WILD_DIGIT_1_9, /* any digit, i.e. 1 thru 9 */
	KEY_FM_RADIO,
	KEY_WILD_ANY_CCITT, /* any CCITT key, i.e. 0 thru 9, + #,* */ 
	KEY_GENERIC, 
	KEY_LONG,
	KEY_CARKIT1,
	KEY_CARKIT2,
	/* 60 */
	KEY_CARKIT3,
	KEY_JOY_OK,  /* Joystick centre key for 5-way navigation. */
	KEY_ACC_POWER,
	KEY_CAM,          /* Dedicated Camera Key */
	KEY_PTT,             /* Push-To_talk Switch key */
	KEY_HIGH_AUDIO,
	KEY_CARRIER_1,
	KEY_CARRIER_2,
	KEY_VIDEO_CALL,

	KEY_POUND_STAR = 0x80,     /* locks the phone's keypad */

	KEY_MULTIKEYS_PRESSED = 0xFF
};

typedef UINT8 KEY_CODES_T;

/* Маски различных клавиш для обработки результата функции DL_KeyKjavaGetKeyState()
   Например, DL_KeyKjavaGetKeyState()&MULTIKEY_STAR проверяет, была ли нажата звёздочка */

#define MULTIKEY_0				0x00000001
#define MULTIKEY_1				0x00000002
#define MULTIKEY_2				0x00000004
#define MULTIKEY_3				0x00000008
#define MULTIKEY_4				0x00000010
#define MULTIKEY_5				0x00000020
#define MULTIKEY_6				0x00000040
#define MULTIKEY_7				0x00000080
#define MULTIKEY_8				0x00000100
#define MULTIKEY_9				0x00000200
#define MULTIKEY_STAR			0x00000400
#define MULTIKEY_POUND			0x00000800
#define MULTIKEY_SOFT_LEFT		0x00001000
#define MULTIKEY_SOFT_RIGHT		0x00002000
#define MULTIKEY_MENU			0x00004000
#define MULTIKEY_UP				0x00008000
#define MULTIKEY_DOWN			0x00010000
#define MULTIKEY_LEFT			0x00020000 
#define MULTIKEY_RIGHT			0x00040000
#define MULTIKEY_JOY_OK			0x00080000



enum
{
	KEY_KEYPAD_ENABLED = 0,
	KEY_KEYPAD_DISABLED
};
typedef UINT8 KEY_KEYPAD_STATUS_T;

enum
{
	KEY_PRESS = 0,
	KEY_RELEASE
};
typedef UINT8 KEY_STATE_T;

enum
{
	KEY_VALID_REQUEST = 0,
	KEY_INVALID_DEVICE_ID,
	KEY_INVALID_KEY_CODE,
	KEY_INVALID_DEVICE_ID_AND_KEY_CODE
};
typedef UINT8 KEY_RESULT_CODE_T;

enum
{
	KEY_HOOKSWITCH_ON = 0,
	KEY_HOOKSWITCH_OFF,
	KEY_HOOKSWITCH_UNKNOWN_STATE
};
typedef UINT8 KEY_HOOKSWITCH_STATE_T;

enum
{
	KEY_PORTABLE_ID = 0,
	KEY_EXTERNAL_HANDSET_ID,
	KEY_IP_ID
};
typedef UINT8 KEY_DEVICE_IDS_T;


#ifdef __cplusplus 
extern "C" {
#endif


KEY_KEYPAD_STATUS_T DL_KeyEnableDisableKeypad( BOOL keypad_state );

KEY_KEYPAD_STATUS_T DL_KeyQueryKeypadStatus( void );

UINT32 DL_KeyQueryKeypadActivity( void );


void DL_KeyInjectKeyPress(	KEY_CODES_T keycode,
							KEY_STATE_T keystate,
							KEY_DEVICE_IDS_T device_id);	// 0


void DL_KeyUpdateKeypadBacklight(UINT8 backlight_setting);


KEY_HOOKSWITCH_STATE_T DL_KeyQueryHookswitchState( void );

UINT32 DL_KeyKjavaGetKeyState ( void );

/*================================================================================================*/

#ifdef __cplusplus
}
#endif 

#endif // __SDK_KEYPAD_API_H__
