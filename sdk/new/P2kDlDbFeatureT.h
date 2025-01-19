/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DL_DB_FEATURE_T_H
#define P2K_SDK_DL_DB_FEATURE_T_H

#ifdef __cplusplus
extern "C" {
#endif

#define ID_KEYPAD_LOCK_79   0x86A
#define ID_KEYPAD_LOCK_49   0x7F3
#define ID_WORKING_TABLE_49 0x7F8

#define ID_IMEI             0x9CE
#if defined(EP1) || defined(EP2)
extern int *BEGIN_4A__IN_DB;
#else
#define BEGIN_4A__IN_DB         0x5C1  // null byte of seem 0004a_0001. u must add this value to offset from seem 004a_0001
#endif
#define CURRENT_RING_STYLE      BEGIN_4A__IN_DB + 0xA  // 0 - Громкий, 1 - Тихий, 2 - Вибро, 3 - Вибро и звонок, 4 -
	                                              // Бесшумный
#define ID_CLOCK                0x6FF  // 0 Dig, 1 an, 2 off
#define ID_DATE                 0x7C3  // 0 off, 1 on
#define ID_DATE_DISP            0x6FE  // 0 left, 1 cent
#define ID_MAINMENU_STYLE       0x6A0  // 0 icons, 1 list
#define ID_BKG_STYLE            0x678  // 0 cent, 1 mul, 2 ad
#define ID_SCREENSAVER          0x69C  // 0 off, ....
#define ID_PHOTO_RESOLUTION     0x6C5  // 0, 1, 2, 3
#define ID_PHOTO_QUALITY        0x75B  // 0, 1, 2
#define ID_VIDEO_LEN            0x692  // 0, 1, 2
#define ID_VIDEO_QUALITY        0x691  // 0, 1, 2
#define ID_JAVA_BKLT            0x71C  // 0 - выкл, 1 - вкл
#define ID_JAVA_VIBRATION       0x71E  // 0 - выкл, 1 - вкл
#define ID_JAVA_BKLT_DURATION   0x74D  // 0 - Unlimited, 1 - Limited (Не играет никакой роли)
#define ID_MISSED_CALLS_RINGER \
	0x85A  // Если 4, то оповещения о пропущенном нету. (возможно это просто стиль CURRENT_RING_STYLE)
#define ID_TIME_FORMAT                    0x5C2  // 0 - 12 часововй формат, 1 - 24 часовой формат
#define ID_IS_DATA_READY                  0x602  // 0, 1
#define ID_IS_BROWSER_READY               0x604  // 0, 1
#define ID_IS_MP3_READY                   0x6A5  // 0, 1    //0x5C1 + 0xE4
#define ID_AIRPLANE_MODE                  0x7C6  // 0 - выкл, 1 - вкл
#define ID_AIRPLANE_MODE_NOTICE           0x7C7  // 0 - выкл, 1 - вкл

// получить строку данных
#define ID_NOTEPAD_STRING                 0xA8F  // Блокнот. WCHAR notepad[42];
#define ID_SECURITY_CODE                  0xA96
#define ID_UNLOCK_CODE                    0xA95
#define ID_TECHNOLOGY                     0xAAF  // WCHAR technology[20];

// получить блок данных.
#define ID_DISP_BACKLIGHT                 0xB57  // UINT8 data[16]. data[7] = яркость дисплея!!!, data[3] = контрастность дисплея!!!
#define ID_RC_DIALED_CALLS_TIME           0xB6D  // Dialed Calls.   sizeof(CALL_TIME_T)
#define ID_RC_RECEIVED_CALLS_TIME         0xB6E  // Received Calls. sizeof(CALL_TIME_T)
#define ID_RC_LAST_CALL_TIME              0xB6F  // Last Call. sizeof(CALL_TIME_T)
#define ID_RC_ALL_CALLS_TIME              0xB70  // All Calls. sizeof(CALL_TIME_T)
#define ID_RC_LIFE_TIME                   0xB71  // Life Time. sizeof(CALL_TIME_T)
#define ID_ESN                            0xB50  // UINT8 data[4]
#define ID_FLEX                           0xB58  // UINT8 data[80]
#define ID_UNK                            0xB59  // UINT8 data[3]

#define ID_SW_UPDATE_FEATURE_AVAILABILITY 0x3D4
#define ID_AIRPLLNAME_MODE_AVAILABILITY   0x4CF

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DL_DB_FEATURE_T_H */

/** @} */ /* end of P2K_Types */
