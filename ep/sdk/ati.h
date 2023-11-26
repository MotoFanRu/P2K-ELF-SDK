// UTF-8 w/o BOM

#ifndef ATI_H
#define ATI_H

#include <typedefs.h>


typedef UINT32 AHIDEVCONTEXT_T;
typedef UINT32 AHISURFACE_T;
typedef UINT32 AHIDEVICE_T;

enum AHIPIXFMT_ENUM
{
	AHIFMT_1BPP = 0,
	AHIFMT_4BPP,
	AHIFMT_8BPP,
	AHIFMT_16BPP_444,
	AHIFMT_16BPP_555,
	AHIFMT_16BPP_565
};
typedef UINT32 AHIPIXFMT_T;

typedef struct
{
	INT32 x, y;

} AHIPOINT_T;

typedef struct
{
	INT32 x1, y1, x2, y2;

} AHIRECT_T;

enum AHICMP_ENUM
{
	AHICMP_ALWAYS = 0,
	AHICMP_NEVER,
	AHICMP_EQUAL,
	AHICMP_NOTEQUAL

};
typedef UINT32 AHICMP_T;

typedef struct
{
	UINT32			key;
	UINT32			mask;
	AHICMP_T		cmp;

} AHICOLOROP_T;

typedef struct
{
	UINT8			stridex, stridey;
	UINT8			w, h;
	void			*image;

} AHICHAR_T;

typedef struct
{
	UINT32			width;
	UINT32			height;
	void			*image;
	INT32			stride;
	AHIPIXFMT_T		format;

} AHIBITMAP_T;

typedef struct
{
	UINT32			width;
	UINT32			height;
	UINT32			offset;
	INT32			stride;
	UINT32			byteSize;
	AHIPIXFMT_T		pixFormat;
	UINT32			numPlanes;

} AHISURFINFO_T;

typedef enum
{
	AHIROT_0 = 0,
	AHIROT_90,
	AHIROT_180,
	AHIROT_270,

	AHIROT_DUMMY = 0x7FFFFFFF

} AHIROTATE_T;

typedef enum
{
	AHIBLEND_SRC_PLUS_DST = 0,
	AHIBLEND_SRC_MINUS_DST,
	AHIBLEND_DST_MINUS_SRC,
	AHIBLEND_SRC_MINUS_ALPHA,
	AHIBLEND_SRC_SHR1,
	AHIBLEND_DST_PLUS_SRC_DIV2

} AHIBLEND_T;

typedef enum
{
	AHIMIRR_NO = 0,
	AHIMIRR_VERTICAL,
	AHIMIRR_HORIZONTAL,
	AHIMIRR_VER_HOR,

	AHIMIRR_DUMMY = 0x7FFFFFFF

} AHIMIRROR_T;

typedef struct
{
	char		drvName[80]; // driver name
	char		drvVer[80];	// driver version
	UINT32		swRevision;
	UINT32		chipId;
	UINT32		revisionId;
	UINT32		totalMemory;
	UINT32		cpuBusInterfaceMode;
	UINT32		internalMemSize;
	UINT32		externalMemSize;
	UINT32		caps1;
	UINT32		caps2;
	UINT8		pad[136];

} AHIDRVINFO_T; //sizeof(AHIDRVINFO_T) == 0x14C

typedef struct {
	UINT32 size;
	BOOL sync;
	AHIRECT_T rect;
} AHIUPDATEPARAMS_T;

typedef struct {
	AHIPOINT_T size;
	AHIPIXFMT_T pixel_format;
	UINT32 frequency;
	AHIROTATE_T rotation;
	AHIMIRROR_T mirror;
} AHIDISPMODE_T;

typedef enum {
	DISPLAY_OFF,
	DISPLAY_ON
} AHIDISPSTATE_T;

/* ----------------------------------- Работа с контекстами ----------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* Возвращает информацию об AHI-устройстве под номером index */
UINT32 AhiDevEnum( AHIDEVICE_T		*dev,		// result
                   AHIDRVINFO_T		*drvInfo,	// result
                   UINT32			index );


/* Создаёт новый контекст для указанного устройства
		dev - параметр, возвращённый AhiDevEnum
		caller - строковый идентификатор пользователя контекста */
UINT32 AhiDevOpen( AHIDEVCONTEXT_T	*devCx,		// result
                   AHIDEVICE_T		dev,
                   const char		*caller,
                   UINT32			flags );


/* Закрывает контекст */
UINT32 AhiDevClose( AHIDEVCONTEXT_T devCx );




/* ----------------------------------- Работа с поверхностями ----------------------------------- */


/* Возвращает размер самой большой поверхности, которую можно выделить, в пикселах */
UINT32 AhiSurfGetLargestFreeBlockSize( AHIDEVCONTEXT_T		devCx,
                                       AHIPIXFMT_T			pixFormat,
                                       UINT32				*size,		// result
                                       UINT32				*align,		// result
                                       UINT32				flags );


