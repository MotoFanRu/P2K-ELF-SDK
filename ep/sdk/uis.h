// UTF-8 w/o BOM

#ifndef UIS_H
#define UIS_H

#include <typedefs.h>
#include <resources.h>
#include <filesystem.h>
#include <time_date.h>
#include <dl.h>


#ifdef __cplusplus
extern "C" {
#endif

enum
{
	ACTION_OP_ADD = 0,
	ACTION_OP_DELETE,
	ACTION_OP_UPDATE
};

typedef struct
{
	UINT8					operation;        // Что делать с текущим пунктом (ACTION_OP_*)
	EVENT_CODE_T			event;            // Ивент, который создаётся при выборе этого действия
	RESOURCE_ID				action_res;       // Ресурс, описывающий действие (см. RES_ACTION_LIST_ITEM_T)

} ACTION_ENTRY_T;

typedef struct
{
	EVENT_CODE_T			event;            // Ивент, который создаётся при выборе этого действия
	RESOURCE_ID				action_res;       // Ресурс, описывающий действие (см. RES_ACTION_LIST_ITEM_T)

} CORE_ACTION_ENTRY_T;

#define UIS_MAX_NUM_ACTION_LIST_ITEMS  16
typedef struct
{
	UINT8					count;            // Кол-во элементов
	ACTION_ENTRY_T			action[UIS_MAX_NUM_ACTION_LIST_ITEMS];       // Список команд дл работы со списком действий

} ACTIONS_T;


typedef struct UIS_TAG_AND_INDEX_T
{
	INT32					index;
	INT32					tag;
} UIS_TAG_AND_INDEX_T;

typedef union
{
	RESOURCE_ID				name;
	CLK_TIME_T				time;
	CLK_DATE_T				date;
	UINT32					number;
	WCHAR					*string;
	UINT8					status_ID;
	UIS_TAG_AND_INDEX_T		list_entry_id; 
	struct CONTENT_T		*content;
	FS_MID_T				file_id;

	UINT32					set_fill_percent; // Progress Bar - fill percentage
	WCHAR					*set_meter_value; // Progress Bar - Meter value string
	WCHAR					*set_object_name; // Progress Bar - Object Name string

} UIS_VALUE_T; // 8 bytes


#define UIS_FORMAT_SIZE			40
#define UIS_MAX_NUM_VALUES		20
typedef struct CONTENT_T
{
	char					format[UIS_FORMAT_SIZE];	// 40
	UINT32					count;						// 4
	UIS_VALUE_T				values[UIS_MAX_NUM_VALUES]; // 8*20 = 160

} CONTENT_T; // 204 bytes


typedef enum
{
	LIST_IMAGE_MEDIA_PATH_T = 0,
	LIST_IMAGE_RESOURCE_ID_T,
	LIST_IMAGE_DL_FS_MID_T, 			// именно это и юзает список яваприложений
	LIST_IMAGE_MEDIA_PATH_WITH_OFFSET_T,
	LIST_IMAGE_POINTER
} LIST_IMAGE_TYPE_T;


typedef struct
{
	WCHAR					*file_name;		// имя файла
	UINT32					offset;	    // оффсет
	UINT32					size;	        // размер
	UINT8					mime_type;		// тип
} LIST_IMAGE_FILE_T ;

typedef struct
{
	void * file_name;		// указатель на картинку
	UINT32 image_size;		// размер
} LIST_IMAGE_POINTER_T;

typedef union
{
	LIST_IMAGE_FILE_T		image_file;
	LIST_IMAGE_POINTER_T	image_ptr;
	FS_MID_T				file_id;
	WCHAR					*file_name;
	RESOURCE_ID				resource_id;
} LIST_IMAGE_T;

typedef struct
{
	LIST_IMAGE_T			image;
	UINT16					image_index;
	LIST_IMAGE_TYPE_T		image_type;
} LIST_IMAGE_ELEMENT_T;



typedef struct
{
	RESOURCE_ID				descr_res;				// 0x00 Ресурс с FIELD_DESCR_T
	UIS_VALUE_T				value;					// 0x04 Текущее значение (RESOURCE_ID/WCHAR*/CONTENT_T*/UINT32)
	BOOL					defined;				// 0x0c Combobinary: blank/value
	BOOL					value_on;				// 0x0d Combobinary: только когда defined==TRUE, OFF/value
	BOOL					app_control_editing;	// 0x0e = 0
} FIELD_T, UIS_FIELD_T; 


#define UIS_MAX_NUM_MARKS		2
typedef struct
{
	CONTENT_T				text;
	UINT32					number;		// for numbered list only
	RESOURCE_ID				marks[UIS_MAX_NUM_MARKS];
#if !defined(FTR_V600)
	RESOURCE_ID				unk3[2];
	RESOURCE_ID				unk4[2];
	UINT8					unk5;
#endif
	UINT8					formatting;
} STATIC_LIST_ENTRY_T;

typedef union
{
	STATIC_LIST_ENTRY_T		static_entry;     // Используется, когда editable == FALSE
	FIELD_T					editable_entry;   // Используется, когда editable == TRUE
#if !defined(FTR_V600)
	UINT8					pad[0xEC];
#else
	UINT8					pad[0xE6];
#endif
} LIST_CONTENT_T;

typedef struct
{
	BOOL					editable;		// Возможно ли изменять содержимое
	INT32					tag;			//
	LIST_CONTENT_T			content;

	/*
	 * EXL:
	 *   02-Jul-2023: Fix LIST_ENTRY_T struct size.
	 *   17-Aug-2023: Fix it again.
	 *
	 * V600 sizeof:    244 (f4): 2928
	 * ROKR E1 sizeof: 248 (f8): 2976
	 */

	BOOL					selection_list;	// если TRUE то на поведение влияют selectable и enabled
	UINT8					pad;
	BOOL					selectable;		// если FALSE то не выделяются (курсор их пропускает)
	BOOL					enabled;		// если FALSE то рисует другим цветом
} LIST_ENTRY_T;



/**************************
 Вспомогательные функции
************************* */      
      
/* Создаёт контент */
UINT32 UIS_MakeContentFromString( const char *format,  CONTENT_T *dst,  ... );
/* Подробнее про строку format:
    Строка состоит из указателей на входящие параметры
    и спецификаторов выравнивания.
    Указатели формируются как тип и номер параметра в функции,
	
    возможные типы:
    a - анимация
    d - дата
    q - указатель на WCHAR строку
    s - ресурс строки
    i - int
    j - LIST_IMAGE_ELEMENT_T
    l - long
    g - hex int
    p - ресурс картинки
    r - CONTENT_T
    f - указатель на файл
    t - время
	
    Спецификаторы выравнивания:
    N - новая строка
    S - распределение текста
    C - текст по центру
    L - обрезать с "...", если строка слишком длинная
    M - текст посередине (вертикальное выравнивание)
    T - текст сверху
    B - текст снизу
    
    Например: "MCq0p1" = "Вывести посередине и по центру первый 
    параметр после dst как строку, а второй - как картинку" */



/* Передаёт ивент диалогу */
UINT32 UIS_HandleEvent( UIS_DIALOG_T dialog,  EVENT_STACK_T *ev_st );

/* Вызвать полную перерисовку UI */
UINT32 UIS_Refresh( void );
void UIS_ForceRefresh(void);

/* Удаляет диалог и выставляет его в NULL */
UINT32 APP_UtilUISDialogDelete( UIS_DIALOG_T  *pdialog );
UINT32 UIS_Delete( UIS_DIALOG_T dialog );

// установить новый список действий
UINT32 UIS_SetActionList(UIS_DIALOG_T dialog, ACTIONS_T *action_list);

// очистить и установить новый список действий
UINT32 UIS_ClearAndSetActionList(UIS_DIALOG_T dialog, ACTIONS_T *action_list);

// устанавливает ивент на джойстик в центр
UINT32 UIS_SetCenterSelectAction (UIS_DIALOG_T dialog, EVENT_CODE_T center_select_action);

// устанавливает ивент на кнопку С
UINT32 UIS_SetClearKeyAction (UIS_DIALOG_T dialog, CORE_ACTION_ENTRY_T clear_action);

#define DialogType_None 0x00 // none
#define DialogType_Dialling 0x05 //dial-up window 
#define DialogType_EnterName 0x08 //input names. . .
#define DialogType_SetDate 0x0B //Set Date 
#define DialogType_SetTime 0x0D //set-up time 
#define DialogType_Homescreen 0x0E //desktop standby 
#define DialogType_List 0x10 //Item List 
#define DialogType_SelectionList 0x13 //options
#define DialogType_BatteryandMemory 0x15 //power, memory usage information, such as window howed that the grid 
#define DialogType_JumpOut 0x17 //pop-up window, such as "keyboard is locked, unlocked by XX", "please wait" category 
#define DialogType_MessageBox 0x1B //information inbox
#define DialogType_GameAni 0x1D //games and applications when they enter the animation 
#define DialogType_WapMenu 0x1E //Wap browser-related menu 
#define DialogType_VideoCamera 0x25 //Camera viewfinder window 
#define DialogType_Password 0x26 //Password window 
#define DialogType_PB_SC_Other 0x28 //phone book, Shortcuts like, a lot of this state 
#define DialogType_Menu 0x2D //Main Menu 
#define DialogType_SecondLevelMenu 0x2E //2 of the three menu and the menu
#define DialogType_WriteText 0x32 //input text 
#define DialogType_Brightness 0x37 //Settings - brightness - highlighted regulation
#define DialogType_Picture 0x39 //Multimedia - picture

//Возвращает "тип" верхнего диалога. В приложениях прошивки часто используется для определения нахождения на рабочем столе.
// Если в res значение 0xE, значит на рабочем столе. Для обычного списка в res значение 0x10. Поэксперементируйте если вам это нужно.
UINT32 UIS_GetActiveDialogType(UINT8* res);

/***************************
Функции создания диалогов 
****************************/

UIS_DIALOG_T 
UIS_CreateNullDialog( SU_PORT_T *port );


// типы для Trancient Notice
enum
{
	NOTICE_TYPE_DEFAULT = 0,// значек "внимание"
	NOTICE_TYPE_OK,			// галачка
	NOTICE_TYPE_FAIL,		// крестик
	NOTICE_TYPE_WAIT,
	NOTICE_TYPE_WAIT_NO_KEY = NOTICE_TYPE_WAIT,// кнопки блокируются, к контенту в конце добавляются точки, приложение должно само както вывести из стэйта
							// кнопками можно вывести только вызвав метку (при этом закроется приложение)
	NOTICE_TYPE_WAIT_2,		// точек нет, при нажатии на любую кнопку генерируется ивент EV_DIALOG_DONE
	NOTICE_TYPE_INFO_NO_KEY,// на кнопки не реагирует
	NOTICE_TYPE_INFO,
	NOTICE_TYPE_OK_2,
	
	NOTICE_TYPE_DELETED = 12,
	NOTICE_TYPE_NEW_MESSAGE,
	NOTICE_TYPE_SMS,
	NOTICE_TYPE_SEND,
	NOTICE_TYPE_SEND_CENTER,// на скине mpx_ultra криво выглядет!
	
	NOTICE_TYPE_WAIT_3 = 27,// с точками, кнопки не блокируются
	NOTICE_TYPE_WAIT_4,		// текст слева, с точками, при нажатии красной генерируется EV_DIALOG_DONE
	NOTICE_TYPE_INFO_LONG,
	NOTICE_TYPE_WAIT_FAST,	// диалог ожидания на пол секунды
	
	NOTICE_TYPE_WAIT_5 = 32,// также как NOTICE_TYPE_WAIT_2
	
	NOTICE_TYPE_WAIT_6 = 37,// центр, с точками, при нажатии любой кнопки генерируется EV_DIALOG_DONE
	
	NOTICE_TYPE_CHAT = 45,
	NOTICE_TYPE_NEW_CHAT_MESSAGE
	
	// дальше ничего интересного вроде
};
typedef UINT8 NOTICE_TYPE_T;

enum  {
	INCOMING_CALL_STATUS = 0,
	CALLING_STATUS,
	CONNECTED_STATUS,
	NO_STATUS
};

enum {
	EXTENDENT_NOTICE_GENERAL = 0,
	EXTENDENT_NOTICE_DELAY,
	EXTENDENT_NOTICE_TRANSIENT
};
typedef UINT8 EXTENDENT_NOTICE_TYPE_T;

typedef struct
{
	RESOURCE_ID			icon1; 
	RESOURCE_ID			icon2; 

	RESOURCE_ID			animation_icon1;
	RESOURCE_ID			animation_icon2;

	void				*key_translation_table;

	RESOURCE_ID			title;

	UINT8				numberOfKeyTranslations;
	UINT8				numberOfResources;
	BOOL				isCritical;
} NOTICE_PARAMETERS_T;

/* Создаёт окошко с сообщением. type = NOTICE_TYPE_* */
UIS_DIALOG_T UIS_CreateTransientNotice( SU_PORT_T *port,  CONTENT_T *content,  UINT8 type );

UIS_DIALOG_T UIS_CreateNotice( SU_PORT_T         *port,
							   CONTENT_T         *content,
							   UINT32             duration,
							   UINT8              type,
							   BOOL               critical,
							   ACTIONS_T         *actions );

UIS_DIALOG_T  UIS_CreateExtendedNotice( SU_PORT_T 				*port,  
										CONTENT_T 				*content,
										UINT32					duration, // время отображения
										UINT8					type, 
										UINT16 					status,
										NOTICE_PARAMETERS_T 	parameters,
										ACTIONS_T       		*actions );

/* Создаёт диалог-запрос */
UIS_DIALOG_T UIS_CreateConfirmation( SU_PORT_T *port,  CONTENT_T *content );
  


enum
{
	UIS_LIST_VIEW_SUMMARY = 0,
	UIS_LIST_VIEW_DETAIL,
	UIS_LIST_VIEW_DEFAULT,
	UIS_LIST_VIEW_PAGEABLE_DETAIL,
	UIS_LIST_VIEW_DETAIL_BY_LINE,
	UIS_LIST_VIEW_DETAIL_SAVE_MEMORY,
	UIS_LIST_VIEW_PAGEABLE_SUMMARY,
	UIS_LIST_VIEW_SUMMARY_WITH_SUBTITLE
};
typedef UINT8 UIS_LIST_VIEW_T;

/* Создаёт диалог-список */
UIS_DIALOG_T UIS_CreateList(		SU_PORT_T       *port,
									UINT32          param1,          // = 0  ID?
									UINT32          count,           // Количество пунктов в списке
									UINT32          marks,           // Что-то связанное с пометками
									UINT32          *starting_num,   // Сколько пунктов требуется передать
									BOOL            reorder,         // = 0
									UINT8           view,            // = 2
									ACTIONS_T       *actions,        // Список действий
									RESOURCE_ID     caption );       // Заголовок списка


/* Создаёт статичекий диалог-список */
UIS_DIALOG_T UIS_CreateStaticList(	SU_PORT_T  	  *port,
									UINT32          param1,          // = 0 ID?
									UINT32          count,
									UINT32          marks,
									LIST_ENTRY_T    *entries, 
									BOOL            reorder,         // = 0
									UINT8           view,            // = 2
									ACTIONS_T       *actions,
									RESOURCE_ID     caption );

UIS_DIALOG_T UIS_CreateSelectionEditor(
									SU_PORT_T		*app_env,
									UINT32			list_handle,
									UINT32			list_size, 
									UINT32			current_position,
									UINT32			*num_entries_to_get,
									RESOURCE_ID		possible_values,
									ACTIONS_T		*action_list, 
									RESOURCE_ID		dialog_resource_id);

UIS_DIALOG_T UIS_CreateStaticSelectionEditor(
									SU_PORT_T		*app_env,
									UINT32			list_handle,
									UINT32			list_size,
									LIST_ENTRY_T*	list_entries, 
									UINT32			current_position,
									ACTIONS_T		*action_list,
									RESOURCE_ID		dialog_resource_id);

// Создает текстовой редактор
UIS_DIALOG_T UIS_CreateCharacterEditor( SU_PORT_T       *port,
										WCHAR           *text,
										UINT8           edit_type,
										UINT32          maxlen,
										BOOL            masked, // вводить текст в закрытом виде (звёздочками)
										ACTIONS_T       *actions, 
										RESOURCE_ID     dlgres ); 
/* 
  Значения edit_type:
0 - Каждое Слово С Большой Буквы
1 - простой текст
2 - Каждое предложение. С большой буквы
3 - аналогично, для очень длинных строк
4 - номер телефона
5 - ВСЕ БУКВЫ БОЛЬШИЕ 
6 - строка как последовательность числовых сегментов
7 - простой текст без смены режимов
8 - аналогично, без детализации
9 - как 4, без доп меню
10 - аналог, без детализации
11 - как 0, с возможностью ввода пустой строки
12 - как 1, с возможностью ввода пустой строки
23 - url
28 - строка с аттачами
29 - тоже, но можно и пусто
32 - только числа
*/

// диалог для просмотра
UIS_DIALOG_T UIS_CreateViewer( SU_PORT_T  	   *port,
                               CONTENT_T       *contents,
                               ACTIONS_T       *actions );

/* Создает диалог вывода контента, где заголовок и тело задаются отдельно */
UIS_DIALOG_T
UIS_CreateViewerWithTitle( SU_PORT_T *		port,
							CONTENT_T *		title,
							CONTENT_T *		body,
							ACTIONS_T *		actions,
							RESOURCE_ID		dlgres );

// список с картинками и тестом                        
UIS_DIALOG_T UIS_CreatePictureAndTextList( SU_PORT_T  	   *port,
                                           UINT32          param1,
                                           UINT32          count, 
                                           UINT32          *starting_num, 
                                           UINT8           param4, // = 2
                                           UINT8           param5, // = 0
                                           UINT8           param6, // = 1, try 0,2,...
                                           ACTIONS_T       *actions, 
                                           RESOURCE_ID     dlgres );

/* Создать диалог-заставку из файла (картинки) */
UIS_DIALOG_T UIS_CreateSplashScreenFs( SU_PORT_T * port, WCHAR * uri, UINT32 unk );	// unk = 0x7D0

/* Создает диалог-заставку из ресурса картинки */
UIS_DIALOG_T UIS_CreateSplashScreen( SU_PORT_T * port, RESOURCE_ID img, UINT32 unk );	// unk = 0xD3 , 0x7D0

/* Создает диалог "хранитель экрана" из картинки. Поддерживаются: GIF, JPG, PNG, BMP, и др. */
UIS_DIALOG_T UIS_CreateScreenSaver( SU_PORT_T * post, WCHAR * uri );

UIS_DIALOG_T
UIS_CreateMixedContentViewer( SU_PORT_T * port, CONTENT_T * content, ACTIONS_T * action_list, RESOURCE_ID dlgres );

// Создаёт прогрессбар
UIS_DIALOG_T UIS_CreateProgressBar( SU_PORT_T		*port,
									RESOURCE_ID		primary_icon,	// 0x12000055
									CONTENT_T		*content,
									const WCHAR			*object_name,
									UINT32			fill_percentage,
									BOOL			arrovs,
									const WCHAR			*meter_value,
									ACTIONS_T		*actions,
									RESOURCE_ID		dlgres );

#if defined(EP1)
// Ивенты для этого диалога
#define EV_PB_SET_PERCENT		0x213F
#define EV_PB_SET_VALUE_STR		0x2140
#define EV_PB_SET_STR			0x2141
#endif

/***************************
 Скин
****************************/
// получить индекс текущего скина
UINT16 UIS_GetCurrentSkinIndex(void);
// получить индекс скина поумолчанию
UINT16 UIS_GetDefaultSkinIndex(void);
// получение имени скина
WCHAR* UIS_GetSkinName(UINT16 skin_id);
// кол-во скинов
UINT16 UIS_GetNumSkins(void);
// установить скин
void UIS_SetSkin(UINT16 skin_id);

// папка скина, строка в юникоде
#if (defined(EP2) || defined(PATCH))
	extern const WCHAR SKIN_FOLDER[];
#else
	extern const WCHAR * SKIN_FOLDER;
#endif


/******************************************
Изменение элементов  рабочего стола 
******************************************/

#define BATTERY_STRENGTH_STATUS		0  //батарея: значения integer от 0 до 3
#define SIGNAL_STRENGTH_STATUS		3  //сигнал сети: значения integer от 0 до 5


typedef union
{
	INT32						integer;
	BOOL						boolean;
	WCHAR						*quoted_string;
	UINT64						unk;
} STATUS_VALUE_T;

// запись инфы на рабочий стол
UINT32 UIS_SetStatus(UINT8 status_id, STATUS_VALUE_T value);

UINT32 UIS_SetStatusOff( UINT8 id, STATUS_VALUE_T status );

// чтение инфы с рабочего стола
UINT32 UIS_GetCurrentStatusValue(UINT8 status_id,  STATUS_VALUE_T *value);


enum
{
	WALLPAPER_LAYOUT_CURRENT = 0,
	WALLPAPER_LAYOUT_CENTERED,
	WALLPAPER_LAYOUT_TILED,
	WALLPAPER_LAYOUT_FIT_TO_SCREEN
};
typedef UINT8 WALLPAPER_LAYOUT_T;

typedef struct
{
	WCHAR					*url;
	WALLPAPER_LAYOUT_T		layout;
} SETWALLPAPER_SETTINGS_T;  

// Установить картинку на рабочий стол
UINT32 UIS_SetWallpaper(SETWALLPAPER_SETTINGS_T * );

// Остановить обновление статус-строки
extern char theStatusManager;
void setCanvasControl__13StatusManagerFScPUs( void *, BOOL, void* );

/***************
  Прочее
****************/

// буфер обмена (копировать/ вставить в редакторе)

#define CLIPBOARD_LEN 5120
#if (defined(EP2) || defined(PATCH))
	extern WCHAR * clipboard_data_buf;
	#define clipboard (clipboard_data_buf)
#else
	extern UINT32 * clipboard_data_buf;
	#define clipboard ((WCHAR*)*clipboard_data_buf)
#endif


#define ustr2cb(w) u_strcpy( clipboard , w )
#define cb2ustr(w) u_strcpy( w, clipboard)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
