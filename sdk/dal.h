// UTF-8 w/o BOM

#ifndef __DAL_H__
#define __DAL_H__

#include <typedefs.h>
#include "ati.h"
#include "canvas.h"


#if defined(FTR_C650)
#define DISPLAY_WIDTH		128
#define DISPLAY_HEIGHT		128
#elif defined(FTR_L6)
#define DISPLAY_WIDTH		128
#define DISPLAY_HEIGHT		160
#else
#define DISPLAY_WIDTH		176
#define DISPLAY_HEIGHT		220
#endif
#define DISPLAY_BPP			16
#define DISPLAY_BYTESPP		2

// Макрос для преобразования трёх 8-битных компонент цвета в формат RGB565
#define ATI_565RGB(r, g, b) (UINT32) ((r & 0xf8) << 8 | (g & 0xFC) << 3 | (b & 0xf8) >> 3)

enum DISPLAY_TYPE_ENUM
{
	DISPLAY_MAIN=0,
	DISPLAY_CLI,
	DISPLAY_EXTERNAL,
	DISPLAY_MAIN_AND_CLI,
	DISPLAY_NONE
};
typedef UINT8 DISPLAY_TYPE_T;

enum
{
	UIS_ONLY,					// Нормальный режим
	UIS_AND_MME,				// Внеэкранная поверхность освобождается
	UIS_AND_KJAVA,
	TCMD_ONLY,					// Освобождается масимум памяти, включая экранную поверхность
	MME_CAPTURE_MODE_ONLY,
	PARTIAL_DISPLAY_MODE,
	YUV_MODE_ONLY,
	YUV_AND_UIS_MODE,
	ATI_MEM_MODE_NONE
};
typedef UINT8 ATIDEVMEMCONFIG_T;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

AHISURFACE_T DAL_ATISurfAlloc( AHIDEVCONTEXT_T devCtx, UINT32 size, AHIPIXFMT_T pixFmt );

BOOL DAL_ATISurfFree( AHIDEVCONTEXT_T devCtx, AHISURFACE_T surf );

/* Возвращает системный контент. Не рекомендуется менять */
AHIDEVCONTEXT_T DAL_GetDeviceContext( DISPLAY_TYPE_T display );

/* Возвращает текущую системную поверхность рисования */
AHISURFACE_T DAL_GetDrawingSurface( DISPLAY_TYPE_T display );

// рисовать bitmap на экране
BOOL DAL_WriteDisplayRegion(	GRAPHIC_REGION_T*	upd_region,
								UINT16				*src_buf,	// RGB444, RGB565
								DISPLAY_TYPE_T		display,	// тип экрана
								BOOL				check_mask);

/* C650 Additional DAL functions. */
BOOL DAL_UpdateDisplayRegion(GRAPHIC_REGION_T *upd_region, UINT16 *src_buf);
/* This one is can be used to relative bitmap displaing on the screen. */
BOOL DAL_UpdateRectangleDisplayRegion(GRAPHIC_REGION_T *upd_region, UINT16 *src_buf, DISPLAY_TYPE_T display);

// получить пиксел
UINT16 *DAL_GetDisplayPixelAddress(	UINT8			buf_type,	//тип буффера = NULL
									GRAPHIC_POINT_T	pixel_coordinate,
									DISPLAY_TYPE_T	display);


// Меняет конфигурацию видеопамяти на указанную
void DAL_ATIDevMemConfiguration( ATIDEVMEMCONFIG_T devMemConfig );

// Возвращает текущий номер конфигурации видеопамяти
ATIDEVMEMCONFIG_T DAL_GetActiveATIDevMemConfiguration( void );

// Удаляет экранную поверхность кеша UIS
void DAL_FreeCache( void );

// Отключить диспей
void DAL_DisableDisplay( UINT32 p1 ); // p1 = 0

// Включить диспей
void DAL_EnableDisplay( UINT32 p1 ); // p1 = 0

// Возвращает экранную поверхность кеша UIS
// лучше всегда запрашивать размер из DAL_GetCacheSize
AHISURFACE_T DAL_GetCachedSurface( AHIPOINT_T temp_surf_size );

// размер кеша
AHIPOINT_T DAL_GetCacheSize( void );

// EXL: Указатель на структуру/класс DAL в памяти.
extern UINT8 *Class_dal;

// EXL: Нарисовать текущую обоину растеризатором на холсте
//      Подходит для V600
extern UINT32 theWallpaper;
BOOL flushWallpaperOnScreen(UINT32 *Wallpaper_class_address, GRAPHIC_REGION_T wallpaper_region, DISPLAY_TYPE_T display_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __DAL_H__ */
