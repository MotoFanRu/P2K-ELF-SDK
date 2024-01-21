// UTF-8 w/o BOM

#ifndef SDK_STDLIB_H
#define SDK_STDLIB_H

#include <typedefs.h>
#include <time_date.h>
#include <stdargs.h>


/* ***************************************************
 * Minimum and maximum macros
 */

#ifndef __max
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef __min
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef abs
#define abs(i)		((i)<0 ? -(i) : (i))
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* ***************************************************
 * ANSI strings routines
 */

/* ВНИМАНИЕ!
     по стандарту лимитированные ф-ции с записью в dst является небезопасными,
	  ибо если длина строки src больше чем n, то в dst не будет записан символ окончания строки '\0'
*/

size_t  strlen( const char * str );								// Длина строки
char *  strcat( char * dst, const char * src );					// Конкатенация строк
char *  strchr( const char * str, int s );						// Поиск символа в строке
int     strcmp( const char * s1, const char * s2 );				// Сравнение строк
int     stricmp( const char * s1, const char * s2 );				// Сравнение строк
#define strcasecmp stricmp
char *  strcpy( char * dst, const char * src );					// Копирование строк

char *  strncat( char * dst, const char * src, size_t n );		// Лимитированая конкатенация
int     strncmp( const char * s1, const char * s2, size_t n );	// Лимит. сравнение
int     strincmp( const char * s1, const char * s2, size_t n );	// Лимит. сравнение
char *  strncpy( char * dst, const char * src, size_t n );		// Лимит. копирование

char *  strstr( const char * str, const char * srch );			// Поиск подстроки
char *  strrchr( const char * str, int s );						// Поиск символа в строке (реверс)
char *  strtok( char *, const char * );

int toupper( int c );		// Символ к верхнему регистру
int tolower( int c );		// Символ к нижнему регистру

size_t strcspn( const char * str1, const char * str2 );		// возвращает индекс символа в str1 который совпадает с одним из символов str2
size_t strspn( const char * str1, const char * str2 );		// аналогично, только `НЕ`

/* ***************************************************
 * Unicode strings routines
 */


size_t	u_strlen( const WCHAR * str );

WCHAR *	u_strcat( WCHAR * dst, const WCHAR * str );

// добавляет n символов из src к строке dst
WCHAR * u_strncat( WCHAR * dst, const WCHAR * src, UINT32 n );		

INT32 	u_strcmp( const WCHAR * s1, const WCHAR * s2 );
WCHAR *	u_strcpy( WCHAR * dst, const WCHAR * src );

INT32	u_strncmp( const WCHAR * s1, const WCHAR * s2, size_t count );
WCHAR *	u_strncpy( WCHAR * dst, const WCHAR * src, size_t count );

// ищет в str первое вхождение символа chr
WCHAR * u_strchr( const WCHAR * src, WCHAR chr );
// ищет в str последнее вхождение символа chr
WCHAR * u_strrchr( const WCHAR * src, WCHAR chr );

// ищет search в src (с учётом регистра)
WCHAR * u_strstr( const WCHAR * src, const WCHAR * search );
// тоже, но без учёта регистра
WCHAR * u_stristr( const WCHAR * src, const WCHAR * search );

// удаляет часть строки длиной n символов с позиции pos
INT32 u_strcut( WCHAR * str, UINT16 len_str, UINT16 pos, int n ); 

WCHAR *	u_strmakeupper( WCHAR * str );		// Unicode Строку к верхнему регистру
WCHAR *	u_strmakelower( WCHAR * str );		// Unicode Строку к нижнему регистру


/* ***************************************************
 * Convert
 */

// ANSI string to Unicode
WCHAR *	u_atou( const char * src, WCHAR * dst );

// Unicode string to ANSI (valid only for ascii-range symbols)
char *	u_utoa( const WCHAR * src, char * dst );

