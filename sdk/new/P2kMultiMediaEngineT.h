/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_MME_T_H
#define P2K_SDK_MME_T_H

#include "P2kT.h"
#include "P2kDeviceLayer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(EM1) || defined(EM2) || defined(EA1)
// Для совместимости с сорцами LTE.
// Воспроизведение
#define MME_GC_playback_create(ifd, uri, sf, unk1, im_ptr, im_size, unk4, unk5, pic_size) \
	MME_FW_gc_handle_playback_create(ifd, uri, sf, unk1, im_ptr, im_size, unk4, unk5, pic_size);
#define MME_GC_playback_delete(mh) MME_FW_gc_handle_delete(mh);
// uri = 0, так как путь к файлу задаётся в  MME_FW_gc_handle_playback_create. взято из прошивы.
#define MME_GC_playback_start(mh, uri)            MME_FW_gc_handle_start( mh, uri );
#define MME_GC_playback_stop(mh)                  MME_FW_gc_handle_stop(mh);
#define MME_GC_playback_close(mh)                 MME_FW_gc_handle_close(mh);
#define MME_GC_playback_pause(mh)                 MME_FW_gc_handle_pause(mh);
#define MME_GC_playback_seek(mh, ms)              MME_FW_gc_handle_seek(mh, ms);
#define MME_GC_playback_get_attribute(mh, an, av) MME_FW_gc_handle_get_attribute(mh, an, av);
#define MME_GC_playback_set_attribute(mh, an, av) MME_FW_gc_handle_set_attribute(mh, an, av);

// захват
#define MME_GC_capture_create(ifd, sf, ff, mf)    MME_FW_gc_handle_capture_create(ifd, sf, ff, mf);
#define MME_GC_capture_delete(mh)                 MME_FW_gc_handle_delete(mh);
#define MME_GC_capture_start(mh, uri)             MME_FW_gc_handle_start(mh, uri);
#define MME_GC_capture_stop(mh)                   MME_FW_gc_handle_stop(mh);
#define MME_GC_capture_close(mh)                  MME_FW_gc_handle_close(mh);
#define MME_GC_capture_pause(mh)                  MME_FW_gc_handle_pause(mh);
#define MME_GC_capture_get_attribute(mh, an, av)  MME_FW_gc_handle_get_attribute(mh, an, av);
#define MME_GC_capture_get_attrib_len(an, x)         mme_fw_gc_handle_get_attrib_len(an, x);
#define MME_GC_capture_set_attribute(mh, an, av)  MME_FW_gc_handle_set_attribute(mh, an, av);
#endif

 // Проверено на V3x!
#if defined(EM1) || defined(EM2) || defined(EA1) || defined(FTR_L7E) || defined(FTR_L9)
#define EV_MME_RANGE_START 0xE0000
#else
#define EV_MME_RANGE_START 0xC1000
#endif

#define EV_MME_OPEN_SUCCESS EV_MME_RANGE_START        // удачное открытие файла В аттаче 16 байт MME_OPEN_SUCCESS_T
#define EV_MME_OPEN_ERROR   EV_MME_RANGE_START + 0x1  // не удачное открытие файла
#define EV_MME_SEEK_SUCCESS EV_MME_RANGE_START + 0x2  // возникает после использования ф-ции MME_GC_playback_seek
#define EV_MME_SEEK_ERROR   EV_MME_RANGE_START + 0x3  // не удачный seek
#if defined(EM1) || defined(EM2) || defined(EA1)
#define EV_MME_PLAY_COMPLETE \
	EV_MME_RANGE_START + 0xF  //+++ //возникает при окончании проигрывания. в аттаче 16 байт (PLAY_COMPLETE_T)
#else
#define EV_MME_PLAY_COMPLETE \
	EV_MME_RANGE_START + 0xD  // возникает при окончании проигрывания // в аттаче PLAY_COMPLETE_T
#endif
// Проверить эти ивенты на V3x
#define EV_MME_PLAY_MARKER \
	EV_MME_RANGE_START + 0xE  //??? //Возникает каждую секудну при проигрывании. в аттаче 16 байт (MME_PLAY_COMPLETE_T)
#define EV_MME_CLOSE_COMPLETE  EV_MME_RANGE_START + 0x14  // при закрытия
#define EV_MME_STOP_COMPLETE   EV_MME_RANGE_START + 0x15  // при стопе
#define EV_MME_STOPPED_AT_TIME EV_MME_RANGE_START + 0x16  //+++ стоп во время воспроизведения. в аттаче 12 байт (MME_OPEN_SUCCESS_T) LTE, V3x


typedef enum {
	// изображения
	MIME_TYPE_IMAGE_GIF = 0,
	MIME_TYPE_IMAGE_BMP,
	MIME_TYPE_IMAGE_WBMP,
	MIME_TYPE_IMAGE_PNG,
	MIME_TYPE_IMAGE_JPEG,

	// аудио
	MIME_TYPE_AUDIO_MID = 11,
	MIME_TYPE_AUDIO_MIDI,
	MIME_TYPE_AUDIO_MIX,
	MIME_TYPE_AUDIO_BAS,
	MIME_TYPE_AUDIO_MP3,
	MIME_TYPE_AUDIO_AAC,
	MIME_TYPE_AUDIO_AMR = 24,
	MIME_TYPE_AUDIO_MP4 = 26,
	MIME_TYPE_AUDIO_M4A,

	// видео
	MIME_TYPE_VIDEO_ASF = 34,
	MIME_TYPE_VIDEO_MP4 = 36,
	MIME_TYPE_VIDEO_MPEG4,
	MIME_TYPE_VIDEO_H263

} MIME_TYPE_T;


// Handle файла.
typedef void *MME_GC_MEDIA_FILE;

typedef struct {
	IFACE_DATA_T iface_data;
	MME_GC_MEDIA_FILE media_handle;
	UINT8 status;  // = 1 // при окончании воспроизведения , на L7e почему-то 128,  // RAINBOW POG. V3x - 8 при окончании воспроизведения.
#if defined(EM1) || defined(EM2) || defined(EA1)
	UINT8 unk[3];
#endif
} MME_PLAY_COMPLETE_T; // Размер = 16 байт (V3x)

typedef struct {
	IFACE_DATA_T iface_data;
	MME_GC_MEDIA_FILE media_handle;
} MME_OPEN_SUCCESS_T;  // Размер = 12 байт

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_MME_T_H */

/** @} */ /* end of P2K_Types */
