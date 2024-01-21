// UTF-8 w/o BOM

#ifndef SDK_MEMORY_H
#define SDK_MEMORY_H

#include <typedefs.h>


#ifdef __cplusplus
extern "C" {
#endif

// Выделить память
// Если выделить память не удалось, то в err будет код ошибки
// Если вторым параметром было NULL то при неудаче выйдет ребут
void * suAllocMem( size_t size, INT32 * err );
#ifdef EP2_INTERNAL
void * malloc( size_t size );
#else
#define malloc(sz) suAllocMem( sz, NULL )
#endif

// Освободить память
void suFreeMem( void * ptr );
#ifdef EP2_INTERNAL
void mfree( void * ptr );
#else
#define mfree(p) suFreeMem(p)
#endif

// Забивает нулями блок памяти
void __rt_memclr( void *, size_t );
#ifdef EP2_INTERNAL
void memclr( void *, size_t );
#else
#define memclr(m,sz)			__rt_memclr(m,sz)
#endif

// Копирует блоки памяти
void * __rt_memcpy( void *, const void *, size_t );
#ifdef EP2_INTERNAL
void * memcpy( void *, const void *, size_t );
#else
#define memcpy(dst,src,sz)		__rt_memcpy((void *)dst,(void *)src,(size_t)sz)
#endif

// Забивает указанным байтом блок памяти
void * __rt_memset( void *, int, size_t );
#ifdef EP2_INTERNAL
void * memset( void *, int, size_t );
#else
#define memset(m,byte,sz)			__rt_memset((void *)m,(int)byte,(size_t)sz)
#endif

// 
void * __rt_memmove( void *, const void *, size_t );
#ifdef EP2_INTERNAL
void * memmove( void *, const void *, size_t );
#else
#define memmove(dst,src,sz)			__rt_memmove((void *)dst,(void *)src,(size_t)sz)
#endif


// Ява менеджер памяти
void * AmMemAlloc( size_t size );
void AmMemFree( void * ptr );


// Получше чем su*Mem (но медленный)
// Для больших блоков использует su*Mem, для мЕньших - uis*Mem
// Доступно больше памяти
// После выделения еще и сам чистит память, поэтому медленный
// При неудаче нет ребута!
void * device_Alloc_mem( UINT32 count, size_t sz );
void device_Free_mem_fn( void * ptr );


typedef UINT32   UIS_PARTITION_BLOCK_SIZE_T ;
typedef INT32    UIS_ALLOCATION_ERROR_T ; // 0 - OK, else - ERROR

void * uisAllocateMemory( UIS_PARTITION_BLOCK_SIZE_T nbytes, UIS_ALLOCATION_ERROR_T *status); 
void   uisFreeMemory( void * address);
void * uisReAllocMemory( void * address, UIS_PARTITION_BLOCK_SIZE_T new_size, UIS_ALLOCATION_ERROR_T *status); 

/*
 * EXL, 24-Apr-2023:
 * Функции которые использует JVM для выделения памяти в Java Heap (800 КБ в дефолте).
 * Если телефон имеет поддержку CORElet'ов, функции работают сразу.
 * Иначе перед использованием нужно запустить Java-приложение и приостановить его.
 */

void* AmMemAllocPointer(int size);
void AmMemFreePointer(void *ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // SDK_MEMORY_H
