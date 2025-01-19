/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DL_T_H
#define P2K_SDK_DL_T_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

// Как преобразовать сим, оффсет, бит в feature_id?
// Пример 1: seem_element_id=0x0032, seem_record=0x0001, seem_offset=0x80 bit=0x6.
// feature_id = seem_offset * 0x8 + bit - seem_element_id = 0x80 * 0x8 + 0x6 - 0x0032 = 3D4
// подходит только для сима 0032_0001!!!!!!!!!!!!!!
// Пример 2: seem_element_id=0x0032, seem_record=0x0001, seem_offset=0x80 bit=0x6.
// feature_id = 0x34E + seem_offset + bit = 0x34E + 0x80 + 6 = 3D4
// UINT16 ConvertSeem0032toID(UINT16 offset, UINT16 bit)
//{
//     UINT16 res = offset * 0x8 + ((bit > 7) ? (7) : (bit));
//     return (res > 0xFF) ? (res - 0x0032) : res;
// }
#define SEEM32_TO_ID(offset, bit)  (offset * 0x8 + bit)
#define SEEM32_TO_ID_1(s, r, o, b) (o * 0x8 + b - s)
#define SEEM32_TO_ID_2(o, b)       (0x34E + o + b)

typedef struct IFACE_DATA_T IFACE_DATA_T;

enum { // volume_type {
	BASE = 0,  // громкость звонка

	PHONE,       // громкость клавиатуры
	VOICE,       // громкость мультимедия  LTE
	MICROPHONE,  // громкость разговора
	MULTIMEDIA,  //  громкость мультимедия LTE2 и V3i
	PTT_TONES,
	MUTABLE_MAX,
	IMMUTABLE_MAX,
	MAX = IMMUTABLE_MAX
};
typedef UINT8 VOLUME_T;


#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DL_T_H */

/** @} */ /* end of P2K_Types */
