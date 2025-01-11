/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_TIMER_H
#define P2K_SDK_TIMER_H

#include "P2kT.h"
#include "P2kTimerT.h"
#include "P2kAppT.h"
#include "P2kDlT.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DL_TIMER_DATA_T {
	UINT32 time;
	UINT32 ID;
};

/***************************************
   Функции Timer
****************************************/

/* Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED, attachment которого указывает на DL_TIMER_DATA_T */

/*  Запустить таймер, который сработает через time мс */
//UINT32
//APP_UtilStartTimer(UINT32 time, UINT32 ID, APPLICATION_T *app);  // ID будет в DL_TIMER_DATA_T при обработке ивента

//******************************************************************************
// Функции Timer
//
// Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED (0x8205A), attachment которого указывает на
// DL_TIMER_DATA_T ID таймера будет в DL_TIMER_DATA_T при обработке ивента EV_TIMER_EXPIRED. (на самом деле это не ID, а
// просто какие-то данные)
//
//******************************************************************************

// Функции APP_* использовать для создания только одного таймера!
UINT32 APP_UtilStartTimer(UINT32 time, UINT32 ID, APPLICATION_T *app);  // Запустить таймер, который сработает через time мс

 // Запустить таймер, который будет срабаывать через каждые time мс
UINT32 APP_UtilStartCyclicalTimer( UINT32 time, UINT32 ID, APPLICATION_T *app );

UINT32 APP_UtilStopTimer(APPLICATION_T *app);  // Остановить таймер


//******************************************************************************
// Функции DL_* использовать для создания нескольких таймеров. iface->handle сохранять для остановки таймера!!!
// Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED (0x8205A), attachment которого указывает на
// DL_TIMER_DATA_T При обработке ивента поле seqnum - уникальный идентификатор. Он равен значению iface->handle после
// создания таймера
//******************************************************************************
// Запустить таймер, который сработает через time мс
UINT32 DL_ClkStartTimer(IFACE_DATA_T *iface, UINT32 period, UINT32 id);
// Запустить таймер, который будет срабаывать через каждые time мс
UINT32 DL_ClkStartCyclicalTimer( IFACE_DATA_T *iface, UINT32 period, UINT32 id );
 // Остановить таймер. Задать iface->handle значением сохранённым при запуске таймера
UINT32 DL_ClkStopTimer(IFACE_DATA_T *iface );

// Создаёт таймер.
// TimerPeriod - время через которое сработает таймер.
// TimerType - тип таймера. 0 - DL_ClkStartTimer. 1 - DL_ClkStartCyclicalTimer
// HandleTimerExpired - ф-ция которая будет вызвана при срабатывании таймера
// unk = 0
UINT32 AMS_TimerCreate(UINT32 TimerPeriod, UINT8 TimerType, void *HadleTimerExpired, UINT16 unk);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_TIMER_H */

/** @} */ /* end of P2K_Types */
