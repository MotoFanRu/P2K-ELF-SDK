/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_HAPI_H
#define P2K_SDK_HAPI_H

#include "P2kT.h"
#include "P2kHapiT.h"

#ifdef __cplusplus
extern "C" {
#endif

HAPI_BATTERY_ROM_T HAPI_BATTERY_ROM_read(UINT8 *dest_address);
void HAPI_BATTERY_ROM_get_unique_id(UINT8 *unique_id);

extern const UINT32 *SBCM_ATOD_vltg;
extern const UINT32 *SBCM_ATOD_supply;

/******************************
   Свет
*******************************/

/* Регулировка яркости дисплея
    E398 ONLY! */
void HAPI_LP393X_disp_backlight_intensity(UINT32 light);  // light = 0..100

/* Боковые светодиоды и фонарик
    E398 ONLY! */
void HAPI_LP393X_set_tri_color_led(
	UINT32 type,  //  0-боковые, 1-фонарик;
	UINT32 val    //  0xRGB (0x000 - 0xFFF), например 0x00F - синий
);

struct HAPI_CAP_ACCESS {
	unsigned int *addr;
	unsigned int data;
	unsigned int mask;
	UINT8 rw;
	UINT8 reg;
};

// клавиатура
void HAPI_PCAP_transceive(HAPI_CAP_ACCESS *);

//  Сенсор освещения.
UINT8 HAPI_ATOD_convert_ambient_light_sensor(void);

/****************************
  Питание
*****************************/

// Функция выключения.
//  0 - Выключение
//  1 - Перезагрузка
//  2 - ???
//  3 - Вход в boot
//  4 - ???
//  5 - ???
void pu_main_powerdown(UINT32 r0);

// Перезагрузка телефона.
void DL_PudSoftReset(void);

/* Функция ребута */
void HAPI_WATCHDOG_soft_reset(void);

/********************************
  HAPI чтение и запись сигналов
  Аналог ioctl и подобного.
********************************/

typedef UINT16 HAPI_SIGNAL_T;
typedef UINT32 HAPI_DATA_T;
void hPortWrite(HAPI_SIGNAL_T hapi_signal, HAPI_DATA_T write_data);
HAPI_DATA_T hPortRead(HAPI_SIGNAL_T hapi_signal);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_HAPI_H */

/** @} */ /* end of P2K_Types */
