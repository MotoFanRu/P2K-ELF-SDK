/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_RESOURCES_H
#define P2K_SDK_RESOURCES_H

#include "P2kT.h"
#include "P2kResourcesT.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ID ресурса формируется типом ресурса в качестве старшего байта и номером ресурса текущего типа
    Например, 0x01000496 = "Системная ошибка"
    Идентификаторы для строк и картинок берутся, соответственно, из ленгпака и DRM */

/*
    DRM Resource Type defination
*/
#define DRM_TYPE_INVALID                0x0
#define DRM_TYPE_PROMPT                 0x1
#define DRM_TYPE_BITMAP                 0x2
#define DRM_TYPE_COMPOSITE              0x3
#define DRM_TYPE_INTEGER                0x4
#define DRM_TYPE_BOOLEAN                0x5
#define DRM_TYPE_ALI                    0x6
#define DRM_TYPE_LIST                   0x7
#define DRM_TYPE_EDITOR                 0x8
#define DRM_TYPE_COMBO                  0x9
#define DRM_TYPE_FORM                   0xa
#define DRM_TYPE_PICKER                 0xb
#define DRM_TYPE_CONTENT                0xc
#define DRM_TYPE_FIELD                  0xd
#define DRM_TYPE_METER                  0xe
#define DRM_TYPE_NUMBER                 0xf
#define DRM_TYPE_ENUM                   0x10
#define DRM_TYPE_ANIMATION              0x12
#define DRM_TYPE_TAGGED                 0x13
#define DRM_TYPE_MMA                    0x14
#define DRM_TYPE_FONT_SIZE              0x15
#define DRM_TYPE_EDITOR_FORMATTER       0x16
#define DRM_TYPE_INCALL_FORMATTER       0x17
#define DRM_TYPE_LIST_FORMATTER         0x18
#define DRM_TYPE_METER_FORMATTER        0x19
#define DRM_TYPE_NOTICE_FORMATTER       0x1a
#define DRM_TYPE_PICKER_FORMATTER       0x1b
#define DRM_TYPE_PROGRESS_BAR_FORMATTER 0x1c
#define DRM_TYPE_WEEKVIEW_FORMATTER     0x1d
#define DRM_TYPE_CUSTOM                 0x1e
#define DRM_TYPE_COLORS_FORMATTER       0x1f
#define DRM_TYPE_MARGINS_FORMATTER      0x20
#define DRM_TYPE_INIT_TIMER             0x21
#define DRM_TYPE_COLOR_SCHEME           0x22
#define DRM_TYPE_SINGLE_FRAME_GIF       0x25
#define DRM_TYPE_FILE_NAME              0x28
#define DRM_TYPE_CUSTOMIZED_ICON        0x29

/* these are for UIS compatibility issues */
#define RES_PROMPT                      DRM_TYPE_PROMPT
#define RES_PICTURE_BITMAP              DRM_TYPE_BITMAP
#define RES_INTEGER                     DRM_TYPE_INTEGER
#define RES_BOOLEAN                     DRM_TYPE_BOOLEAN
#define RES_UIS_ACTION_LIST             DRM_TYPE_ALI
#define RES_UIS_LIST                    DRM_TYPE_LIST
#define RES_UIS_EDITOR                  DRM_TYPE_EDITOR
#define RES_UIS_COMBO_BINARY_EDITOR     DRM_TYPE_COMBO
#define RES_FORM                        DRM_TYPE_FORM
#define RES_COMPOSITE_DIALOG            DRM_TYPE_COMPOSITE
#define RES_PICKER                      DRM_TYPE_PICKER
#define RES_CONTENT                     DRM_TYPE_CONTENT
#define RES_FIELD_DESCRIPTION           DRM_TYPE_FIELD
#define RES_METER_FIELD                 DRM_TYPE_METER
#define RES_NUMBER_FIELD                DRM_TYPE_NUMBER
#define RES_ENUM_FIELD                  DRM_TYPE_ENUM
#define RES_ANIMATION_LABEL             DRM_TYPE_ANIMATION
#define RES_FILE_NAME                   DRM_TYPE_FILE_NAME
#define RES_TAGGED_CONTENT              DRM_TYPE_TAGGED

