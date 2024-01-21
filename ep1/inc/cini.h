/* ********************************************************************
 * Класс для работы с текстовым Файлом настройки. INI.
 * ********************************************************************
 *  (c) G-XaD 2008
 *  version: 1.00.00.00
 * ********************************************************************
 */
#ifndef CLASS_INI_H
#define CLASS_INI_H

#include <typedefs.h>
#include "ccfile.h"


#ifndef __cplusplus
#error "This is a C++ header file. Please use `tcpp` or `armcpp` compiler."
#endif // __cplusplus


#define SECTIONS_MAX_CACHE_SIZE		32


// секция в кэше
typedef struct {
	char *			name;		// динамически выделяемое имя секции
	UINT32			f_pos;		// офсет в ini-файле откуда начинается секция
} CINI_section_t;




class CINI {
public:
	CINI();
	~CINI();
	
	// base file functions
	UINT32 Open( const WCHAR * p_uri );
	UINT32 Close( void );
	
	// base config functions
	UINT32 ReadSections( void );
	BOOL FindSection( const char * section_name );
	BOOL ReadParam( const char * param_name, char * value_buf, size_t buf_size, const char * default_value );
	
	// удобства
	UINT32 GetPos( void );
	void SetPos( UINT32 );
	UINT32 ResetPos( void );
	const char * GetCurSection( void );
	
	
private:
	void FreeCache( void );
	
	FILE					file;
	/* Указатель чтения/записи внутри файла,
	      указывает на первую строку текущей секции, не на заголовок секции */
	UINT32					f_pos;
	char *					cur_section;
	CINI_section_t			cache[SECTIONS_MAX_CACHE_SIZE];
	UINT32					cache_size;
};


#endif // CLASS_FILE_EXTINI_H
