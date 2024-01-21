/* ***********************************
 *	Хэширование
 */
#ifndef SDK_CRYPT_H
#define SDK_CRYPT_H

#include <typedefs.h>

#define HASH_MD5_DATA_LENGTH		16


typedef struct {
	BYTE	data[HASH_MD5_DATA_LENGTH];
} HASH_MD5_T;

#ifdef __cplusplus
extern "C" {
#endif

void MD5( HASH_MD5_T * dst, const void * src, size_t src_length );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // SDK_CRYPT_H