#define RES_TYPE_STRING     0x01
#define RES_TYPE_ACTION     0x06
#define RES_TYPE_GRAPHICS   0x12
#define RES_TYPE_CONTENT    0x0C
#define RES_TYPE_FIELD      0x0d  // Editable field in list
#define RES_TYPE_LIST_DESCR 0x07  // Список, меню в списке
#define RES_TYPE_INTEGER    0x04
#define RES_TYPE_BITMAP     0x02
#define RES_TYPE_ENUM       0x10  // перечисление возможных значений
#define RES_TYPE_ANIMATION  0x12
#define RES_TYPE_FILE_NAME  0x28

#define DRM_MAX_NUM_VALUES  30 /* this max value for the tagged content resource */

#define DRM_NO_OFFSET       0
#define DRM_TYPE_MIN        0x01
#define DRM_TYPE_MAX        0x29

typedef UINT32 RESOURCE_ID;

/* Структура для описания ресурса для Action-а */
typedef struct {
	RESOURCE_ID softkey_label;  // Надпись на софт-кнопке
	RESOURCE_ID list_label;     // Надпись в списке действий
	INT8 softkey_priority;  // Приоритет расположения на софткнопках, отрицательый -
	                        // на левый софт, положительный - на правый
	INT8 list_priority;  // Приоритет расположения в списке действий, два пункта
	                     // с наивысшим приоритетом располагаются на софткнопках, и дальше смотрится softkey_priority
	BOOL isExit;       // ?? Явлется ли командой выхода из приложения
	BOOL sendDlgDone;  // Посылать ли дополнительно ивент EV_DIALOG_DONE
} RES_ACTION_LIST_ITEM_T;

/*
LGID:
04 Chinese Complex
05 Chinese Simple
03 British English
01 English
*/

enum {
	LNG_ENGLISH = 1,
	LNG_BRITISH_ENGLISH = 3,
	LNG_CHINESE_COMPLEX,
	LNG_CHINESE_SIMPLE,
	LNG_RUSSIAN = 35  // или 0x2e

};
typedef UINT8 LANGUAGE_T;

#define TTF_TYPEFACE_NAME_LENGTH 50

typedef struct {
	UINT8 ID;
	WCHAR name[TTF_TYPEFACE_NAME_LENGTH];
	UINT8 *offset;
	UINT32 size;
	UINT8 minHeight;
	UINT8 maxHeight;
	UINT32 pxPerPoint;  // single float
} TTF_TYPEFACE_ATTRIB_T;

typedef struct {
	UINT8 *typefaces_count;
	TTF_TYPEFACE_ATTRIB_T *maintableptr;
} TTF_FONT_INFO_STRUCT_T;

typedef struct {
	UINT8 *lookuptableptr;
	UINT8 *maintableptr;
} DRM_FONT_INFO_STRUCT_T;

// Создать ресурс
UINT32 DRM_CreateResource(RESOURCE_ID *res_id, UINT32 res_type, void *data, UINT32 size);

// Получить содержимое ресурса
UINT32 DRM_GetResource(RESOURCE_ID res_id, void *buf, UINT32 size);

// Получить размер ресурса
UINT32 DRM_GetResourceLength(RESOURCE_ID res_id, UINT32 *size);

// Изменить ресурс
UINT32 DRM_SetResource(RESOURCE_ID res_id, void *data, UINT32 size);

// Уничтожить ресурс
UINT32 DRM_ClearResource(RESOURCE_ID res_id);

// получаем текущий язык
UINT8 DRM_GetCurrentLanguage(LANGUAGE_T *LGID);
// установить язык
UINT8 DRM_SetLanguage(LANGUAGE_T LGID);

UINT8 DRM_GetFontInfo(DRM_FONT_INFO_STRUCT_T **font_info);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_RESOURCES_H */

/** @} */ /* end of P2K_Types */
