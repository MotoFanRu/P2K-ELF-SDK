// UTF-8 w/o BOM

#ifndef __SDK_SC_H__
#define __SDK_SC_H__


#include <typedefs.h>
//#include <events.h>

/********************************
  Метки
*********************************/


#define SC_TYPE_URL		5
#define SC_TYPE_APP		6

#define SC_TEXT_LEN				16
#define SC_MAX_NUMBER			100
#define SC_ALL_RECORD			0xFE
#define SC_INVALID_RECORD		0xFF

typedef UINT8 SC_TYPE_T;

typedef struct
{
	UINT8				seem_rec;	// seem_rec-1
	SC_TYPE_T			type;		// тип метки 
	UINT8				index;	   // номер в списке, отсчет от 1. Номер метки
	UINT8				unk1;		// 0xFE
	UINT32				ev_code;
	UINT32				list1_index;	 // id выделенного пункта списка
	UINT32				param1;	   
	UINT32				list2_index; // тоже какой-то id пункта
	UINT32				param2;	   
	INT32				data;		 // используется для указателя на данные, например указатель на URL-адрес 
	UINT32				lang_text;   // RESOURCE_ID
	WCHAR				text[SC_TEXT_LEN+1];	 // имя метки
	UINT16				unk2;		// 0xffbd
	
} SEEM_0002_T;

typedef struct {
	UINT8			type;
	UINT32			param1;
	UINT32			list1_index;
	UINT32			param2;
	UINT32			list2_index;
	WCHAR			text[SC_TEXT_LEN+1]; // 0x22 = 34
	UINT32			lang_text;
} SHORTCUT_CREATE_T; // 0x3c = 60


typedef struct 
{
	UINT8			result; // 0 - если успешно
	UINT8			unk0;
	SEEM_0002_T 	*record;
} SHORTCUT_READ_RECORD_T;

typedef struct 
{ 
	UINT8			result; 
	UINT16			url_strlen;
	UINT8			url[100];
} SHORTCUT_READ_URL_T;


#ifdef __cplusplus
extern "C" {
#endif


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

// DL_DbShortcutIsNumberAvailable // номер существует?
// первый доступный номер
UINT8 DL_DbGetFirstAvailableNumber( void ); 

// получает кол-во используемых  записей URL
UINT16 DL_DbShortcutGetNumOfURLRecordsUsed( void );

// получает кол-во доступных записей URL
UINT16 DL_DbShortcutGetNumOfURLRecordsAvailable( void );

// получает URL. Функция асинхронная. Отправляет EV_SHORTCUT_READ_URL
UINT32 DL_DbShortcutGetURLByURLId( IFACE_DATA_T *data, UINT32 URLId);

// получение типа записи
SC_TYPE_T DL_DbShortcutGetshortcutType( UINT8 index );

// создание метки
UINT32 APP_UtilShortcutCreateAppShortcut ( EVENT_STACK_T *ev_st,  void *app, UINT32 event_code );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* __SDK_SC_H__ */

/* EOF */