/* Выделяет поверхность из видеопамяти
		size - требуемый размер поверхности
		Если flags == 2, то формат будет использован такой же, как у
		отображаемой поверхности. */
UINT32 AhiSurfAlloc( AHIDEVCONTEXT_T	devCx,
                     AHISURFACE_T		*surf,	// result
                     AHIPOINT_T			*size,
                     AHIPIXFMT_T		pixFormat,
                     UINT32				flags );

#define AHIFLAG_DISPLAYPIXFMT				0x00000002
#define AHIFLAG_SYSMEMORY					0x00000008
#define AHIFLAG_EXTMEMORY					0x00000020
#define AHIFLAG_INTMEMORY					0x00000040

/* Освобождает ранее выделенную поверхность */
UINT32 AhiSurfFree( AHIDEVCONTEXT_T		devCx,
                    AHISURFACE_T		surf   );


/* Перевыделяет ранее выделенную поверхность (или изменяет параметры) */
UINT32 AhiSurfReuse( AHIDEVCONTEXT_T	devCx,
                     AHISURFACE_T		*surf,
                     AHISURFACE_T		surfReuse,
                     AHIPOINT_T			*size,
                     AHIPIXFMT_T		pixFormat,
                     UINT32				flags );


/* Возвращает информацию о поверхности */
UINT32 AhiSurfInfo( AHIDEVCONTEXT_T		devCx,
                    AHISURFACE_T		surf,
                    AHISURFINFO_T		*info );


/* Возвращает текущую экранную поверхность */
UINT32 AhiDispSurfGet( AHIDEVCONTEXT_T  devCx,
                       AHISURFACE_T     *surf );	// result


/* Устанавливает текущую экранную поверхность */
UINT32 AhiDispSurfSet( AHIDEVCONTEXT_T	devCx,
                       AHISURFACE_T		surf,
                       UINT32			flags );



/* Копирует содержимое поверхности из(в) системной памяти
		Если flags == 1, то копируется из поверхности в bitmap */
UINT32 AhiSurfCopy( AHIDEVCONTEXT_T		devCx,
                    AHISURFACE_T		surf,
                    AHIBITMAP_T			*bitmap,
                    AHIRECT_T			*dstRect,
                    AHIPOINT_T			*srcPt,
                    UINT32				param5,
                    UINT32				flags );

/* IDK, but seems it working. */
#define AHIFLAG_COPYTO 0x00000000
#define AHIFLAG_COPYFROM 0x00000001

/* ----------------------------------- Установка параметров рисования ----------------------------------- */


/* Устанавливает текущий цвет переднего плана (для битовых масок) */
UINT32 AhiDrawFgColorSet( AHIDEVCONTEXT_T	devCx,
                          UINT32			color ); // RGB565 @ E398


/* Устанавливает текущий цвет заднего плана (для битовых масок) */
UINT32 AhiDrawBgColorSet( AHIDEVCONTEXT_T	devCx,
                          UINT32			color );


/* Устанавливает текущий цвет переднего плана кисти */
UINT32 AhiDrawBrushFgColorSet( AHIDEVCONTEXT_T	devCx,
                               UINT32			color );


/* Устанавливает текущий цвет заднего плана кисти */
UINT32 AhiDrawBrushBgColorSet( AHIDEVCONTEXT_T	devCx,
                               UINT32			color );


/* Устанавливает текущие параметры кисти:
		pattern - указатель на шаблон для кисти (или NULL)
		srcPt	- смещение в шаблоне */
UINT32 AhiDrawBrushSet( AHIDEVCONTEXT_T		devCx,
                        AHIBITMAP_T			*pattern,
                        AHIPOINT_T			*srcPt,
                        UINT32				param3,
                        UINT32				flags );
#define AHIFLAG_BRUSH_TRANSPARENT		1
#define AHIFLAG_BRUSH_SOLID				2


/* Устанавливает текущую поверхность-источник (source)  */
UINT32 AhiDrawSurfSrcSet( AHIDEVCONTEXT_T	devCx,
                          AHISURFACE_T		surf,
                          UINT32			flags );


/* Устанавливает текущую поверхность-приёмник (destination)  */
UINT32 AhiDrawSurfDstSet( AHIDEVCONTEXT_T devCx,
                          AHISURFACE_T    surf,
                          UINT32          flags );


/* Устанавливает область вырезания для поверхности-источника.
		Если clipRect == NULL, то область сбрасывается до всего экрана */
UINT32 AhiDrawClipSrcSet( AHIDEVCONTEXT_T	devCx,
                          AHIRECT_T			*clipRect );