// Unicode to UTF-8
UINT16 DL_Char_convUCS2toUTF8String(	const WCHAR	*src_ptr,
										UINT16		src_len,
										UINT8		*trg_ptr,
										UINT16		trg_len);
// UTF-8 to Unicode
UINT16 DL_Char_convUTF8toUCS2String(	const UINT8	*src_ptr,
										UINT16		src_len, 
										WCHAR		*trg_ptr, 
										UINT16		trg_len);

// CP-1251 to Unicode
UINT16 DL_Char_convCP1251toUCS2String(	const UINT8	*src_ptr,
										UINT16		src_len,
										WCHAR		*trg_ptr,
										UINT16		trg_len);
// KOI8-R to Unicode
UINT16 DL_Char_convKOI8RtoUCS2String(	const UINT8	*src_ptr,
										UINT16		src_len,
										WCHAR		*trg_ptr,
										UINT16		trg_len);

/* ***************************************************
 *     Преобразование СТРОКА <-> ЧИСЛО
 *  str - указатель на троку
 *  end - ввозвращается указатель на строку в месте, где закончилось преобразование
 *  base, radix - основание СС.
 *     Если 0, то будет использоваться "умное" определение - "0x..." = HEX / "0..." = OCT / остальное DEC
 */
// преобразует строку в число long (INT32)
long strtol( const char * str, char ** end, int base );

// строка -> unsigned long (UINT32)
unsigned long strtoul( const char * str, char ** end, int base );

// число -> строку
char * _itoa( int value, char * string, int radix );
#define itoa(v,s,r)		_itoa(v,s,r)

// число -> строку Unicode
WCHAR *	u_ltou( long number, WCHAR * str );

// Unicode строка -> unsigned long (UINT32)
long u_atol( const WCHAR * str );

// преобразует из dec в HEX
WCHAR * u_itoh( int value, WCHAR * dst );

/* ***************************************************
 * end string funcs
 */



/* ***************************************************
 * other functions
 */

void * bsearch( const void * key, const void * buf, size_t num, size_t size, int (*compare)(const void *, const void *) );
void qsort( void * buf, size_t num, size_t size, int (*compare)(const void *, const void *) );

int rand( void );
void srand( unsigned int seed );
#define randomize()		srand((UINT32)suPalReadTime())
#define rand_init()		srand((UINT32)suPalReadTime())
#define random(num)		(rand() % (num))


int abs_0( int );

#ifndef EMUELF

int sprintf( char * str, const char * format, ... );
int snprintf( char * str, size_t maxlen, const char * format, ... );
int vsprintf( char * buffer, const char * format, va_list arglist );
int vsnprintf( char * buffer, size_t maxlen, const char * format, va_list arglist );

#ifndef WIN32
#define printf(format, ...)		PFprintf (format, ## __VA_ARGS__)
#define sscanf( buffer, format, ... )		Nsscanf ( buffer, format, ## __VA_ARGS__ )
#endif

#endif

void PFprintf( const char * format, ... ); // %d %x %s, a-la printf

int Nsscanf( const char *buffer, const char *format, ... /*arguments*/ );

void suLogData(UINT32 * phandle, UINT32 msgid, UINT32 num_pairs, ...);

#define LOG(format, ...) \
	do { \
		UtilLogStringData("%s:%d: " format, __func__, __LINE__, ##__VA_ARGS__); \
		PFprintf("%s:%d: " format, __func__, __LINE__, ##__VA_ARGS__); \
	} while(0);

#if !defined(DEBUG)
#define D(format, ...)
#define P()
#else
#define D(format, ...) \
	do { \
		UtilLogStringData("%s:%d: " format, __func__, __LINE__, ##__VA_ARGS__); \
		PFprintf("%s:%d: " format, __func__, __LINE__, ##__VA_ARGS__); \
	} while(0);
#define P() D("%s\n", "Debug Line!")
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // SDK_STDLIB_H
