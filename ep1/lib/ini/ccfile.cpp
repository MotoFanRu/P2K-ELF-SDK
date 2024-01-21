//#define DEBUG

#include "ccfile.h"
#include <utilities.h>

#define ___MIN(a,b)  (((a) < (b)) ? (a) : (b))

CCFile::CCFile( void )
{
	this->hfile = NULL;
	this->pos = NULL;
	this->fcache = NULL;
	this->fcache_sz = 0;
	this->b_cache = FALSE;
	this->need_cache = FALSE;
}

CCFile::CCFile( BOOL b_en_cache )
{
	this->hfile = NULL;
	this->pos = NULL;
	this->fcache = NULL;
	this->fcache_sz = NULL;
	this->b_cache = b_en_cache;
	this->need_cache = b_en_cache;
}

CCFile::~CCFile( void )
{
	this->FreeCache();
	this->Close();
	this->b_cache = FALSE;
	this->need_cache = FALSE;
}

// ---------------------------------------

void CCFile::FreeCache( void )
{
	if ( this->fcache ) {
		delete [] this->fcache;
		this->fcache = NULL;
		this->fcache_sz = 0;
	}
}
UINT32 CCFile::Close( void )
{
	UINT32	status = RESULT_OK;
	if ( this->hfile ) {
		status = DL_FsCloseFile( this->hfile );
		this->hfile = NULL;
		this->pos = 0;
	}
	return status;
}

UINT32 CCFile::Open( const WCHAR * p_uri )
{
	FILE	f;
	if ( this->hfile != NULL )
		return RESULT_FAIL;
	if ( p_uri == NULL || DL_FsFFileExist(p_uri) == FALSE )
		return RESULT_FAIL;
	f = DL_FsOpenFile( p_uri, FILE_READ_MODE, 0 );
	if ( f == FILE_INVALID )
		return RESULT_FAIL;
	this->hfile = f;
	return RESULT_OK;
}

UINT32 CCFile::Read( void * buf, size_t count, size_t * read )
{
	size_t	sz;
	size_t	rd;
	char *	tmp;
	UINT32	status;
	
	if ( this->hfile == NULL )
		return RESULT_FAIL;
	if ( this->need_cache == TRUE ) {
		D(" make cache...", NULL);
		this->FreeCache();
		sz = ___MIN( this->GetSize(), FILE_CACHE_SIZE );
		D(" sz = %d", sz);
		tmp = new char[sz];
		D(" tmp = 0x%p", tmp);
		if ( tmp != NULL
				&& DL_FsFSeekFile( this->hfile, 0, SEEK_WHENCE_SET ) == RESULT_OK
				&& DL_FsReadFile( tmp, 1, sz, this->hfile, (UINT32 * ) &rd ) == RESULT_OK ) {
			D(" all OK... rd = %d", rd);
			this->fcache = tmp;
			this->fcache_sz = rd;
			this->need_cache = FALSE;	// OK, кэшировали файл, в следующий раз не нужно уже =)
		} else {
			D(" some error", NULL);
			delete [] tmp;
		}
		this->SetPos( this->pos );
	}
	rd = 0;
	// положение каретки после чтения
//	sz = this->GetSize();
	sz = ___MIN( this->pos + count, this->GetSize() );
	D(" sz = %d", sz);
	// Если кэш доступен и в кэше есть эта часть файла
	if ( this->b_cache == TRUE && sz <= this->fcache_sz ) {
		memcpy(buf, (this->fcache + this->pos), count);
		*read = this->GetSize()-this->pos;
		status = RESULT_OK;
	} else {
		status = DL_FsReadFile( buf, 1, count, this->hfile, (UINT32 *) read );
	}
	this->SetPos((UINT32)(sz));
	return status;
}

size_t CCFile::GetSize( void )
{
	if ( this->hfile != NULL )
		return DL_FsGetFileSize( this->hfile );
	return 0;
}

UINT32 CCFile::SetPos( UINT32 newpos )
{
	UINT32	status;
	if ( this->hfile == NULL )
		return RESULT_FAIL;
	status = DL_FsFSeekFile( this->hfile, newpos, SEEK_WHENCE_SET );
	if ( status == RESULT_OK )
		this->pos = newpos;
	return status;
}

UINT32 CCFile::GetPos( void )
{
	return this->pos;
}

void CCFile::EnableCache( BOOL b_en_cache )
{
	if ( b_en_cache == FALSE ) {
		// выключить
		this->FreeCache();
		this->need_cache = FALSE;
		this->b_cache = FALSE;
	} else {
		// включить
		this->b_cache = TRUE;
		this->need_cache = TRUE;
	}
}