/* Устанавливает область вырезания для поверхности-приёмника */
UINT32 AhiDrawClipDstSet( AHIDEVCONTEXT_T	devCx,
                          AHIRECT_T			*clipRect );


/* Устанавливает растровую операцию (ROP). Используйте макрос AHIROP3
		для формирования параметра.
		За описаним ROP, обратитесь по ссылкам:
		http://msdn.microsoft.com/en-us/library/dd145130(VS.85).aspx
		http://www.svgopen.org/2003/papers/RasterOperationsUsingFilterElements/index.html */
UINT32 AhiDrawRopSet( AHIDEVCONTEXT_T	devCx,
                      UINT32			rop );
#define AHIROP_BLACKNESS			0x00
#define AHIROP_NOTSRCERASE			0x11
#define AHIROP_NOTSRCCOPY			0x33
#define AHIROP_SRCERASE				0x44
#define AHIROP_DSTINVERT			0x55
#define AHIROP_PATINVERT			0x5A
#define AHIROP_SRCINVERT			0x66
#define AHIROP_SRCAND				0x88
#define AHIROP_MERGEPAINT			0xBB
#define AHIROP_MERGECOPY			0xC0
#define AHIROP_SRCCOPY				0xCC
#define AHIROP_SRCPAINT				0xEE
#define AHIROP_PATCOPY				0xF0
#define AHIROP_PATPAINT				0xFB
#define AHIROP_WHITENESS			0xFF

#define AHIROP3(_rop) ( (_rop) | ((_rop)<<8) )


/* Устанавливает глобальный уровень прозрачности для AhiDrawAlphaBlt.
		alpha задаётся поцветно, RGB888 */
UINT32 AhiDrawAlphaSet( AHIDEVCONTEXT_T		devCx,
                        UINT32				alpha );
#define AHIALPHA(_level) ( (_level) | ((_level)<<8) | ((_level)<<16) )




/* ----------------------------------- Функции рисования ----------------------------------- */


/* Рисует последовательность линий.
		points - указатель на массив точек
		count - количество точек
		Если flags == 0, то строится набор линий, последовательно соединяющих все точки
		массива ("лента")
		Если flags == 1, то массив задаёт начальную и конечную точку каждой линии
		( count должен быть чётным ) */
UINT32 AhiDrawLines( AHIDEVCONTEXT_T  devCx,
                     AHIPOINT_T       *pointArr,
                     UINT32           count,
                     UINT32           flags );
#define AHIFLAG_LINE_STRIP				0
#define AHIFLAG_LINE_SEPARATE			1


/* Рисует набор прямоугольных областей */
UINT32 AhiDrawSpans( AHIDEVCONTEXT_T  devCx,
                     AHIRECT_T        *rectArr,
                     UINT32           count,
                     UINT32           flags );


/* Копирует изображение с поверхности-источника на приёмник, с учётом ROP */
UINT32 AhiDrawBitBlt( AHIDEVCONTEXT_T	devCx,
                      AHIRECT_T			*dstRect,
                      AHIPOINT_T		*srcPt );


/* То же, что AhiDrawBitBlt, только с массивом областей */
UINT32 AhiDrawBitBltMulti( AHIDEVCONTEXT_T		devCx,
                           AHIRECT_T			*dstRectArr,
                           AHIPOINT_T			*srcPtArr,
                           UINT32				count );


/* Копирует изображение из системной памяти на поверхность-приёмник
		palette - указатель на палитру
		Если flags == 1, то у битовых изображений "0" (фон) будет прозрачным,
		если flags == 2, то прозрачная "1" */
UINT32 AhiDrawBitmapBlt( AHIDEVCONTEXT_T	devCx,
                         AHIRECT_T			*dstRect,
                         AHIPOINT_T			*srcPt,
                         AHIBITMAP_T		*bitmap,
                         void				*palette,
                         UINT32				flags );


/* Копирует изображение с поверхности-источника на приёмник с использованием битовой маски
		maskPt - начальная точка на маске
		mask - данные маски (1bpp)
		maskStride - количество байт в строке маски */
UINT32 AhiDrawMaskBlt( AHIDEVCONTEXT_T	devCx,
                       AHIRECT_T		*dstRect,
                       AHIPOINT_T		*srcPt,
                       AHIPOINT_T		*maskPt,
                       void				*mask,
                       UINT32			maskStride );

/* Копирует изображение с поворотом и отражением */
UINT32 AhiDrawRotateBlt( AHIDEVCONTEXT_T	devCx,
                         AHIRECT_T			*dstRect,
                         AHIPOINT_T			*srcPt,
                         AHIROTATE_T		rotation,
                         AHIMIRROR_T		mirroring,
                         UINT32				flags );


