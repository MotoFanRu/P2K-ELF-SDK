// UTF-8 w/o BOM

#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <typedefs.h>
#include <events.h>

// менеджер погоды 
// www.gismeteo.ru

#ifdef EP2
#include <loader2.h>

#define WeatherReqAPI(add_data) \
		AFW_CreateInternalQueuedEvAuxD(ldrGetConstVal(WEATHER_MGR_MAIN_REGISTER),&add_data,FBF_LEAVE,0,NULL);

#else

#define WeatherReqAPI(add_data) \
		AFW_CreateInternalQueuedEvAuxD(WEATHER_MGR_MAIN_REGISTER,&add_data,FBF_LEAVE,0,NULL);

#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef UINT32 Weather_getForecast_f(WCHAR *buf, UINT8 num); // num=1...4 - пока не используется
typedef UINT32 Weather_getTemperature_f(WCHAR *buf, UINT8 num);
typedef UINT32 Weather_getPhenomena_f(WCHAR *buf, UINT8 num);
typedef UINT32 Weather_getWind_f(WCHAR *buf, UINT8 num);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
