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
void *suAllocMem(UINT32 size, INT32 *err);
#ifdef EP2_INTERNAL
void *malloc(UINT32 size);
#else
#define malloc(sz) suAllocMem(sz, NULL)
#endif

// Освободить память
void suFreeMem(void *ptr);
#ifdef EP2_INTERNAL
void mfree(void *ptr);
#else
#define mfree(p) suFreeMem(p)
#endif

// Забивает нулями блок памяти
void __rt_memclr(void *, UINT32);
#ifdef EP2_INTERNAL
void memclr(void *, UINT32);
#else
#define memclr(m, sz) __rt_memclr(m, sz)
#endif

// Копирует блоки памяти
void *__rt_memcpy(void *, const void *, UINT32);
#ifdef EP2_INTERNAL
void *memcpy(void *, const void *, UINT32);
#else
#define memcpy(dst, src, sz) __rt_memcpy((void *) dst, (void *) src, (UINT32) sz)
#endif

// Забивает указанным байтом блок памяти
void *__rt_memset(void *, int, UINT32);
#ifdef EP2_INTERNAL
void *memset(void *, int, UINT32);
#else
#define memset(m, byte, sz) __rt_memset((void *) m, (int) byte, (UINT32) sz)
#endif

//
void *__rt_memmove(void *, const void *, UINT32);
#ifdef EP2_INTERNAL
void *memmove(void *, const void *, UINT32);
#else
#define memmove(dst, src, sz) __rt_memmove((void *) dst, (void *) src, (UINT32) sz)
#endif

// Ява менеджер памяти
void *AmMemAlloc(UINT32 size);
void AmMemFree(void *ptr);

// Получше чем su*Mem (но медленный)
// Для больших блоков использует su*Mem, для мЕньших - uis*Mem
// Доступно больше памяти
// После выделения еще и сам чистит память, поэтому медленный
// При неудаче нет ребута!
void *device_Alloc_mem(UINT32 count, UINT32 sz);
void device_Free_mem_fn(void *ptr);

typedef UINT32 UIS_PARTITION_BLOCK_SIZE_T;
typedef INT32 UIS_ALLOCATION_ERROR_T;  // 0 - OK, else - ERROR

void *uisAllocateMemory(UIS_PARTITION_BLOCK_SIZE_T nbytes, UIS_ALLOCATION_ERROR_T *status);
void uisFreeMemory(void *address);
void *uisReAllocMemory(void *address, UIS_PARTITION_BLOCK_SIZE_T new_size, UIS_ALLOCATION_ERROR_T *status);

/*
 * EXL, 24-Apr-2023:
 * Функции которые использует JVM для выделения памяти в Java Heap (800 КБ в дефолте).
 * Если телефон имеет поддержку CORElet'ов, функции работают сразу.
 * Иначе перед использованием нужно запустить Java-приложение и приостановить его.
 */

void *AmMemAllocPointer(int size);
void AmMemFreePointer(void *ptr);

void *memcmp(void *src, const void *dst, UINT32 bytes);
void *memchr(const void *ptr, int ch, UINT32 count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // SDK_MEMORY_H
