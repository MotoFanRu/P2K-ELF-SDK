// UTF-8 w/o BOM
/********************************
  Справочник
********************************/

#ifndef __SDK_PHONEBOOK_H__
#define __SDK_PHONEBOOK_H__

#include <typedefs.h>


typedef struct {
	UINT8		result;	   // результат операции. 0 - если успешно.
	UINT8		list_type;	// тип листа
	UINT8		sort_type;	// тип сортировки
	UINT8		count_items;  //  кол-во записей в buf
	UINT8		buf[0x4000];  //  данные записи
} PHONEBOOK_READ_RECORD_T;


#define PB_RECORD_NAME_LEN 24
#define PB_RECORD_NUMBER_LEN 50
// запись справочника
typedef struct {
	UINT16		index; // индекс записи
	WCHAR		name[PB_RECORD_NAME_LEN+1]; // имя
	WCHAR		number[PB_RECORD_NUMBER_LEN+1]; // номер
	UINT8		pad[383];
} PHONEBOOK_RECORD_T; // 0x21C

// запись категории
typedef struct {
	WCHAR		name[PB_RECORD_NAME_LEN+1];
	UINT8		id;
	UINT8		index;
	UINT8		pad[6];
} PHONEBOOK_CATEGORY_RECORD_T; // 0x3A

typedef enum {
	PHONEBOOK_DEVICE_PHONE,
	PHONEBOOK_DEVICE_SIM,
	PHONEBOOK_DEVICE_ALL
} PHONEBOOK_DEVICE_T;


#ifdef __cplusplus
extern "C" {
#endif

// прочитать запись из справочника. Отправляет ивент EV_PHONEBOOK_READ_RECORD
UINT32 DL_DbPhoneBookGetRecordByIndex(	IFACE_DATA_T	*data,
										UINT8			list_type, // 0 - справочник
										UINT16			index, // индекс записи
										UINT8			count_items,
										UINT8			sort_type ); 

// поиск записи по имени
UINT32 DL_DbPhoneBookFindRecordByName(	UINT8			list_type,
										WCHAR			*name, 
										UINT8			sort_type,
										UINT16			*index );

// поиск  записи по номеру телефона. Отправляет ивент EV_PHONEBOOK_READ_RECORD
UINT32 DL_DbPhoneBookFindRecordByTelno(	IFACE_DATA_T	*data, 
										UINT8			list_type,
										WCHAR			*tel_number, 
										UINT8			sort_type );

// Кол-во возможных записей
UINT32 DL_DbPhoneBookGetNumberRecords(	UINT8			list_type,
										PHONEBOOK_DEVICE_T device, 
										UINT16			*count_numbers );
// кол-во используемых записей
UINT32 DL_DbPhoneBookGetUsedRecords(	UINT8			list_type,
										PHONEBOOK_DEVICE_T device, 
										UINT16			*count_numbers,
										UINT8			sort_type );
// кол-во доступных записей
UINT32 DL_DbPhoneBookGetAvailableRecords(	UINT8		list_type,
											PHONEBOOK_DEVICE_T device, 
											UINT16		*count_numbers);

// удаление записи
UINT32 DL_DbPhoneBookDeleteRecordByIndex(	IFACE_DATA_T *iface_data, 
											UINT8		list_type,
											UINT16		index, 
											UINT8		sort_type );

// Удалить все записи
UINT32 DL_DbPhoneBookDeleteAllRecords(	IFACE_DATA_T	*iface_data, 
										UINT8			list_type);

// обновить запись
UINT32 DL_DbPhoneBookUpdateRecord(	IFACE_DATA_T		*iface_data, 
									UINT8				list_type,
									void				*record, 
									UINT8				sort_type );

// новая запись
UINT32 DL_DbPhoneBookStoreRecord(	IFACE_DATA_T		*iface_data, 
									UINT8				list_type,
									void				*record, 
									UINT8				sort_type );

// возвращает кол-во категорий
UINT32 DL_DbPhoneBookGetNumberofCategories(	UINT8		list_type,
											PHONEBOOK_DEVICE_T device,
											UINT16		*count_categories );

// получение категории. Отправляет ивент EV_PHONEBOOK_READ_RECORD
UINT32 DL_DbPhoneBookGetCategoryRecordByIndex(	IFACE_DATA_T *data, 
												UINT8	list_type,
												PHONEBOOK_DEVICE_T device,
												UINT8	category_index );

// запись категории
UINT32 DL_DbPhoneBookStoreCategoryRecord(	IFACE_DATA_T *data,
											UINT8		list_type,
											PHONEBOOK_DEVICE_T device,
											void		*category_record, // PHONEBOOK_CATEGORY_RECORD_T
											UINT8		*unk ); // может быть NULL

// удаление категории
UINT32 DL_DbPhoneBookDeleteCategoryRecord(	IFACE_DATA_T *data,
											UINT8		list_type,
											PHONEBOOK_DEVICE_T device,
											UINT8		category_id );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __SDK_PHONEBOOK_H__