/* Копирует изображение с использованием сравнения цветов
		Принцип действия AHICOLOROP_T: if( cmp(color & mask, key) ) draw(); */
UINT32 AhiDrawTransBlt( AHIDEVCONTEXT_T		devCx,
                        AHIRECT_T			*dstRect,
                        AHIPOINT_T			*srcPt,
                        AHICOLOROP_T		*dstCop,
                        AHICOLOROP_T		*srcCop );


/* Функция объединяет возможности AhiDrawRotateBlt и AhiDrawTransBlt */
UINT32 AhiDrawSpriteBlt( AHIDEVCONTEXT_T	devCx,
                         AHIRECT_T			*dstRect,
                         AHIPOINT_T			*srcPt,
                         AHICOLOROP_T		*dstCop,
                         AHICOLOROP_T		*srcCop,
                         AHIROTATE_T		rotation,
                         AHIMIRROR_T 		mirroring,
                         UINT32				flags );


/* Копирует изображение с поверхности-источника на приёмник с альфа-смешиванием.
		bitmap - задаёт карту прозрачности (4bpp)
		Если flags == 1, то будет использовано значение прозрачности, задаваемое
		AhiDrawAlphaSet */
UINT32 AhiDrawAlphaBlt( AHIDEVCONTEXT_T		devCx,
                        AHIRECT_T			*dstRect,
                        AHIPOINT_T			*srcPt,
                        AHIPOINT_T			*bmpPt,
                        AHIBITMAP_T			*bitmap,
                        UINT32				flags );
#define AHIFLAG_ALPHA_SOLID				1


/* Копирует изображение с растяжением(сжатием)
	flags == 1..4 управляет качеством */
UINT32 AhiDrawStretchBlt( AHIDEVCONTEXT_T	devCx,
                          AHIRECT_T			*dstRect,
                          AHIRECT_T			*srcRect,
                          UINT32			flags );

#define AHIFLAG_STRETCHFAST     0x00000001
#define AHIFLAG_STRETCHPIXEXACT 0x00000002
#define AHIFLAG_STRETCHSMOOTH   0x00000004

/* Копирует изображение со смешиванием */
UINT32 AhiDrawBlendBlt( AHIDEVCONTEXT_T		devCx,
                        AHIRECT_T			*dstRect,
                        AHIPOINT_T			*srcPt,
                        AHIBLEND_T			blendOp,
                        UINT32				flags );


/* Копирует изображение с размножением */
UINT32 AhiDrawTileBlt( AHIDEVCONTEXT_T		devCx,
                       AHIRECT_T			*dstRect,
                       AHIPOINT_T			*srcPt,		// align
                       UINT32				flags );


/* Рисует набор символов из charsArr, count штук */
UINT32 AhiDrawChar( AHIDEVCONTEXT_T		devCx,
                    AHIPOINT_T			*dstPt,
                    AHICHAR_T			*charsArr,
                    UINT32				count,
                    UINT32				flags );


/* Рисует набор символов, аналогично AhiDrawChar */
UINT32 AhiDrawGlyph( AHIDEVCONTEXT_T	devCx,
                     AHIPOINT_T			*dstPt,
                     AHIBITMAP_T		*charsArr,
                     UINT32				count,
                     UINT32				flags );




/* ----------------------------------- Прочие функции ----------------------------------- */


/* Ожидает состояния VBlank дисплея */
UINT32 AhiDispWaitVBlank( AHIDEVCONTEXT_T	devCx,
                          UINT32			flags );

/* Обновляет CSTN-дисплей на таких устройствах как Motorola L6 и других. */
UINT32 AhiDispUpdate(AHIDEVCONTEXT_T context, AHIUPDATEPARAMS_T *update_params);

/* Ожидает окончания выполнения текущей очереди команд */
UINT32 AhiDrawIdle( AHIDEVCONTEXT_T devCx, UINT32 flags );

/* Проверка на пересечение ректов + результат пересечения */
BOOL AhiIntersect( AHIRECT_T *prc1, AHIRECT_T *prc2, AHIRECT_T *prcResult );

/* Получить список доступных видеорежимов. */
UINT32 AhiDispModeEnum(AHIDEVCONTEXT_T context, AHIDISPMODE_T *pMode, UINT32 index);

/* Получить текущий видеорежим. */
UINT32 AhiDispModeGet(AHIDEVCONTEXT_T context, AHIDISPMODE_T *pMode);

/* Установить видеорежим. */
UINT32 AhiDispModeSet(AHIDEVCONTEXT_T context, AHIDISPMODE_T *pMode, UINT32 flags);

/* Отключить/включить питание дисплея, перезагрузить дисплей. */
UINT32 AhiDispState(AHIDEVCONTEXT_T context, AHIDISPSTATE_T state, UINT32 flags);

#ifdef __cplusplus
}
#endif

#endif
