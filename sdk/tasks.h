#ifndef TASKS_H
#define TASKS_H 

#include "typedefs.h"

typedef void (*TASK_ENTRY_FN)(void *);

#ifdef __cplusplus
extern "C" {
#endif

INT32	suCreateTask( TASK_ENTRY_FN entry, UINT32 stacksize, UINT8 priority );
INT32	suDeleteTask( int tid );
void 	suSleep( UINT32 time, INT32 *result );

typedef UINT32			SEMAPHORE_HANDLE_T;
typedef UINT32			SEMAPHORE_STATE_T;	

#define SEMAPHORE_LOCKED			0
#define SEMAPHORE_UNLOCKED			1

#define SEMAPHORE_WAIT_FOREVER		0x7FFFFFFFL

/* 
	Создаёт бинарный семафор - может иметь только два состояния - SEMAPHORE_LOCKED и SEMAPHORE_UNLOCKED.
*/
SEMAPHORE_HANDLE_T suCreateBSem(SEMAPHORE_STATE_T init_state, INT32 *err);

/* 
    Создаёт семафор-счётчик - каждый Acquire уменьшает count на 1.
	Если при очередном Acquire счётчик равен 0, таск встаёт в ожидани ненулевого значения.
*/
SEMAPHORE_HANDLE_T suCreateCSem(SEMAPHORE_STATE_T init_count, SEMAPHORE_STATE_T bound, INT32 *err);

/* 
    Создаёт семафор-mutex.
    Работает аналогично бинарному, за исключением того, что он привязывается к таску-владельцу, 
    и владелец может многократно делать Acquire уже заблокированного семафора.  
*/
SEMAPHORE_HANDLE_T suCreateMSem(SEMAPHORE_STATE_T init_state, INT32 *err);

/* 
    Уничтожает семафор, созданный одной из Create-функций
*/
void suDeleteSem(SEMAPHORE_HANDLE_T handle, INT32 *err);

/* 
    Захватить семафор (уменьшет счётчик/меняет состояние на LOCKED, ждёт освобождения семафора, если требуется)
*/
INT32 suAcquireSem(SEMAPHORE_HANDLE_T handle, SU_TIME timeout, INT32 *err);

/* 
    Освободить семафор (увеличивает счётчик/меняет состояние на UNLOCKED)
*/
INT32 suReleaseSem(SEMAPHORE_HANDLE_T handle, INT32 *err);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
