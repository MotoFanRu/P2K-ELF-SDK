// UTF-8 w/o BOM

#ifndef CANVAS_H
#define CANVAS_H

#include <typedefs.h>
#include <utilities.h>
#include <mme.h>
#include <resources.h>
#include <uis.h>

#ifdef __cplusplus
extern "C" {
#endif



// Точка привязки для текста и картинок
// вертикальная
#define ANCHOR_BASELINE			   64 // только текст
#define ANCHOR_BOTTOM				 32
#define ANCHOR_TOP					16
#define ANCHOR_VCENTER				2  // только картинка

// горизонтальная
#define ANCHOR_RIGHT				  8
#define ANCHOR_LEFT				   4
#define ANCHOR_HCENTER				1

//  использовать знак "или". Например: ANCHOR_LEFT | ANCHOR_TOP

/*******************************/

// стиль шрифта ... font_style
#if (defined(FTR_L7E) || defined(FTR_L9))
// для векторных шрифтов
#define FONT_STYLE_PLAIN			0
#define FONT_STYLE_ITALIC			0x02
#define FONT_STYLE_BOLD				0x01
#define FONT_STYLE_BOLD_ITALIC		(FONT_STYLE_ITALIC | FONT_STYLE_BOLD)
#define FONT_STYLE_UNDERLINE		0x04
#define FONT_STYLE_STRIKETHROUGH	0x08
#define FONT_STYLE_REVERSEVIDEO     0x10
#define FONT_STYLE_NORMALVIDEO		0x20

#else

#define FONT_STYLE_PLAIN		0xFF
#define FONT_STYLE_ITALIC		0x01
#define FONT_STYLE_BOLD			0x02
#define FONT_STYLE_BOLD_ITALIC	(FONT_STYLE_ITALIC | FONT_STYLE_BOLD)

#endif

/*******************************/

typedef struct {

     UINT8 			font_id;
     UINT8		 	font_style;
     UINT16    		point_size;
     UINT16    		frac_point_size;

} FONT_ATTRIB_T; // для векторных шрифтов


typedef struct
{
	UINT8 *buf;	 // буфер для рисования. Должен быть NULL
	UINT16 w;	   // ширина
	UINT16 h;	   // высота
} DRAWING_BUFFER_T;

typedef struct
{
	UINT8 red;
	UINT8 green;
	UINT8 blue;
	UINT8 transparent; // прозрачность
} COLOR_T;

#define COLOR_RED		(COLOR_T)(0xFF000000)
#define COLOR_GREEN		(COLOR_T)(0x00FF0000)
#define COLOR_BLUE		(COLOR_T)(0x0000FF00)
#define COLOR_BLACK		(COLOR_T)(0x00000000)
#define COLOR_TRANSPARENT	(COLOR_T)(0x000000FF)

#ifdef WIN32
	#ifdef RGB
		#undef RGB
	#endif
#endif

#define RGB(r,g,b)		(COLOR_T)((BYTE)(r)<<24 | (BYTE)(g)<<16 | (BYTE)(b)<<8)
#define RGBT(r,g,b,t)	(COLOR_T)((BYTE)(r)<<24 | (BYTE)(g)<<16 | (BYTE)(b)<<8 | (BYTE)(t))
#define RGB_getr(c)		(BYTE)((UINT32)(c) >> 24 & 0xFF)// RED
#define RGB_getg(c)		(BYTE)((UINT32)(c) >> 16 & 0xFF)// GREEN
#define RGB_getb(c)		(BYTE)((UINT32)(c) >>  8 & 0xFF)// BLUE
#define RGB_gett(c)		(BYTE)((UINT32)(c)	   & 0xFF)// TRANSPARENT

typedef struct
{
	INT16 x;   // горизонтальная координата x
	INT16 y;   // вертикальная координата
} GRAPHIC_POINT_T;

typedef struct
{
	GRAPHIC_POINT_T ulc;  // верхний левый угол
	GRAPHIC_POINT_T lrc;  // нижний правый угол
} GRAPHIC_REGION_T;



#ifdef EMUELF
	#define AlphaBlend EMUAlphaBlend
#endif

enum  // drawOp
{
	AlphaBlend,  // прозрачность белым
	Copy,
	NotUsed1,
	Invert,
	NotCopy,
	Resize,
	Xor,
	NotUsed2,
	Move
};

enum  //тип картинки ... picture_type
{
	DRM_PICTURE_TYPE,
	IMAGE_PATH_TYPE,
	STORED_IN_MEMORY_TYPE,
	FILE_HANDLE_TYPE,
	IMAGE_PATH_WITH_OFFSET
};

typedef WCHAR *IMAGE_PATH_T; // путь к картинке

typedef struct
{
	const void		*resourcePicturePointerValue;  // Местоположение указателя в памяти
	UINT32			imageSize;				   // размер изображения
} PICTURE_POINTER_T;

typedef struct
{
	FILE_HANDLE_T	fileHandle;
	UINT32			imageOffset;
	UINT32			imageSize;
} UIS_FILE_HANDLE_T;

typedef struct
{
	IMAGE_PATH_T	PathValue;
	MIME_TYPE_T		mimeFormat; // тип картинки
	UINT32			imageOffset;
	UINT32			imageSize;
} FILE_PATH_WITH_OFFSET_T;

typedef union
{
	RESOURCE_ID					DrmValue;
	IMAGE_PATH_T				PathValue;
	PICTURE_POINTER_T			PointerValue;
	UIS_FILE_HANDLE_T			FileHandleValue;
	FILE_PATH_WITH_OFFSET_T 	PathWithOffsetValue;
} PICTURE_TYPE_UNION_T;

typedef UINT8 CANVAS_IMAGE_HANDLE_T;

typedef struct
{
	UINT16			height;
	UINT16			width;
} GRAPHIC_METRIC_T;

typedef struct
{
	GRAPHIC_REGION_T	offset_region;
	UINT8				*background_buffer;
	UINT16				width;
	UINT16				height;
} IMAGE_BACKGROUND_BUFFER;

typedef struct
{
	COLOR_T			begin_color;
	COLOR_T			end_color;
} GRADIENT_T;


typedef enum
{
	IA_NO_ATTRIBUTES = 0, // без аттрибутов
	IA_FRAME_COMPLETED, // вызвать  ивент по завершению фрейма
	IA_CYCLE_COMPLETED, // вызвать  ивент по завершению цикла
	IA_ANIMATION_COMPLETED, // вызвать  ивент по завершению анимации
	IA_STILL_IMAGE, // первый кадр анимации
	IA_BACKGROUND_TRANSPARENT_IMAGE, // IMAGE_BACKGROUND_BUFFER
	IA_BACKGROUND_TRANSPARENT_COLOR, // COLOR_T
	IA_BACKGROUND_TRANSPARENT_GRADIENT, // GRADIENT_T

	IA_BACKGROUND_TRANSPARENT_TILED_IMAGE,
	IA_BACKGROUND_WALLPAPER_NEEDED,

	IA_START_ANIMATION=10, // анимация будет стартовать с первого кадра
	IA_BACKGROUND_EXCLUDING_IMAGE, // COLOR_T

	IA_NO_PAINT_WHITE=13,  // не рисовать белый
	IA_NO_PAINT_WHITE_WITH_CROSS,
	IA_PAINT_WHITE_AND_PAINT_WHITE_WITH_CROSS,
	IA_NO_OF_CYCLES_OF_ANIMATION, // UINT8 номер цикла
	IA_DECODE_WALLPAPER_GRAYSCALE,
	IA_PRESERVE_BACKGROUND_GRAYSCALE,

	IA_OBEY_TRANSPARENCY=23  // учитывать прозрачность

} IMAGE_ATTRIBUTE_TYPE_T;


/*************************
 Создание диалога Canvas
**************************/

//создать Canvas
UIS_DIALOG_T UIS_CreateColorCanvas( SU_PORT_T  *port,
									DRAWING_BUFFER_T * drawing_buffer,
									BOOL status_line_area ); // true - не показывать статус-строку, false - показывать

/*	Цвета поумолчанию
BackgroundColor - white
FillColor  - white
ForegroundColor  - blue
*/

// с картинкой
UIS_DIALOG_T UIS_CreateColorCanvasWithWallpaper (  SU_PORT_T  *port,
												   DRAWING_BUFFER_T * drawing_buffer,
												   BOOL use_status_line_area,
												   BOOL wallpaper_flag );

/**************************
  Графические примитивы
***************************/

// рисует точку
void UIS_CanvasDrawPixel(GRAPHIC_POINT_T point, UIS_DIALOG_T handle);

//  рисует  линию
void UIS_CanvasDrawLine(GRAPHIC_POINT_T begin,
						GRAPHIC_POINT_T end,
						UIS_DIALOG_T handle);

// рисует закрашенный прямоугольник
void UIS_CanvasFillRect(GRAPHIC_REGION_T region,
						UIS_DIALOG_T handle);

//  рисует прямоугольник
void UIS_CanvasDrawRect(GRAPHIC_REGION_T region,
						BOOL fill, //  заливка
						UIS_DIALOG_T handle);

// рисует закруглённый прямоугольник
void UIS_CanvasDrawRoundRect(GRAPHIC_REGION_T region,
							UINT16 arcW, // горизонтальный диаметр скругления
							UINT16 arcH, // вертикальный диаметр скругления
							BOOL fill,
							UIS_DIALOG_T handle);

// рисует дугу окружности или эллипса
void UIS_CanvasDrawArc(GRAPHIC_REGION_T region,
						UINT16 startAngle,	// начальный угол.
						UINT16 arcAngle,	  // конечный угол
						BOOL fill,
						UIS_DIALOG_T handle);

// рисует многоугольник
void UIS_CanvasDrawPoly(UINT16 number_of_vertices,
						GRAPHIC_POINT_T  *array,
						BOOL fill,
						UIS_DIALOG_T handle);
// рисует точку
void UIS_CanvasDrawPixel ( GRAPHIC_POINT_T position,
						   UIS_DIALOG_T handle );

/***************
  Text
****************/

// рисует строку
void UIS_CanvasDrawColorText( WCHAR *str,
							  UINT16 offset,
							  UINT16 str_len,  			   // длинна строки
							  GRAPHIC_POINT_T anchor_point, // координаты точки привязки
							  UINT16 anchor_value,		   // значение точки привязки. Например: ANCHOR_LEFT | ANCHOR_TOP
							  UIS_DIALOG_T handle);

UINT32 UIS_CanvasDrawText ( UINT8* draw_buf,
							WCHAR *str,
							GRAPHIC_REGION_T region,
							UINT8 font_id,
							BOOL unk1,
							COLOR_T color,
							void *unk2 );

#if (defined(FTR_L7E) || defined(FTR_L9))
// установка шрифта, для векторных шрифтов
UINT32 UIS_CanvasSetFontAttrib( FONT_ATTRIB_T fontattrib,UIS_DIALOG_T handle);
#endif



// получение текущего шрифта
#if (defined(FTR_L7E) || defined(FTR_L9))
UINT32 UIS_CanvasGetCurrentFonts ( FONT_ATTRIB_T *working_font, FONT_ATTRIB_T *softkey_font );
#else
UINT32 UIS_CanvasGetCurrentFonts ( UINT8 *working_font, UINT8 *softkey_font );
#endif

//возвращает стиль шрифта
UINT8 UIS_CanvasGetFontStyle(UIS_DIALOG_T handle);
UINT8 UIS_CanvasGetFontId(UIS_DIALOG_T dialog);
// задает стиль	шрифта
UINT32 UIS_CanvasSetFontStyle(UINT8 font_style, UIS_DIALOG_T handle);
// устанавливает шрифт canvas'а, font_id можно взять из langpack'а (CG4), значение 0x01 -- стандартный шрифт
UINT32 UIS_CanvasSetFont(UINT8 font_id, UIS_DIALOG_T handle);

// получает размер символа
#if (defined(FTR_L7E) || defined(FTR_L9))
UINT32 UIS_CanvasGetCharacterSize (  WCHAR* str,
		                             UINT8 position,
		                             GRAPHIC_METRIC_T * char_size,
		                             FONT_ATTRIB_T font_id );
#else
UINT32 UIS_CanvasGetCharacterSize (  WCHAR* str,
		                             UINT8 position,
		                             GRAPHIC_METRIC_T * char_size,
		                             UINT8 font_id );	
#endif

// возвращает размер строки
#if (defined(FTR_L7E) || defined(FTR_L9))
UINT32 UIS_CanvasGetStringSize( WCHAR * str,
								GRAPHIC_METRIC_T * string_size, // размер строки
								FONT_ATTRIB_T font_id );	
#else
UINT32 UIS_CanvasGetStringSize( WCHAR * str,
								GRAPHIC_METRIC_T * string_size, // размер строки
								UINT8 font_id );
#endif


// размер шрифта
#if (defined(FTR_L7E) || defined(FTR_L9))
UINT32 UIS_CanvasGetFontSize ( FONT_ATTRIB_T font_id, GRAPHIC_METRIC_T * font_size);
#else
UINT32 UIS_CanvasGetFontSize ( UINT8 font_id, GRAPHIC_METRIC_T * font_size);
#endif


/*****************
 Image
**************/

// рисует картику
UINT32 UIS_CanvasDrawImage(UINT8 picture_type,			// тип. откуда будем брать картинку. DRM, файл....
						   PICTURE_TYPE_UNION_T picture_data,
						   GRAPHIC_POINT_T anchor_point,
						   UINT16 anchor_value,
						   UINT8 drawOp,
						   UIS_DIALOG_T handle);

UINT32 UIS_CanvasDrawWallpaper(CANVAS_IMAGE_HANDLE_T image_handle,
							   GRAPHIC_REGION_T src_area,
							   GRAPHIC_REGION_T dest_region,
							   UINT8 drawOp,
							   WALLPAPER_LAYOUT_T layout,
							   UIS_DIALOG_T handle);

BOOL UIS_CanvasGetWallpaperFlag(UIS_DIALOG_T dialog);
void UIS_CanvasSetWallpaperFlag(UIS_DIALOG_T handle, BOOL wallpaperFlag);

UINT32 UIS_CanvasDrawColorBitmap( void*  picture_bytes,  // первый байт - ширина, второй байт высота
																	   // остальное 8-bpp(RGB332)  или  16-bpp(RGB565)
								GRAPHIC_POINT_T  ulc_anchor_point,
								UIS_DIALOG_T  handle );
// из DRM
UINT32 UIS_CanvasDrawPicture ( 	UINT8* draw_buf,
								RESOURCE_ID picture,
								GRAPHIC_REGION_T region,
								BOOL unk0,
								COLOR_T foreground,
								COLOR_T fill );

UINT32 UIS_CanvasDrawBitmap(UINT8* p_buf, const BYTE* p_bitmap, GRAPHIC_REGION_T region, BOOL b_reverse);

UINT32 UIS_CanvasFillRegion(UINT8 *	p_buf,		// UIS_DRAWING_BUFFER_T::p_buf
					GRAPHIC_REGION_T	region,
					UINT32		unk_0 );	// =0

// открывает картинку
CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImage(UINT8 picture_type,
										  PICTURE_TYPE_UNION_T picture_data);

// открывает картинку	 и получает её размер
CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImageAndGetSize(UINT8 picture_type,
													PICTURE_TYPE_UNION_T picture_data,
													GRAPHIC_POINT_T *attributes);
// закрывает картинку
UINT32 UIS_CanvasCloseImage(CANVAS_IMAGE_HANDLE_T image_handle);


// изменение размера
UINT32 UIS_CanvasResizeImage(CANVAS_IMAGE_HANDLE_T image_handle,
							 UINT16 percent, // zoom в процентах
							 GRAPHIC_REGION_T src_area, // Исходный размер изображения
							 GRAPHIC_REGION_T dest_region,// новый размер
							 UINT8 drawOp,
							 UIS_DIALOG_T handle);
// возвращает размер изображения
GRAPHIC_POINT_T UIS_CanvasGetImageSize(UINT8 picture_type,
									   PICTURE_TYPE_UNION_T picture_data);
// из  DRM
UINT32 UIS_CanvasGetPictureSize ( RESOURCE_ID picture,
								  GRAPHIC_METRIC_T *pic_size );

// устанавливает аттрибут
UINT32 UIS_CanvasSetImageAttribute(CANVAS_IMAGE_HANDLE_T image_handle,
									IMAGE_ATTRIBUTE_TYPE_T attribute_type,
									void* attribute_value);
// получает аттрибут
UINT32 UIS_CanvasGetImageAttribute(CANVAS_IMAGE_HANDLE_T image_handle,
									IMAGE_ATTRIBUTE_TYPE_T attribute_type,
									void* attribute_value);
// вставляет изображение в буфер
UINT32 UIS_CanvasRenderImageToBuffer(CANVAS_IMAGE_HANDLE_T image_handle,
									 GRAPHIC_POINT_T image_size,
									 GRAPHIC_POINT_T anchorpoint,
									 UINT16 anchor_value,
									 GRAPHIC_REGION_T * image_region,
									 UINT8 drawOp,
									 UIS_DIALOG_T handle);

// преобразует картинку в буфер pointer
UINT32 UIS_CanvasDecodeImageToPointer(CANVAS_IMAGE_HANDLE_T image_handle, void* pointer);

// вставляет буфер pointer
UINT32 UIS_CanvasInsert(UIS_DIALOG_T handle,void* pointer, GRAPHIC_POINT_T point);

// перемещает регион
UINT32 UIS_CanvasMoveRegion(GRAPHIC_REGION_T source_region,
							GRAPHIC_REGION_T dest_region,
							UIS_DIALOG_T handle);

// запустить анимацию
BOOL UIS_CanvasIsImageAnimated(CANVAS_IMAGE_HANDLE_T image_handle);
// приостановить анимацию
BOOL UIS_CanvasPauseAnimation(CANVAS_IMAGE_HANDLE_T image_handle);

/***************
standart GUI
****************/

// TitleBar
void UIS_CanvasDrawTitleBar(WCHAR *str,
							BOOL  unk0,  // = false
							UINT8 type_titlebar, // в типах никакой разнецы не увидел :(
							BOOL  left_icon, // иконка слева
							BOOL  right_icon,  // иконка справа
							UIS_DIALOG_T handle);
// TitleBar с иконкой
void UIS_CanvasDrawTitleBarWithIcon(WCHAR *str,
									RESOURCE_ID picture,
									BOOL unk0, // = false
									UINT8 type_titlebar, // 0 - обычный, 1 - расширенный, 2 - editor
									BOOL  left_icon, // иконка слева
									BOOL  right_icon,  // иконка справа
									UIS_DIALOG_T handle,
									UINT16 slide,// номер слайда // =0
									UINT16 slides); // кол-во слайдов

// SoftKey
void UIS_CanvasDrawColorSoftkey(WCHAR *str,
								UINT8 type_softkey, // 0 - menu, 1 - left, 2 - right
								BOOL  pressed,
								BOOL  unk0, //  = false
								UIS_DIALOG_T handle);


// ScrollBar
void UIS_CanvasDrawColorScrollBar(UINT16 count_items, // кол-во пунктов
								  UINT16 visible, // показанно
								  UINT16 count_items_passes, // пройдено
								  BOOL unk0, // = false
								  UIS_DIALOG_T handle);

// получить область занимаемую софт кнопками
GRAPHIC_REGION_T UIS_CanvasGetMainDisplaySoftkeyArea(void);

UINT8 UIS_CanvasGetSoftkeyRegion (GRAPHIC_REGION_T *region,
								  UINT8 type_softkey,
								  BOOL use_soft_icon_area );

UINT8 UIS_CanvasGetSoftkeyTextRegion (GRAPHIC_REGION_T *region,
									  UINT8 type_softkey,
									  BOOL use_soft_icon_area );


/*******************************/

// задает цвет заливки
void UIS_CanvasSetFillColor(COLOR_T color);
// возвращает цвет заливки
COLOR_T UIS_CanvasGetFillColor(void);


// задает цвет для линий, прямоугольников, текста.....
void UIS_CanvasSetForegroundColor(COLOR_T color);
// возвращает цвет для линий, прямоугольников, текста.....
COLOR_T UIS_CanvasGetForegroundColor(void);

// цвет фона
void UIS_CanvasSetBackgroundColor(COLOR_T color);
COLOR_T UIS_CanvasGetBackgroundColor(void);


// ширина линии
void UIS_CanvasSetLineWidth(UINT16 width);
UINT16 UIS_CanvasGetLineWidth(void);


/*******************************/

// обновление экрана
void UIS_CanvasRefreshDisplayRegion(UIS_DIALOG_T handle, GRAPHIC_REGION_T region);
void UIS_CanvasRefreshDisplayBuffer(UIS_DIALOG_T handle);

// установка буфера по умолчанию
void UIS_CanvasSetDefaultDisplayBuffer(UINT8* new_draw_buf, UIS_DIALOG_T handle);
// получение буфера по умолчанию
UINT8* UIS_CanvasGetDefaultDisplayBuffer(UIS_DIALOG_T handle);

/*******************************/

//возвращает  возможную глубину цвета в битах
UINT32 UIS_CanvasGetColorDepth (void);

// возвращает размер дисплея
GRAPHIC_POINT_T UIS_CanvasGetDisplaySize(void);

// включает выключает подсветку
UINT32 UIS_CanvasSetBackLight(BOOL state);

/**********************/
enum
{
	WHOLE_DISPLAY_AREA,
	NORMAL_DISPLAY_AREA,
	COMMON_DISPLAY_AREA,
	SOFTKEY_AREA,
	STATUS_LINE_AREA,
	TITLE_BAR_AREA
};
typedef UINT32 WORKING_AREA_T;

// получение размеров области
UINT8 UIS_CanvasGetWorkingArea (   GRAPHIC_REGION_T *working_area,
								   UINT8 *count_lines,
								   UINT8 *chars_on_line,
								   WORKING_AREA_T areas,
								   BOOL use_status_line_area,
								   UINT8 font_id );
/*******************
   Рисуем на экране
********************/

// получает размер картинки-ресурса
void utility_get_image_size(RESOURCE_ID res_id, UINT16 *width, UINT16 *height);
// рисует картинку на рабочем столе. Gif или bitmap
UINT32 utility_draw_image( RESOURCE_ID res_id,
							GRAPHIC_REGION_T region,
							void * p_attr, // = NULL
							void * drawBuf ); // = NULL


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
