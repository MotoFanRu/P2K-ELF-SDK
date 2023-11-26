#ifndef SC_H
#define SC_H

/********************************
  Метки
*********************************/

#include <typedefs.h>
#include <events.h>


#ifdef __cplusplus
extern "C" {
#endif


#define SC_TYPE_URL		5
#define SC_TYPE_APP		6

typedef UINT8 SC_TYPE_T;

// передается ивентом в аттаче при вызове меткой
typedef struct
{
/*+00*/    UINT8            	seem_rec;    // seem_rec-1
/*+01*/    UINT8				type;        // тип метки 
/*+02*/    UINT8            	index;       // номер в списке, отсчет от 1. Номер метки
/*+03*/    UINT8            	unk1;        // 0xFE
/*+04*/    UINT32            	ev_code;
/*+08*/    UINT32            	list1_index;	 // id выделенного пункта списка
/*+12*/    UINT32            	param1;       
/*+16*/	   UINT32            	list2_index; // тоже какой-то id пункта
/*+20*/    UINT32            	param2;       
/*+24*/    UINT32            	data;	     // 
/*+28*/    UINT32		        lang_text;   // RESOURCE_ID
/*+32*/    WCHAR            	text[17];	 // имя метки // 34
/*+66*/    UINT8            	state;       // номер state, при котором создана метка
} SEEM_0002_T;
// 68

#define SHORTCUT_RECORD_T		SEEM_0002_T

/* передается в аттаче с ивентом 
#define EV_HANDLE_SHORTCUT			0x20C5 
*/
typedef struct 
{
/*+00*/		UINT8			type;
/*+04*/		UINT32			param1;
/*+08*/		UINT32			list1_index;
/*+0C*/		UINT32			param2;
/*+10*/		UINT32			list2_index;
/*+14*/		WCHAR			text[17]; // 0x22 = 34
/*+38*/		UINT32			lang_text;
} SHORTCUT_CREATE_T; // 0x3c = 60


UINT32 APP_UtilShortcutBuildRecord(EVENT_STACK_T *ev_st, SHORTCUT_RECORD_T *sc_data, UINT32 ev_code, UINT32 state, UINT32 data);
/* формирует sc_data на основе параметров ev_code, state, data и данных из аттача 
аттач типа SHORTCUT_CREATE_T
*/

UINT32 APP_UtilShortcutCopyRecordFromEv(EVENT_STACK_T *ev_st, void *app);
/* копирует аттач (SHORTCUT_RECORD_T) в app->sc_data (выделяет память) 
*/

UINT32 APP_UtilShortcutAcceptCreate(EVENT_STACK_T *ev_st, void *app, UINT32 ev_code, UINT32 state, UINT32 data);
/* вызывает стандартный диалог создания метки
формирует запись на основе параметров ev_code, state и данных из аттача
ev_code - ивент, вызываемый меткой
state - состояние приложения, хотя для своих целей можно и что-нить другое
data - доп. данные, хотя для своих целей можно и что-нить другое
*/

UINT32 APP_UtilShortcutCreateAppShortcut(EVENT_STACK_T *ev_st, void *app, UINT32 ev_code);
/* вызывает стандартный диалог создания метки
формирует запись на основе параметров ev_code и данных из аттача
ev_code - ивент, вызываемый меткой
SHORTCUT_RECORD_T.type = SC_TYPE_APP
SHORTCUT_RECORD_T.state = 0xFF
*/

UINT32 APP_UtilShortcutGetShortcutType(UINT32 t /*SC_RECORD_T.type*/);
// получить тип метки из записи (не равен SHORTCUT_RECORD_T.type)

UINT32 APP_UtilShortcutRejectInvoke(EVENT_STACK_T *ev_st, void *app);
/* вызывает нотайс о запрете вызова метки
 данные берутся или из app->sc_data или из аттача */

UINT32 APP_UtilShortcutRejectCreate(EVENT_STACK_T *ev_st, void *app);
/* если ивент != EV_HANDLE_SHORTCUT то вернет RESULT_FAIL
иначе вызывает нотайс о запрете создания метки */



/********************************
  Метки
*********************************/
// непроверено !

// при чтении метки
#define EV_SHORTCUT_READ_RECORD                0x820F7 // в аттаче SHORTCUT_READ_RECORD_T
// при чтении URL метки
#define EV_SHORTCUT_READ_URL	               0x82112 // в аттаче  SHORTCUT_READ_URL_T

typedef struct 
{
    UINT8       	result; // 0 - если успешно
    UINT8       	unk0;
    SEEM_0002_T 	*record;
} SHORTCUT_READ_RECORD_T;

typedef struct 
{ 
    UINT8  			result; 
    UINT16          url_strlen;
    UINT8		    url[100];
} SHORTCUT_READ_URL_T;


// создание метки									
UINT8 DL_DbShortcutCreateRecord( IFACE_DATA_T  *data, SEEM_0002_T  record );

// обновление метки
UINT8 DL_DbShortcutUpdateRecord( IFACE_DATA_T  *data, SEEM_0002_T  record );

// удаление метки
UINT8 DL_DbShortcutDeleteRecord( IFACE_DATA_T  *data, UINT8 seem_rec);

// получение метки. Функция асинхронная. Отправляет EV_SHORTCUT_READ_RECORD. В aatchment  будет SHORTCUT_READ_RECORD_T
UINT8 DL_DbShortcutGetRecordByRecordId( IFACE_DATA_T  *data, UINT8  seem_rec);

// получает ко-во доступных записей
UINT16 DL_DbShortcutGetNumOfRecordsAvailable( void ); 

// получает кол-во используемых записей
UINT16 DL_DbShortcutGetNumOfRecordsUsed( BOOL voice_shortcut );

// получает первый доступный номер
UINT8 DL_DbGetFirstAvailableNumber( void ); 

// получает кол-во используемых  записей URL
UINT16 DL_DbShortcutGetNumOfURLRecordsUsed( void );

// получает кол-во доступных записей URL
UINT16 DL_DbShortcutGetNumOfURLRecordsAvailable( void );

// получает URL. Функция асинхронная. Отправляет EV_SHORTCUT_READ_URL
UINT32 DL_DbShortcutGetURLByURLId( IFACE_DATA_T *data, UINT32 URLId);

// получение типа записи
SC_TYPE_T DL_DbShortcutGetshortcutType( UINT8 index );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif