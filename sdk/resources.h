// UTF-8 w/o BOM

#ifndef RESOURCES_H
#define RESOURCES_H

#include <typedefs.h>



/* ID ресурса формируется типом ресурса в качестве старшего байта и номером ресурса текущего типа
    Например, 0x01000496 = "Системная ошибка"
    Идентификаторы для строк и картинок берутся, соответственно, из ленгпака и DRM */


/* 
	DRM Resource Type defination 
*/
#define DRM_TYPE_INVALID				0x0
#define DRM_TYPE_PROMPT					0x1
#define DRM_TYPE_BITMAP					0x2
#define DRM_TYPE_COMPOSITE				0x3
#define DRM_TYPE_INTEGER				0x4
#define DRM_TYPE_BOOLEAN				0x5
#define	DRM_TYPE_ALI					0x6
#define DRM_TYPE_LIST					0x7
#define DRM_TYPE_EDITOR					0x8
#define DRM_TYPE_COMBO					0x9
#define DRM_TYPE_FORM					0xa
#define DRM_TYPE_PICKER					0xb
#define DRM_TYPE_CONTENT				0xc
#define DRM_TYPE_FIELD					0xd
#define DRM_TYPE_METER					0xe
#define DRM_TYPE_NUMBER					0xf
#define DRM_TYPE_ENUM					0x10
#define DRM_TYPE_ANIMATION				0x12
#define DRM_TYPE_TAGGED					0x13
#define DRM_TYPE_MMA					0x14
#define DRM_TYPE_FONT_SIZE				0x15
#define DRM_TYPE_EDITOR_FORMATTER		0x16
#define DRM_TYPE_INCALL_FORMATTER		0x17
#define DRM_TYPE_LIST_FORMATTER			0x18
#define DRM_TYPE_METER_FORMATTER		0x19
#define DRM_TYPE_NOTICE_FORMATTER		0x1a
#define DRM_TYPE_PICKER_FORMATTER		0x1b
#define	DRM_TYPE_PROGRESS_BAR_FORMATTER	0x1c
#define DRM_TYPE_WEEKVIEW_FORMATTER		0x1d
#define DRM_TYPE_CUSTOM					0x1e
#define DRM_TYPE_COLORS_FORMATTER		0x1f
#define DRM_TYPE_MARGINS_FORMATTER		0x20
#define DRM_TYPE_INIT_TIMER				0x21
#define DRM_TYPE_COLOR_SCHEME			0x22
#define DRM_TYPE_SINGLE_FRAME_GIF		0x25
#define DRM_TYPE_FILE_NAME				0x28
#define DRM_TYPE_CUSTOMIZED_ICON		0x29

/* these are for UIS compatibility issues */
#define	RES_PROMPT						DRM_TYPE_PROMPT
#define	RES_PICTURE_BITMAP				DRM_TYPE_BITMAP
#define	RES_INTEGER						DRM_TYPE_INTEGER
#define	RES_BOOLEAN						DRM_TYPE_BOOLEAN
#define	RES_UIS_ACTION_LIST				DRM_TYPE_ALI
#define	RES_UIS_LIST					DRM_TYPE_LIST
#define	RES_UIS_EDITOR					DRM_TYPE_EDITOR
#define	RES_UIS_COMBO_BINARY_EDITOR		DRM_TYPE_COMBO
#define	RES_FORM						DRM_TYPE_FORM
#define	RES_COMPOSITE_DIALOG			DRM_TYPE_COMPOSITE
#define	RES_PICKER						DRM_TYPE_PICKER
#define	RES_CONTENT						DRM_TYPE_CONTENT
#define	RES_FIELD_DESCRIPTION			DRM_TYPE_FIELD
#define	RES_METER_FIELD					DRM_TYPE_METER
#define	RES_NUMBER_FIELD				DRM_TYPE_NUMBER
#define	RES_ENUM_FIELD					DRM_TYPE_ENUM
#define	RES_ANIMATION_LABEL				DRM_TYPE_ANIMATION  
#define RES_FILE_NAME					DRM_TYPE_FILE_NAME
#define	RES_TAGGED_CONTENT				DRM_TYPE_TAGGED

#if 0
// G-XAD SDK
#define RES_TYPE_STRING					RES_PROMPT
#define RES_TYPE_FIELD					RES_FIELD_DESCRIPTION
#define RES_TYPE_ACTION					RES_UIS_ACTION_LIST
#define RES_TYPE_GRAPHICS				DRM_TYPE_ANIMATION
#define RES_TYPE_LIST_DESCR				DRM_TYPE_LIST
#endif

#define RES_TYPE_STRING         0x01
#define RES_TYPE_ACTION		    0x06
#define RES_TYPE_GRAPHICS       0x12
#define RES_TYPE_CONTENT 		0x0C
#define RES_TYPE_FIELD			0x0d    // Editable field in list
#define RES_TYPE_LIST_DESCR		0x07    // Список, меню в списке
#define RES_TYPE_INTEGER 		0x04
#define RES_TYPE_BITMAP 		0x02
#define RES_TYPE_ENUM			0x10	// перечисление возможных значений
#define RES_TYPE_ANIMATION 		0x12
#define RES_TYPE_FILE_NAME 		0x28

#define DRM_MAX_NUM_VALUES				30	/* this max value for the tagged content resource */

#define DRM_NO_OFFSET					0
#define DRM_TYPE_MIN					0x01
#define DRM_TYPE_MAX					0x29


typedef UINT32					RESOURCE_ID;



/* Структура для описания ресурса для Action-а */
typedef struct
{
	RESOURCE_ID			softkey_label;		// Надпись на софт-кнопке
	RESOURCE_ID			list_label;			// Надпись в списке действий
	INT8				softkey_priority;	// Приоритет расположения на софткнопках, отрицательый - 
											// на левый софт, положительный - на правый
	INT8				list_priority;		// Приоритет расположения в списке действий, два пункта
											// с наивысшим приоритетом располагаются на софткнопках, и дальше смотрится softkey_priority
	BOOL				isExit;				// ?? Явлется ли командой выхода из приложения
	BOOL				sendDlgDone;		// Посылать ли дополнительно ивент EV_DIALOG_DONE
	
} RES_ACTION_LIST_ITEM_T; 


#define UIS_MAX_EDIT_SOURCES		11
typedef struct
{
	RESOURCE_ID			sources_title;   /* Named string res ID for BROWSE list sub-dialog title. */
	RESOURCE_ID			menu_title;      /* Named string res ID for action menu sub-dialog title. */
	RESOURCE_ID			title;           /* Named string res ID for editor title. */
	UINT32				source_list_size;/* Number of entries in the BROWSE list. */
	RESOURCE_ID			sources[UIS_MAX_EDIT_SOURCES];  /* The entries of this array are of type
										 * UIS_CONTENT_T to be presented in a BROWSE list sub-dialog */
	UINT8				num_decimal_places;
	RESOURCE_ID			primary_icon; /* Icon used in the tab border */

} UIS_EDITOR_RESOURCE_CONTENTS_T;


typedef struct
{
	RESOURCE_ID			list_title;      /* Named string res ID for the list title. */
	RESOURCE_ID			menu_title;      /* Named string res ID for an action menu sub-dialog. */
	RESOURCE_ID			reorder_title;   /* Title when reordiing the list. */
	UINT32				num_marks;     /* Number of valid entries in marks[]. */
	RESOURCE_ID			marks[2/*UIS_MAX_NUM_MARKS*/]; /* array of possible marks */
	BOOL				show_marks_at_front;
	RESOURCE_ID			detailed_view_format;
	RESOURCE_ID			primary_icon;    /* Icon used in the tab border */
	BOOL				Categorised;    /* Flag for setting a Categorised List */

} UIS_LIST_RESOURCE_CONTENTS_T;


typedef struct
{
	INT32				tag;
	RESOURCE_ID			resource;	/* RES_PROMPT, RES_CONTENT or RES_PICTURE_BITMAP */
	
} UIS_TAGGED_CONTENT_T;


typedef struct
{
	UINT32				num_values;
	RESOURCE_ID			possible_values;	/* RES_TAGGED_CONTENT */
	RESOURCE_ID			editor_resource;	/* RES_UIS_EDITOR */

} UIS_ENUM_FIELD_RESOURCE_CONTENTS_T;



