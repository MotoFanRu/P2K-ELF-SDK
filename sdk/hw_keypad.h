// UTF-8 w/o BOM

#ifndef HW_KEYPAD_H
#define HW_KEYPAD_H

#include <typedefs.h>



#define KEYPAD_DATA_MASK		0xFC00

#define HAPI_KEYPAD_get_keycode() (((kpp.kpdr) | KEYPAD_DATA_MASK))
#define HAPI_KEYPAD_detect_key() ((kpp.kpsr & 0x1) ? TRUE : FALSE)

/* Keypad Port module register struct. */
typedef struct
{
	UINT16 kpcr;      /* Keypad Control Register */
	UINT16 kpsr;      /* Keypad Status Register */
	UINT16 kddr;      /* Keypad Data Direction Register */
	UINT16 kpdr;      /* Keypad Data Register */
} KPP_T;

extern volatile KPP_T kpp;

/*--------------------------------------------------------------------*/

enum 
{
	RTIME_MAINT_KEYCODE_NO_KEY = 0xFFFF, 
	RTIME_MAINT_KEYCODE_1      = 0xFE7F, // 1
	RTIME_MAINT_KEYCODE_2      = 0xFEF7, // 2
	RTIME_MAINT_KEYCODE_3      = 0xFD7F, // 3
	RTIME_MAINT_KEYCODE_4      = 0xFDF7, // 4
	RTIME_MAINT_KEYCODE_5      = 0xFF7E, // 5
	RTIME_MAINT_KEYCODE_6      = 0xFFF6, // 6
	RTIME_MAINT_KEYCODE_7      = 0xFFF3, // 7
	RTIME_MAINT_KEYCODE_8      = 0xFFF5, // 8
	RTIME_MAINT_KEYCODE_9      = 0xFFEB, // 9
	RTIME_MAINT_KEYCODE_10     = 0xFF7D, // *
	RTIME_MAINT_KEYCODE_11     = 0xFEBF, // 0
	RTIME_MAINT_KEYCODE_12     = 0xFDBF, // #
	RTIME_MAINT_KEYCODE_13     = 0xFEDF, // GREEN
	
	RTIME_MAINT_KEYCODE_15     = 0xFFBB, // Soft Right
	RTIME_MAINT_KEYCODE_16     = 0xFFBD, // Soft Left
	RTIME_MAINT_KEYCODE_17     = 0xFEEF,
	RTIME_MAINT_KEYCODE_18     = 0xFF7B, // Menu
	RTIME_MAINT_KEYCODE_19     = 0xFFED,
	RTIME_MAINT_KEYCODE_20     = 0xFDDF, // Vol Up
	RTIME_MAINT_KEYCODE_21     = 0xFFDE, // Vol Down
	RTIME_MAINT_KEYCODE_22     = 0xFFDD, // Smart
	RTIME_MAINT_KEYCODE_23     = 0xFFDB, // Camera
	RTIME_MAINT_KEYCODE_24     = 0xFFFF,
	RTIME_MAINT_KEYCODE_25     = 0xFFFF,
	RTIME_MAINT_KEYCODE_26     = 0xFFFF,
	RTIME_MAINT_KEYCODE_27     = 0xFFEE, // Nav Right
	RTIME_MAINT_KEYCODE_28     = 0xFDEF, // Nav Left
	RTIME_MAINT_KEYCODE_29     = 0xFFBE,
	
	RTIME_MAINT_KEYCODE_31     = 0xFF3F,
	RTIME_MAINT_KEYCODE_32     = 0xFF9F 
};
typedef UINT16 RTIME_MAINT_KEYCODE_T;                    


#define  RTM_KEY_0				RTIME_MAINT_KEYCODE_11
#define  RTM_KEY_1				RTIME_MAINT_KEYCODE_1
#define  RTM_KEY_2				RTIME_MAINT_KEYCODE_2
#define  RTM_KEY_3				RTIME_MAINT_KEYCODE_3
#define  RTM_KEY_4				RTIME_MAINT_KEYCODE_4
#define  RTM_KEY_5				RTIME_MAINT_KEYCODE_5
#define  RTM_KEY_6				RTIME_MAINT_KEYCODE_6
#define  RTM_KEY_7				RTIME_MAINT_KEYCODE_7
#define  RTM_KEY_8				RTIME_MAINT_KEYCODE_8
#define  RTM_KEY_9				RTIME_MAINT_KEYCODE_9
#define  RTM_KEY_STAR			RTIME_MAINT_KEYCODE_10
#define  RTM_KEY_POUND			RTIME_MAINT_KEYCODE_12

#define  RTM_KEY_SMART			RTIME_MAINT_KEYCODE_22
#define  RTM_KEY_CAM			RTIME_MAINT_KEYCODE_23

#define  RTM_KEY_UP				RTIME_MAINT_KEYCODE_24 // not test
#define  RTM_KEY_DOWN			RTIME_MAINT_KEYCODE_25 // not test
#define  RTM_KEY_LEFT			RTIME_MAINT_KEYCODE_28
#define  RTM_KEY_RIGHT			RTIME_MAINT_KEYCODE_27
#define  RTM_KEY_JOY_OK			RTIME_MAINT_KEYCODE_26 // not test

#define  RTM_KEY_GREEN			RTIME_MAINT_KEYCODE_13
//#define  RTM_KEY_RED			// красная на другом порте 

#define  RTM_KEY_SOFT_LEFT		RTIME_MAINT_KEYCODE_16
#define  RTM_KEY_SOFT_RIGHT		RTIME_MAINT_KEYCODE_17

#define  RTM_KEY_MENU			RTIME_MAINT_KEYCODE_18
#define  RTM_KEY_ITUNES			RTIME_MAINT_KEYCODE_22 // for E1 is other

#define  RTM_KEY_VOL_UP			RTIME_MAINT_KEYCODE_20
#define  RTM_KEY_VOL_DOWN		RTIME_MAINT_KEYCODE_21



#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // HW_KEYPAD_H
