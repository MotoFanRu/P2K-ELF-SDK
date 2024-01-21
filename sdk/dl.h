// UTF-8 w/o BOM

#ifndef __SDK_DL_H__
#define __SDK_DL_H__

#include <typedefs.h>
#include <time_date.h>
#include <dl_keypad.h>


#ifdef __cplusplus
extern "C" {
#endif

/******************************
   Аккумулятор 
*******************************/

#define HAPI_BATTERY_ROM_BYTE_SIZE       128
#define HAPI_BATTERY_ROM_UNIQUE_ID_SIZE    6

enum
{
    HAPI_BATTERY_ROM_NONE = 0,
    HAPI_BATTERY_ROM_W_DATA,
    HAPI_BATTERY_ROM_WO_DATA,
    HAPI_BATTERY_ROM_INVALID
};

typedef UINT8 HAPI_BATTERY_ROM_T;

HAPI_BATTERY_ROM_T HAPI_BATTERY_ROM_read(UINT8 *dest_address);
void HAPI_BATTERY_ROM_get_unique_id(UINT8 *unique_id);

extern const UINT32 *SBCM_ATOD_vltg;
extern const UINT32 *SBCM_ATOD_supply;

/******************************
   Свет 
*******************************/

/* Регулировка яркости дисплея 
	E398 ONLY! */
void HAPI_LP393X_disp_backlight_intensity( UINT32 light ); // light = 0..100

/* Боковые светодиоды и фонарик 
	E398 ONLY! */
void HAPI_LP393X_set_tri_color_led( UINT32 type, //  0-боковые, 1-фонарик;
									UINT32 val  //  0xRGB (0x000 - 0xFFF), например 0x00F - синий
								   );

typedef struct {
	unsigned int *addr;
	unsigned int  data;
	unsigned int  mask;
	UINT8  			rw;
	UINT8 		   reg;
} HAPI_CAP_ACCESS;

// клавиатура
void HAPI_PCAP_transceive(HAPI_CAP_ACCESS*);

/* Начать мигалку(funlight) под номером fl_id

	 E398 ONLY! */
UINT32 DL_AudStartFunlight( UINT8 fl_id,  UINT32 p2,  UINT32 p3 ); // p2 = 1, p3 = 0

/* Останавливает мигалку(funlight) под номером fl_id 
	E398 ONLY! */
UINT32 DL_AudStopFunlight( UINT8 fl_id );

//  Сенсор освещения.
UINT8 HAPI_ATOD_convert_ambient_light_sensor( void );

// подсветка клавиатуры
void DL_KeyUpdateKeypadBacklight( UINT8 P1 ); // P1 = 1 lighting; P2 = 0

// Регулировка яркости дисплея
UINT32 UIS_SetBacklightWithIntensity( UINT8 color, // = 255 
									  UINT8 intensity // = 0...6
									); 

#define FL_MAX_REGIONS		8

#define FL_REGION1_MASK		0x01
#define FL_REGION2_MASK		0x02
#define FL_REGION3_MASK		0x04
#define FL_REGION4_MASK		0x08
#define FL_REGION5_MASK		0x10
#define FL_REGION6_MASK		0x20
#define FL_REGION7_MASK		0x40
#define FL_REGION8_MASK		0x80
#define FL_ALL_REGIONS_MASK 0xff

/* запрос управления/освобождение регионов
** prior - желаемый приоритет, 0 - низший, 3 - высший
** mask  - сумма масок регионов. указывать те, которыми хотим управлять,
**         остальные будут освобождены
** возвращает сумму масок тех регионов, над которыми получили управление
** пример: DL_LED_FL_set_control( 3, 0x03 ); // экран и клава
*/
UINT8 DL_LED_FL_set_control( UINT8 prior, UINT8 mask );


/* установить значение регионов
** prior - указываем тоже что и в DL_LED_FL_set_control
** count - количество пар регион/значение
** ...   - тут пишем парами номера регионов и их значения
** возвращает сумму масок тех регионов, которые обновились
** пример: DL_LED_FL_update( 3, 2, 1, 5, 2, 0 ); // экран на уровень 5, клаву выключить
*/
UINT8 DL_LED_FL_update( UINT8 prior, UINT8 count, ... ); 

/******************************
   DbFeature
******************************/

#define ID_KEYPAD_LOCK_79              0x86A 
#define ID_KEYPAD_LOCK_49              0x7F3 
#define ID_WORKING_TABLE_49			   0x7F8

// чтение 
UINT8 DL_DbFeatureGetCurrentState( UINT16 fstate_ID, UINT8 *state );
// запись 
UINT8 DL_DbFeatureStoreState(UINT16 fstate_ID, UINT8 state);

UINT8 DL_DbFeatureStoreBlock(UINT16 fstate_ID, UINT8 *data, UINT8 data_length);

UINT8 DL_DbFeatureGetValue(UINT16 feature_id, UINT32 *feature_value );

UINT8 DL_DbFeatureGetValueString(UINT32 feature_id, WCHAR *feature_string );

UINT32 APP_UtilGetDataVolume (UINT16 feature_id, UINT64 * data_vol); 

 // время последнего звонка
#define C_TM S_TM-3

typedef struct
{
    UINT16 hours;
    UINT8  minutes;
    UINT8  seconds;
} CALL_TIME_T;

UINT8 DL_DbFeatureGetBlock( UINT16 feature_id, void *data_ptr);

// версия проши
#ifdef PATCH
	extern char product_version[];
#else
	extern char product_version[];
#endif

/*************************
  Симы
*************************/

typedef struct {
	UINT16		seem_element_id;
	UINT16		seem_record_number;
	UINT32		seem_offset;
	UINT32		seem_size;
} SEEM_ELEMENT_DATA_CONTROL_T; 


// чтение из сима
UINT16   SEEM_ELEMENT_DATA_read 	( 	SEEM_ELEMENT_DATA_CONTROL_T  *data_ctrl_ptr, // указатель на структуру данных, содержащую информацию о запросе
										UINT8  *data_buf, // указатель на буфер, куда прочитаются данные
										BOOL read_zero_byte_allowed  // если true, то можно читать маленькие симы, длина которых меньше 255
									);

//  запись в сим												
UINT16  SEEM_ELEMENT_DATA_write	(  	SEEM_ELEMENT_DATA_CONTROL_T  *data_ctrl_ptr,
									UINT8  *seem_data_ptr // указатель на буфер, где хранятся записываемые данные
								);	
								
//  возвращает размер сима							
UINT32 SEEM_GET_ADDRESS_LENGTH_element_length(UINT16 seem_element_id);
// возвращает кол-во records (записей) сима
UINT16 SEEM_MAX_RECORD_get_max_record(UINT16 seem_element_id);



/* Читает в буфер seem_data count байт сима seem, записи record
	Перед чтением ОБЯЗАТЕЛЬНО выделить не менее count байт памяти! */
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_read( UINT32 seem,  UINT32 record,  void* seem_data,  UINT32 count );

/* Записывает из буфера seem_data count байт в сим seem, запись record
	Не проверено */
#define SEEM_WRITE_METHOD_ADD				0
#define SEEM_WRITE_METHOD_UPDATE			1
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_write( UINT32 method,  UINT32 seem,  UINT32 record,  void* seem_data,  UINT32 count );
													
													
/****************************
  Питание 
*****************************/													
/* Функция ребута */
void HAPI_WATCHDOG_soft_reset( void );

/* Функция выключения */
void pu_main_powerdown(UINT32 r0);


/****************************
  Громкость 
****************************/

enum // volume_type
{
	BASE = 0,	// громкость звонка

	PHONE, // громкость клавиатуры
	VOICE, // громкость мультимедия  LTE
	MICROPHONE, // громкость разговора
	MULTIMEDIA, //  громкость мультимедия LTE2 и V3i
	PTT_TONES,
	MUTABLE_MAX,
	IMMUTABLE_MAX,
	MAX = IMMUTABLE_MAX
};

// установить громкость
void DL_AudSetVolumeSetting(UINT8 volume_type, UINT8 volume);
// получить текущую громкость
void DL_AudGetVolumeSetting(UINT8 volume_type, UINT8 *volume);

extern const UINT32 *GAIN_TABLE;
#define GAIN_TABLE_BIN		((UINT32*)*GAIN_TABLE)

extern const UINT32 *PARAM_TABLE;
#define PARAM_TABLE_BIN		((UINT32*)*PARAM_TABLE)

/****************************
  Звонки
****************************/

#define MAX_CALLS			7

typedef struct
{
	UINT16			call_id;  
	UINT8			call_state;
} CALL_ID_T;

typedef struct
{
	UINT8		number_of_calls;
	UINT8		overall_call_state;
	CALL_ID_T	call_state_info[MAX_CALLS];
} CALL_STATES_T;


// если number_of_calls == 0, то вызовов нет
void DL_SigCallGetCallStates(CALL_STATES_T *call_states);

// TRUE - звонок (входящий/исходящий)
BOOL APP_MMC_Util_IsVoiceCall(void); // если FALSE, то вызовов нет

BOOL AlmclkReminderUtilCallInProgress(void); // если FALSE, то никакого активного вызова нет

/********************************
  HAPI чтение и запись сигналов
  Аналог ioctl и подобного.
********************************/

typedef UINT16 HAPI_SIGNAL_T;
typedef UINT32 HAPI_DATA_T;
void hPortWrite(HAPI_SIGNAL_T hapi_signal, HAPI_DATA_T write_data);
HAPI_DATA_T hPortRead(HAPI_SIGNAL_T hapi_signal);

/********************************
  Прочее 
********************************/

// Location Area Identification - идентификатор зоны расположения
typedef struct
{
    UINT16  mcc; // Mobile Country Code - код страны
    UINT8   mnc; //Mobile Network Code - код сети
    UINT16  lac; // Location Area Code  -  код зоны расположения
} SIG_REG_LAI_T;
void DL_SigRegGetLAI(SIG_REG_LAI_T *lai);

// Полчает IMSI
UINT32 DL_DbSigNamGetTrueIMSI( UINT8 unk, // = 0,1,2...
							   WCHAR *imsi );
							   
// запсь на флеш, перед записью нужно сотрать блок						   
// относительно адреса 0x10000000, т.е. 0x0 = 0x10000000
UINT32 FlashDevWrite(UINT8 * buf, UINT32 addr, UINT32 size);
// стирает блок в 128 кб (забивает 0xFF) 
UINT32 FlashDevEraseBlock(UINT32 adr);

// воспроизведение тона/ возвращает seq_num
UINT32 DL_AudPlayTone( UINT32 tone,  UINT8 volume ); //Current volume = 0xFF
// остановка тона
void DL_AudStopTone(UINT32 tone, UINT32 seq_num);


// получение кодов
UINT32 DL_DbSigNamGetSecurityCode(WCHAR *);
UINT32 DL_DbSigNamGetUnlockCode(WCHAR *);

// статус PIN
enum {
	SIMPIN_STATUS_SECURED = 2,	// Также без симки
	SIMPIN_STATUS_NO_PIN,
	SIMPIN_STATUS_INVALID
};
UINT8 DL_SimMgrGetPinStatus( UINT8 card );

// сила сигнала сети
typedef struct
{
	UINT8 	percent;
	INT8	dbm;
} SIGNAL_STRENGTH_T;

void DL_SigRegQuerySignalStrength(SIGNAL_STRENGTH_T *signal_strength);

// csd/gprs/edge (возвращает что за интернет на текущей вышке Cell )
void DL_SigRegQueryGprsEgprsState(UINT8 * gprs_state);

// Cell Id
void DL_SigRegGetCellID(UINT16 *cell_id);

// показания батареи в процентах
UINT8 DL_PwrGetActiveBatteryPercent( void );

// true - гарнитура подключена
BOOL DL_AccIsHeadsetAvailable(void);

// true - подключено внешнее питание (зарядка, USB)
BOOL DL_AccIsExternalPowerActive(void);

// Буфер дисплея можно копировать сюда, по этому адресу.
extern UINT32 display_source_buffer;

UINT32 suDisableAllInt(void); // отключает прерывания
void suSetInt(UINT32 mask);   // включает прерывания, в параметрах передаётся то, что вернула suDisableAllInt

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __SDK_DL_H__