enum
{
	UIS_FIELD_BINARY_CAP_STRING = 0,
	UIS_FIELD_BINARY_CURRENCY,
	UIS_FIELD_BINARY_NUMBER,
	UIS_FIELD_CAP_STRING,
	UIS_FIELD_CLEANUP,
	UIS_FIELD_CURRENCY,
	UIS_FIELD_DATE,
	UIS_FIELD_DURATION,
	UIS_FIELD_ELAPSED_TIME,
	UIS_FIELD_ENUMERATED, // 9
	UIS_FIELD_MESSAGE,
	UIS_FIELD_METER,
	UIS_FIELD_NAME,
	UIS_FIELD_NULL_CAP_STRING,
	UIS_FIELD_NULL_MESSAGE,
	UIS_FIELD_NULL_NAME,
	UIS_FIELD_NULL_NUMBER,
	UIS_FIELD_NULL_TEXT,
	UIS_FIELD_NUMBER,
	UIS_FIELD_PHONE_NUMBER,
	UIS_FIELD_ZIP_CODE,
	UIS_FIELD_SPEED_NUM,
	UIS_FIELD_TEXT,
	UIS_FIELD_TIME_OF_DAY,
	UIS_FIELD_VOICE_NAME,
	UIS_FIELD_SEGMENT,
	UIS_FIELD_NULL_SEGMENT,
	UIS_FIELD_NOTES,
	UIS_FIELD_NULL_PHONE_NUMBER,
	UIS_FIELD_DATE_DEFINED,
	UIS_FIELD_NICKNAME,
	UIS_FIELD_NUMBER_WITHOUT_LEADING_ZERO,
	UIS_FIELD_NULL_EMAIL,
	UIS_FIELD_NULL_URL,
	UIS_FIELD_NULL_PHONE_NUMBER_STORE_SIM, 
	UIS_FIELD_BOGUS_INIT = 99
};
typedef UINT16 UIS_FIELD_TYPE_T;


typedef struct
{
	UIS_FIELD_TYPE_T	type;
	RESOURCE_ID			label;			// имя "опции"
	BOOL				view_only;
	BOOL				show_value;
	BOOL				secure;
	UINT32				maxlen;
	RESOURCE_ID			resource;		// res_type = 0x10
	UINT32				auth_event;		// 0xB
	
} FIELD_DESCR_T, FIELD_DESCR_CONTENT_T, UIS_FIELD_DESCRIPTION_RESOURCE_CONTENTS_T;



typedef enum 
{
	LNG_ENGLISH=1,
	LNG_BRITISH_ENGLISH=3,
	LNG_CHINESE_COMPLEX, 
	LNG_CHINESE_SIMPLE,
	LNG_RUSSIAN=35  // или 0x2e
	
} LANGUAGE_T;


#define TTF_TYPEFACE_NAME_LENGTH 50

typedef struct
{
	UINT8			ID;
	WCHAR			name[TTF_TYPEFACE_NAME_LENGTH];
	UINT8			*offset;
	UINT32			size;
	UINT8			minHeight;
	UINT8			maxHeight;
	UINT32			pxPerPoint;		// single float
} TTF_TYPEFACE_ATTRIB_T;

typedef struct
{
	UINT8					*typefaces_count;
	TTF_TYPEFACE_ATTRIB_T	*maintableptr;
} TTF_FONT_INFO_STRUCT_T;


typedef struct
{
	UINT8				*lookuptableptr;
	UINT8				*maintableptr;
} DRM_FONT_INFO_STRUCT_T;


#ifdef __cplusplus
extern "C" {
#endif


// Создать ресурс
UINT32 DRM_CreateResource( RESOURCE_ID      *res_id,
                           UINT32           type,       //RES_TYPE_*
                           void             *data,
                           UINT32           size );

// Получить содержимое ресурса
UINT32 DRM_GetResource( RESOURCE_ID     res_id,
                        void            *buf,
                        UINT32          size );

// Получить размер ресурса
UINT32 DRM_GetResourceLength( RESOURCE_ID res_id,  UINT32 *size );

// Изменить ресурс
UINT32 DRM_SetResource( RESOURCE_ID     res_id,
                        void            *data,
                        UINT32          size );


// Уничтожить ресурс
UINT32 DRM_ClearResource( RESOURCE_ID res_id );



// получаем текущий язык
UINT8 DRM_GetCurrentLanguage(LANGUAGE_T *LGID);
// установить язык
UINT8 DRM_SetLanguage( LANGUAGE_T LGID );

UINT8 DRM_GetFontInfo( DRM_FONT_INFO_STRUCT_T ** font_info );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
