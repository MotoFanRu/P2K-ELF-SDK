// UTF-8 w/o BOM

#ifndef SDK_ZLIB_H
#define SDK_ZLIB_H

#include <typedefs.h>

struct internal_state;

typedef struct/* z_stream_s */{
	BYTE *	next_in;  /* next input byte */
	UINT32	avail_in;  /* number of bytes available at next_in */
	UINT32	total_in;  /* total nb of input bytes read so far */

	BYTE *	next_out; /* next output byte should be put there */
	UINT32	avail_out; /* remaining free space at next_out */
	UINT32	total_out; /* total nb of bytes output so far */

	char *	msg;	  /* last error message, NULL if no error */
	struct internal_state * state; /* not visible by applications */

	void *	zalloc_func;  /* used to allocate the internal state */
	void *	zfree_func;   /* used to free the internal state */
	void *	opaque_func;  /* private data object passed to zalloc and zfree */

	int		data_type;  /* best guess about the data type: binary or text */
	UINT32	adler;	  /* adler32 value of the uncompressed data */
	UINT32	reserved;   /* reserved for future use */
} z_stream;


#ifdef __cplusplus
extern "C" {
#endif

int inflate( z_stream * strm, int flush );
int inflateEnd( z_stream * strm );
int inflateInit2_(z_stream * strm, int windowBits,
					const char * version, int stream_size);

int inflateReset(z_stream * strm);

#ifdef __cplusplus
} /* extern "C" */
#endif

#if !defined(FTR_V600)
#define inflateInit2(strm,windowBits)		inflateInit2_(strm,windowBits,"1.2.1",sizeof(z_stream))
#else
#define inflateInit2(strm,windowBits)		inflateInit2_(strm,windowBits,"1.1.3",sizeof(z_stream))
#endif

#ifndef MAX_WBITS
#  define MAX_WBITS		15
#endif

#define Z_SYNC_FLUSH    2

#define Z_OK					0
#define Z_STREAM_END			1
#define Z_NEED_DICT				2
#define Z_ERRNO					(-1)
#define Z_STREAM_ERROR			(-2)
#define Z_DATA_ERROR			(-3)
#define Z_MEM_ERROR				(-4)
#define Z_BUF_ERROR				(-5)
#define Z_VERSION_ERROR			(-6)


#endif // SDK_UTIL_H
