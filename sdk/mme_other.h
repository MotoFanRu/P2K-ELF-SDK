#ifndef MME_H
#define MME_H

#include <typedefs.h>


#ifdef __cplusplus
extern "C" {
#endif

#if (defined(FTR_L7E) || defined(FTR_L9))
	#define EV_MME_RANGE_START		  0xE0000
#else 
	#define EV_MME_RANGE_START		  0xC1000
#endif


#define EV_MME_OPEN_SUCCESS       EV_MME_RANGE_START // удачное открытие файла
#define EV_MME_OPEN_ERROR         EV_MME_RANGE_START+0x1 // не удачное открытие файла
#define EV_MME_SEEK_SUCCESS       EV_MME_RANGE_START+0x2 // возникает после использования ф-ции MME_GC_playback_seek
#define EV_MME_SEEK_ERROR         EV_MME_RANGE_START+0x3 // не удачный seek
#define EV_MME_PLAY_COMPLETE      EV_MME_RANGE_START+0xD // возникает при окончании проигрывания // в аттаче PLAY_COMPLETE_T
#define EV_MME_CLOSE_COMPLETE     EV_MME_RANGE_START+0x14 // при закрытия
#define EV_MME_STOP_COMPLETE      EV_MME_RANGE_START+0x15 // при стопе
#define EV_MME_STOPPED_AT_TIME    EV_MME_RANGE_START+0x16 // стоп во время?



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

#if defined(FTR_L7E) || defined(FTR_L9)
typedef enum
{
    CODEC_NONE = 0,
    MP3_CODEC = 2,
    AAC_CODEC,
    AAC_PLUS_CODEC = 4,
    MIDI_CODEC = 5,
    WAV_CODEC = 6,
    WMA_CODEC = 10,
    AMR_CODEC = 17,
    RAW_AUDIO = 18
} AUDIO_CODEC_TYPE_T;
#else
typedef enum{
    CODEC_NONE=0,
	MP3_CODEC=2,
    AAC_CODEC,
    MIDI_CODEC,
    WAV_CODEC,
    AMR_CODEC=15
} AUDIO_CODEC_TYPE_T;
#endif

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

typedef struct RECTANGLE_LIST
{
   UINT16                  x1; // верхний левый угол
   UINT16                  y1;
   UINT16                  x2; // нижний правый угол
   UINT16                  y2; 
   struct RECTANGLE_LIST   *next_list;
} RECTANGLE_LIST_T;

typedef struct RECTANGLE_T
{
	UINT16 left_x;
	UINT16 left_y;
	UINT16 right_x;
	UINT16 right_y;
} RECTANGLE_T;

#if defined(FTR_L7E) || defined(FTR_L9)
typedef struct  
{
	UINT16 				frame_num;
	UINT32 				color_key;
	UINT8  				rotation;
	UINT8  				display; // 0 - primary, 1 - secondary
	RECTANGLE_T 		base_rectangle;
	BOOL 				alpha_blending;
	UINT32 				alpha_value;
	INT16 				rectangle_off_x;
	INT16 				rectangle_off_y;
	RECTANGLE_LIST_T   *rectangle_list;
} SAFE_FRAME_ID_T;
#else
typedef struct  
{
   UINT16                  frame_num;
   RECTANGLE_LIST_T   *rectangle_list;
} SAFE_FRAME_ID_T; // для фото и видео 
#endif

#if defined(FTR_L7E) || defined(FTR_L9)
typedef enum
{
	MMSS_MP4 = 4,
	MMSS_3GP,
	MMSS_ROM,
	MMSS_RAW,

	MMSS_WMA = 17,
	MMSS_MP3,
	MMSS_MIDI,

	MMSS_IMELODY = 21,
	MMSS_MYMIX_MIX = 22,
	MMSS_MYMIX_BASE_MIDI = 23,
	MMSS_FUNLIGHT = 24,
	MMSS_AMR = 25,
	MMSS_AAC = 26,
	MMSS_AMR_WB = 27,
	MMSS_AU = 28,
	MMSS_WAV = 29,
	MMSS_3GA = 30,
	MMSS_AMRWB_FORMAT = 31,

	MMSS_JPEG = 35,
	MMSS_JFIF = 36,
	MMSS_JFIF_AND_EXIF = 37,
    MMSS_EXIF = 38,
    MMSS_CIFF = 39,
    MMSS_GIF = 40,
    MMSS_PNG = 41,
    MMSS_BMP = 42,
    MMSS_WBMP = 43,
    MMSS_EMS_BMP = 44
	
} MMSS_FILE_FORMAT_T;
#else
typedef enum
{
    // Video 
    MMSS_MP4=3,
    MMSS_3GP,
    MMSS_ROM,
    MMSS_RAW,
   
    // Audio 
    MMSS_WMA=12, 
    MMSS_MP3,
    MMSS_MIDI,
    MMSS_IMELODY,
    MMSS_AMR=19,
    MMSS_AAC,
    MMSS_WAV=22,

    // Image 
    MMSS_JPEG=25,
    MMSS_JFIF,
    MMSS_GIF=30,
    MMSS_PNG,
    MMSS_BMP,
    MMSS_WBMP,
    MMSS_EMS_BMP

} MMSS_FILE_FORMAT_T;
#endif

typedef enum
{
    H263_BASELINE_CODEC=1,
    H263_PROFILE_3_CODEC,
    WMV_CODEC,
    WMV9_CODEC,
    RV8_CODEC,
    RV9_CODEC,
    MPEG4_LEVEL_0_CODEC, // default
    VIDEO_NANCY_CODEC,
    VIDEO_INPUT_DEVICE  // для видео захвата

} VIDEO_CODEC_T; 

typedef enum
{
  COUNT_COLOR_2=1, 
  COUNT_COLOR_4,
  COUNT_COLOR_8, 
  COUNT_COLOR_16,
  COUNT_COLOR_32,
  COUNT_COLOR_64,
  COUNT_COLOR_128,
  COUNT_COLOR_256,
  COUNT_COLOR_512,
  COUNT_COLOR_1024,
  COUNT_COLOR_2048,
  COUNT_COLOR_4096,
  COUNT_COLOR_8192,
  COUNT_COLOR_16384,
  COUNT_COLOR_32768,
  COUNT_COLOR_65536 // 16bit, default

} COUNT_COLORS_T;

typedef enum
{
    COUNT_FRAME_RATE_1=1,
    COUNT_FRAME_RATE_2,
    COUNT_FRAME_RATE_3,
    COUNT_FRAME_RATE_4,
    COUNT_FRAME_RATE_5,
    COUNT_FRAME_RATE_6,
    COUNT_FRAME_RATE_7,
    COUNT_FRAME_RATE_10,
    COUNT_FRAME_RATE_15 // default

} COUNT_VIDEO_FRAME_RATE_T;

typedef struct MMSS_VIDEO_FORMAT_S
{
    VIDEO_CODEC_T          		video_codec; // видео-кодек
    COUNT_COLORS_T              count_colors;
    UINT32				        bit_rate;
    UINT8					    unk1; // =2
    COUNT_VIDEO_FRAME_RATE_T    frame_rate; // кол-во кадров в секунду
	UINT32    					frame_width; // ширина фрейма
    UINT32   					frame_height; // высота фрейма
	#if defined(FTR_L7E) || defined(FTR_L9)
	UINT32						video_sampling;
	#endif
} VIDEO_FORMAT_T;

typedef enum
{
    JPEG_HEIRARCHICAL_CODEC=1,
    GIF87a_CODEC,
    GIF89a_CODEC,
    JPEG_PROGRESSIVE_CODEC,
    JPEG_BASELINE_CODEC // default
} IMAGE_CODEC_T;

typedef struct
{
    COUNT_COLORS_T          colors;
    UINT8   				unk1; // = 3 - VGA
    IMAGE_CODEC_T       	image_codec;
    UINT8   				unk2; // = 0,1,2 ???
} IMAGE_FORMAT_T;

typedef enum
{
    MMSS_AUDIO,
    MMSS_VIDEO,
    MMSS_AUDIO_VIDEO,
    MMSS_IMAGE
} MMSS_MEDIA_TYPE_T;

typedef struct
{
    MMSS_MEDIA_TYPE_T               media_type;
    AUDIO_FORMAT_T             		audio_format;
    VIDEO_FORMAT_T             		video_format;
    IMAGE_FORMAT_T       			image_format;
} MMSS_MEDIA_FORMAT_T;

typedef struct 
{
    UINT16   width;
    UINT16   height;
} PICTURE_SIZE_T;



/************
   Playback
  *************/

// Открывает файл uri для воспроизведения, и возвращает указатель на открытый файл. uri файла может как начинаться на "file:/", так и не начинаться
MME_GC_MEDIA_FILE  MME_GC_playback_create(
	IFACE_DATA_T  		*data, 
	WCHAR 				*uri,
	SAFE_FRAME_ID_T 	*sf_id, // для видео и изображений
	INT32 				unk1, 
	void 				*image_mem, // указатель на изображение
	UINT32				image_size,
	UINT32 				unk4, // = 0 -for audio&video, =1 - for image  
	void 				*unk5, 
	PICTURE_SIZE_T		*picture_size // размер картинки
); 

// Закрывает файл
UINT32 MME_GC_playback_delete(MME_GC_MEDIA_FILE  media_file); 

// Начать воспроизведение файла
UINT32 MME_GC_playback_start(MME_GC_MEDIA_FILE  media_file, UINT32 unk1, UINT32 unk2); 

// Остановить воспроизведение файла
UINT32 MME_GC_playback_stop(MME_GC_MEDIA_FILE  media_file); 

// Пауза
UINT32 MME_GC_playback_pause(MME_GC_MEDIA_FILE  media_file); 

// перемотка на ms мс. от начала файла
UINT32 MME_GC_playback_seek(MME_GC_MEDIA_FILE  media_file, UINT32 ms); 


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
									
/*****************
   Capiture
*****************/

// созадаём  объект захвата
MME_GC_MEDIA_FILE MME_GC_capture_create
(
    IFACE_DATA_T                  *iface_data,
    SAFE_FRAME_ID_T               *sf_id, // для аудио - NULL
    MMSS_FILE_FORMAT_T            file_format, // формат файла
    MMSS_MEDIA_FORMAT_T     	  *media_format
);

// удаление объекта захвата
UINT32 MME_GC_capture_delete(MME_GC_MEDIA_FILE media_handle);

// начинает захват
UINT32 MME_GC_capture_start
(
    MME_GC_MEDIA_FILE   media_handle,
    WCHAR   			*path // путь к файлу
);

// установить аттрибуты
UINT32 MME_GC_capture_set_attribute
(
   MME_GC_MEDIA_FILE           media_handle,
   ATTRIBUTE_NAME_T   		   attribute_name,
   void                        *attribute_value
);

// остановить захват
UINT32 MME_GC_capture_stop(MME_GC_MEDIA_FILE media_handle);

// приостанавливает захват
UINT32 MME_GC_capture_pause(MME_GC_MEDIA_FILE media_handle);


// получить аттрибуты
UINT32 MME_GC_capture_get_attribute
(
    MME_GC_MEDIA_FILE           media_handle,
   ATTRIBUTE_NAME_T   		   attribute_name,
   void                        *attribute_value
);

#ifdef __cplusplus
} /* extern "C" */
#endif									
									
#endif
