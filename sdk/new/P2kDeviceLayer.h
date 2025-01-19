/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_H
#define P2K_SDK_DEVICE_LAYER_H

#include "P2kT.h"
#include "P2kSuApiT.h"
#include "P2kDeviceLayerT.h"

#ifdef __cplusplus
extern "C" {
#endif

struct IFACE_DATA_T {
	SU_PORT_T port;
	UINT32 handle;
};

// версия прошивки
UINT32 DL_DbFeatureGetSoftwareVersion(WCHAR *Version, UINT32 unk);  // unk = 0xFFFFFFFF;
#define GET_SOFTWARE_VERSION(Version) DL_DbFeatureGetSoftwareVersion(Version, 0xFFFFFFFF);

/****************************
  Громкость
****************************/

// Установить громкость
void DL_AudSetVolumeSetting(VOLUME_T volume_type, UINT8 volume);
// Получить текущую громкость
void DL_AudGetVolumeSetting(VOLUME_T volume_type, UINT8 *volume);

extern const UINT32 *GAIN_TABLE;
#define GAIN_TABLE_BIN ((UINT32 *) *GAIN_TABLE)

extern const UINT32 *PARAM_TABLE;
#define PARAM_TABLE_BIN ((UINT32 *) *PARAM_TABLE)

// воспроизведение тона/ возвращает seq_num
UINT32 DL_AudPlayTone(UINT32 tone, UINT8 volume);  // Current volume = 0xFF
// остановка тона
void DL_AudStopTone(UINT32 tone, UINT32 seq_num);

// Получение кодов
UINT32 DL_DbSigNamGetSecurityCode(WCHAR *SCode);
UINT32 DL_DbSigNamGetUnlockCode(WCHAR *UCode);

// Установка кодов
UINT32 DL_DbSigNamStoreSecurityCode(WCHAR *SCode);
UINT32 DL_DbSigNamStoreUnlockCode(WCHAR *UCode);

// запсь на флеш, перед записью нужно сотрать блок
// относительно адреса 0x10000000, т.е. 0x0 = 0x10000000
UINT32 FlashDevWrite(UINT8 *src, UINT32 addr, UINT32 size);

// чтение с флеш.
// относительно адреса 0x10000000, т.е. 0x0 = 0x10000000
UINT32 FlashDevRead(UINT8 *dst, UINT32 addr, UINT32 size);

// стирает блок в 128 кб (забивает 0xFF)
UINT32 FlashDevEraseBlock(UINT32 adr);

// сила сигнала сети
typedef struct {
	UINT8 percent;
	INT8 dbm;
} SIGNAL_STRENGTH_T;

// Получить силу сигнала сети
void DL_SigRegQuerySignalStrength(SIGNAL_STRENGTH_T *signal_strength);

// csd/gprs/edge (возвращает что за интернет на текущей вышке Cell )
void DL_SigRegQueryGprsEgprsState(UINT8 *gprs_state);

// Cell Id
void DL_SigRegGetCellID(UINT16 *cell_id);

// Статус подключения устройства (1 - вкл., 0 - выкл) по dev_id
BOOL DL_AccGetConnectionStatus(UINT8 dev_id);

/********************************
  Прочее
********************************/

// Location Area Identification - идентификатор зоны расположения
typedef struct {
	UINT16 mcc;  // Mobile Country Code - код страны
	UINT8 mnc;   // Mobile Network Code - код сети
	UINT16 lac;  // Location Area Code  -  код зоны расположения
} SIG_REG_LAI_T;

void DL_SigRegGetLAI(SIG_REG_LAI_T *lai);

// Полчает IMSI
UINT32 DL_DbSigNamGetTrueIMSI(
	UINT8 unk,  // = 0,1,2...
	WCHAR *imsi
);

// статус PIN
enum {
	SIMPIN_STATUS_SECURED = 2,  // Также без симки
	SIMPIN_STATUS_NO_PIN,
	SIMPIN_STATUS_INVALID
};

UINT8 DL_SimMgrGetPinStatus(UINT8 card);

// true - гарнитура подключена
BOOL DL_AccIsHeadsetAvailable(void);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_H */

/** @} */ /* end of P2K_Types */
