// UTF-8 w/o BOM

#ifndef MME_H
#define MME_H

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef   void*    MME_GC_MEDIA_FILE;

typedef struct
{
   IFACE_DATA_T                 iface_data;
   MME_GC_MEDIA_FILE            media_handle;
   UINT8						status; // = 1 // при окончании воспроизведения , на L7e почему-то 128
} MME_PLAY_COMPLETE_T; 

typedef struct
{
   IFACE_DATA_T          iface_data;
   MME_GC_MEDIA_FILE   	 media_handle;
} MME_OPEN_SUCCESS_T;

/*******************************/

typedef enum
{
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

/***************************************
   AUDIO
****************************************/

// Открывает файл uri для воспроизведения, и возвращает указатель на открытый файл. uri файла может как начинаться на "file:/", так и не начинаться
MME_GC_MEDIA_FILE  MME_GC_playback_create(IFACE_DATA_T  *data, WCHAR * uri, UINT32 unk1, UINT32 unk2, UINT32 unk3, UINT32 unk4, UINT32 unk5, UINT32 unk6); 

// Закрывает файл
UINT32 MME_GC_playback_delete(MME_GC_MEDIA_FILE  media_file); 

// Начать воспроизведение файла
UINT32 MME_GC_playback_start(MME_GC_MEDIA_FILE  media_file, UINT32 unk1, UINT32 unk2);

// Одиночное простое воспроизведение файла
void MME_GC_playback_open_audio_play_forget(WCHAR *media_file_path);

// Остановить воспроизведение файла
UINT32 MME_GC_playback_stop(MME_GC_MEDIA_FILE  media_file); 

// Пауза
UINT32 MME_GC_playback_pause(MME_GC_MEDIA_FILE  media_file); 

// перемотка на ms мс. от начала файла
UINT32 MME_GC_playback_seek(MME_GC_MEDIA_FILE  media_file, UINT32 ms); 

typedef struct 
{
    WCHAR           *str;
    UINT16          str_size;
} STRING_T;

typedef struct 
{
    WCHAR           *str;
    UINT16          str_size;
    UINT8           unk0[4];
    UINT8           unk1[4];
} RATING_T;

typedef struct 
{
    STRING_T   title;
    STRING_T   author;
    STRING_T   copyright;
    STRING_T   description;
    STRING_T   performer;
    STRING_T   genre;
    RATING_T   rating;
    STRING_T   location;
    STRING_T   date;
    STRING_T   album;

} MEDIA_FILE_INFO_T;

typedef enum 
{
   PLAY_RATE_1X = 1,
   PLAY_RATE_2X,
   PLAY_RATE_3X,
   PLAY_RATE_4X,
   PLAY_RATE_MAX
} PLAY_RATE_T;

typedef enum {
    CODEC_NONE=0,
	MP3_CODEC=2,
    AAC_CODEC,
    MIDI_CODEC,
    WAV_CODEC,
    AMR_CODEC=15
} AUDIO_CODEC_TYPE_T;

typedef struct
{
    UINT32        				bit_rate;
    AUDIO_CODEC_TYPE_T          audio_codec;
    UINT32        				sampling_freq;
    UINT8           			audio_mode;    // 0=Mono, 1=Stereo 
    UINT16       				unk1[7];
    UINT8            			wav_bit_rate; // 0 =4, 1=8, 2=16 Kb/s
    UINT16            			unk2[7];
} AUDIO_FORMAT_T;

#if defined(FTR_L7E)
	#define ATTRIBUTE_RANGE_1 	0x5C // для L7e и Z3
#else
	#if defined(FTR_L9)
		#define ATTRIBUTE_RANGE_1 	0x5E // для L9 и K1 
	#else
		#define ATTRIBUTE_RANGE_1 	71
	#endif
#endif

typedef enum
{
  // get и set
	FILE_INFO=0, 				// MEDIA_FILE_INFO_T
	DURATION,    				// UINT32
	DURATION_MS, 				// UINT32
	FILE_SIZE,					// UINT32
	MEDIA_PATH=6,				// WCHAR
	PLAY_RATE=8,				// PLAY_RATE_T
	MEDIA_STOP_TIME=58,			// UINT32
	MEDIA_STOP_TIME_MS=60,		// UINT32
	PLAYBACK_AUDIO_VOLUME=63, 	// UINT8
	MEDIA_VOLUME,				// UINT8		
  
   // get
    AUDIO_FORMAT=ATTRIBUTE_RANGE_1,		// AUDIO_FORMAT_T
	POSITION=ATTRIBUTE_RANGE_1 + 3,		// UINT32
	POSITION_MS,							// UINT32 
	PAUSE_POSITION=114						// UINT32
} ATTRIBUTE_NAME_T;

// получаем аттрибуты 
UINT32 MME_GC_playback_get_attribute( MME_GC_MEDIA_FILE     handle,
									  ATTRIBUTE_NAME_T   		attribute_name,
								      void  				*attribute_value
									);

// устанавливаем аттрибуты 
UINT32 MME_GC_playback_set_attribute( MME_GC_MEDIA_FILE		handle,
									  ATTRIBUTE_NAME_T   	attribute_name,
									  void  				*attribute_value
									);


#ifdef __cplusplus
} /* extern "C" */
#endif									
									
#endif
