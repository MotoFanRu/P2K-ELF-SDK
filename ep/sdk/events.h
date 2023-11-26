// UTF-8 w/o BOM

#ifndef EVENTS_H
#define EVENTS_H

#include <typedefs.h>
#include <uis.h>
#include <dl_keypad.h>


#ifdef EP2
	#include <ev_codes2.h>
	#include <consts.h>
#else
	#include <ev_codes1.h>
#endif




#define GET_KEY(ev_st) ((AFW_GetEv(ev_st))->data.key_pressed)

#define GET_TIMER_ID(ev_st) (((DL_TIMER_DATA_T*)(AFW_GetEv(ev_st)->attachment))->ID)






typedef struct
{
	WCHAR	name[0x103];
	WCHAR	temp[0x75];
} START_PREVIEW_T;



typedef struct
{
	UINT32					bufsize;
	UINT32					index;
	BOOL					overflow;
} REQUEST_DATA_T;

typedef struct
{
	UINT32					ID;		   // ? Идентификатор списка
	UINT32					begin_idx;	// Начальный индекс требуемого списка элементов
	UINT8					count;		// Сколько эдементов списка требуется
} REQUEST_LIST_ITEMS_T;

typedef struct
{	
	UINT32					list_idx;
	UINT32					unk1;
	void *					ptr;
} DATA_CHANGE_DATA_T;



typedef struct
{
	char				uri[64];
	char				params[64];
} ELF_PARAMS_T;


/* Структура, отвечающая за данные, передаваемые с событием */
typedef union
{
	UINT8						key_pressed;
	UINT8						value8;
	UINT16						value16;
	UINT32						value32;
	UINT8						val_byte;
	UINT16						val_word;
	UINT32						val_dword;
	void *						val_ptr;
	UINT32						index;
	UINT32						params[16];
	EVENT_CODE_T				ev_code;
	
	REQUEST_LIST_ITEMS_T		list_items_req;
	REQUEST_DATA_T				data_req;
	DATA_CHANGE_DATA_T			data_change;
	
	UIS_TAG_AND_INDEX_T			tag_and_index;

	//ELF parameters
	ELF_PARAMS_T				start_params;
	//padding to real size
	UINT8						pad[0xFC]; // 252

} EVENT_DATA_T;

typedef struct
{
	UINT32					data_tag;
	EVENT_DATA_T			data;
} ADD_EVENT_DATA_T; // 256



struct EVENT_T
{
	EVENT_CODE_T			code;		// 0
	UINT32					seqnum;		// 4
	UINT32					ev_port;	// 8
	UINT8					ev_catg;	// 12
	BOOL					unk2;		// 13
	BOOL					first_pass;	// 14
	UINT8					unk3;		// 15
	UINT32					unk4;		// 16
	UINT32					unk5;		// 20
	struct EVENT_T			*next;		// 24
	UINT32					data_tag;	// 28
	EVENT_DATA_T			data;		// 32
	FREE_BUF_FLAG_T			free_buf;	// 32+252 = 284
	UINT16					att_size;	// 286
	void					*attachment;	// 288
	UINT8					unk6;		// 292
}; //size = 296



struct EVENT_STACK_T 
{
	EVENT_CODE_T			code;
	UINT8					unk1;
	UINT8					unk2;
	EVENT_T					*top;
	void					*unk3;
	UINT8					unk4[6];
	UINT8					unk5;
};


#ifdef __cplusplus
extern "C" {
#endif

// Получить текущий event из списка
EVENT_T* AFW_GetEv( EVENT_STACK_T *ev_st );

// код верхнего ивента из списка
EVENT_CODE_T AFW_GetEvCode( EVENT_STACK_T *ev_st );

// возвращает ev_st->top->seqnum
// приходит с ивентом в ответ на запросы, использующие IFACE_DATA_T. Равен IFACE_DATA_T::handle
UINT32 AFW_GetEvSeqn( EVENT_STACK_T * ev_st );

// Функции добавления ивента (в различных вариациях данных для ивента)
UINT32 AFW_AddEvNoD( EVENT_STACK_T		*ev_st,  
					 UINT32				event_code );

UINT32 AFW_AddEvEvD( EVENT_STACK_T		*ev_st,
					 UINT32				event_code,
					 ADD_EVENT_DATA_T	*data );

UINT32 AFW_AddEvAux( EVENT_STACK_T		*ev_st,
					 UINT32				event_code,
					 FREE_BUF_FLAG_T	free_buf,
					 UINT32				att_size,
					 void				*attachment );

UINT32 AFW_AddEvAuxD( EVENT_STACK_T		*ev_st,
					  UINT32			event_code,
					  ADD_EVENT_DATA_T	*data,
					  FREE_BUF_FLAG_T	free_buf,
					  UINT32			att_size,
					  void				*attachment );

// добавляет ивент и передаёт его диалогу
UINT32 AFW_TranslateEvEvD( EVENT_STACK_T	*ev_st,
						   UINT16			event_code,
						   ADD_EVENT_DATA_T *data ); 

// Удалет последний ивент из списка
UINT32 APP_ConsumeEv( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

//  Создает Internal ивент. . Реакцией на ивент будет старт приложения
UINT32 AFW_CreateInternalQueuedEvAux(	UINT32 event_code, 
										FREE_BUF_FLAG_T  free_buf, 
										UINT32 att_size, 
										void* att_data ); 

UINT32 AFW_CreateInternalQueuedEvAuxD(	UINT32 event_code, 
										ADD_EVENT_DATA_T *data, 
										FREE_BUF_FLAG_T  free_buf, 
										UINT32 size, 
										void *att_data);

UINT32 AFW_CreateInternalQueuedEvPriv(	UINT32				ev_code, 
										UINT32				param1,
										AFW_ID_T			afwid,
										UINT32				param3,
										UINT32				param4,
										ADD_EVENT_DATA_T	*data,
										FREE_BUF_FLAG_T		free_buf,
										UINT32				att_size,
										void				*attachment,
										UINT32				param9 );

// Добавляет ивент EV_LIST_ITEMS (ответ на EV_REQUEST_LIST_ITEMS)
UINT32 APP_UtilAddEvUISListData( EVENT_STACK_T		*ev_st,
								 void				*app,
								 UINT32				param2, // = 0
								 UINT32				begin_index,
								 UINT8				count,
								 FREE_BUF_FLAG_T	free_buf,
								 UINT32				bufsize,
								 void				*buffer );

// Добавляет ивент EV_REQUEST_DATA для поля с нужным индексом
UINT32 APP_UtilAddEvUISGetDataForItem(	EVENT_STACK_T	*ev_st,
										void			*app,
										UINT32			index ); 

// обновление листа
UINT32 APP_UtilAddEvUISListChange( EVENT_STACK_T	*ev_st,
								   void				*app,
								   UINT32			param2, // = 0
								   UINT32			position,
								   UINT8			size,
								   BOOL				refresh_data,
								   UINT8			param6,		  // = 2
								   FREE_BUF_FLAG_T	free_buf,
								   UINT32			bufsize,	// NULL ?
								   void				*buffer );	// NULL ?

// переход на заданный пункт листа
UINT32 APP_UtilAddEvChangeListPosition ( EVENT_STACK_T	 *ev_st,
										 void			 *app,
										 UINT32			  position,
										 FREE_BUF_FLAG_T  free_buf,
										 UINT32			  bufsize,
										 void			 *buffer );
 
#ifdef __cplusplus
} /* extern "C" */
#endif
 
#endif
