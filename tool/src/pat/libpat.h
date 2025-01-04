#ifndef LIBPAT__H
#define LIBPAT__H

#if defined(WIN32)
#include <windows.h>
#endif

#if !defined(WIN32)
#define __stdcall
#endif

#if defined(_MSC_VER)
#ifdef LIBPAT_EXPORTS
#define LIBPAT_API __declspec(dllexport)
#else
#define LIBPAT_API __declspec(dllimport)
#endif
#else
#define LIBPAT_API
#endif

typedef unsigned char	u8;
typedef signed char		s8;
typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned long	u32;
typedef signed long		s32;

#if !defined(WIN32)
typedef u8 BOOL;
#define TRUE (BOOL)(1)
#define FALSE (BOOL)(0)
#endif

typedef struct
{
	u32			patcount;
	float		avglen;
	u32			maxlen;
	u32			len16cnt;

} LIBPAT_LDSTAT;

#pragma pack(push,1)

typedef struct
{
	char		*name;
	u8			*text;
	u8			*mask;
	u32			value;
	char		mode;
	u8			load;
	u16			length;
	u16			nmatch;
	u16			found;
	u16			flags;
	s16			offs[2];
} PATTERN_T;

#pragma pack(pop)


/* Сигнатура для коллбек-функции (см. libpatSetCallback).
 При использовании совместно с libpatFindAllPatterns:
 curPat - последняя найденная сигнатура
 count - порядковый номер сигнатуры в загруженном списке сигнатур */
typedef void (__stdcall *libpatCallback) ( PATTERN_T *curPat, u32 count );


#ifdef __cplusplus
extern "C" {
#endif


/* Открывает для работы бинарный файл (CG1 обычно) и возвращает указатель на него,
 а также размер через u32 *size. */
LIBPAT_API void* __stdcall libpatLoadBinary( const char *path, u32 *size );


/* Открывает и файл с паттернами в уже известном формате *.pts и загружает оттуда данные */
LIBPAT_API BOOL __stdcall libpatLoadPatterns( const char *path );


/* Функция для добавления паттерны вручную, формат - как в *.pts файле */
LIBPAT_API BOOL __stdcall libpatAddPattern( const char *pattern );


/* Сохраняет результаты поиска libpatFindAllPatterns в файле формата *.sym */
LIBPAT_API void __stdcall libpatSaveSymfile( const char *path );


/* Устанавливает смещение CG1. По умолчанию - 0x10080000 */
LIBPAT_API void __stdcall libpatSetOffset( u32 off );


/* Начать поиск всех введённых паттерн */
LIBPAT_API int	__stdcall libpatFindAllPatterns( );


/* Найти заданную паттерну. Возвращает массив найденных значений и count=кол-во значений.
 Возвращает больше одного значения только если в паттерне не указан искомый номер вхождения  */
LIBPAT_API u32* __stdcall libpatFindPattern( const char *pattern, u32 *count );


/* Аналог libpatFindPattern для быстрого поиска отдельной паттерны. Сразу возвращает
 найденное значение, либо 0, если значение не найдено или их найдено больше одного. */
LIBPAT_API u32 __stdcall libpatFindPatternSingle(const char *pattern);


/* Возвращает статистику по введённым паттернам */
LIBPAT_API void __stdcall libpatGetStats( LIBPAT_LDSTAT *result );


/* Устанавливает коллбек-функцию, которая будет вызываться при нахождении очередной
 паттерны при поиске функцией libpatFindAllPatterns.
 Вызов с NULL выключает коллбек */
LIBPAT_API void __stdcall libpatSetCallback( libpatCallback fn );


/* Сбрасывает все введённые паттерны */
LIBPAT_API void __stdcall libpatResetPatterns( );


/* Включает/выключает обработку результата RamTrans-ом (утилита для нахождения кешированного в IRAM адреса фунции)
 По умолчанию выключено */
LIBPAT_API void __stdcall libpatEnableRamTrans(BOOL enable);


/* Запускает проход по списку паттерн, вызывая для каждой коллбек-функцию. */
LIBPAT_API void __stdcall libpatCrawlStart();


/* Завершает досрочно проход по списку паттерн. Вызывается из коллбек-функции. */
LIBPAT_API void __stdcall libpatCrawlStop();

LIBPAT_API void __stdcall libpatInit();
LIBPAT_API void __stdcall libpatTerm();
LIBPAT_API void RamTransTerm();

#ifdef __cplusplus
}
#endif






#endif
