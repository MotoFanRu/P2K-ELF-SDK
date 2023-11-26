// UTF-8 w/o BOM
/********************************
  Web-сессии
********************************/

#ifndef __SDK_WEBSESSIONS_H__
#define __SDK_WEBSESSIONS_H__

#include <typedefs.h>


enum
{
	WS_RESULT_OK = 0,
	WS_RESULT_IN_PROGRESS,
	WS_RESULT_FAILED
};
typedef UINT8 WS_RESULT_T;


typedef enum {
	WS_TIMEOUT_NULL = 0,
	WS_TIMEOUT_1_MIN,
	WS_TIMEOUT_2_MIN,
	WS_TIMEOUT_5_MIN,
	WS_TIMEOUT_10_MIN,
	WS_TIMEOUT_15_MIN
} WS_TIMEOUT_T;


#define WS_RECORD_APN_LEN 100
#if defined(FTR_L7E) || defined(FTR_L9)
	#define WS_RECORD_LOGIN_LEN 64
	#define WS_RECORD_PASS_LEN 64
#else
	#define WS_RECORD_LOGIN_LEN 32
	#define WS_RECORD_PASS_LEN 16
#endif

typedef struct
{
	WCHAR		apn[WS_RECORD_APN_LEN+1];
	WCHAR		login[WS_RECORD_LOGIN_LEN+1];
	WCHAR		password[WS_RECORD_PASS_LEN+1];
} WS_GPRS_RECORD_T;


#define WS_RECORD_NAME_LEN 16
#define WS_RECORD_IP_LEN 8
#define WS_RECORD_URL_LEN 100
typedef struct
{
	UINT8		attr;
	WCHAR		name[WS_RECORD_NAME_LEN+1];
	WCHAR		home_page[WS_RECORD_URL_LEN+1];
	UINT16		ip1[WS_RECORD_IP_LEN];
	UINT16		port1;
	WCHAR		domain1[WS_RECORD_URL_LEN+1];
	UINT8		service_type1;
	UINT16		ip2[WS_RECORD_IP_LEN];
	UINT16		port2;
	WCHAR		domain2[WS_RECORD_URL_LEN+1];
	UINT8		service_type2;
	UINT16		dns1[WS_RECORD_IP_LEN];
	UINT16		dns2[WS_RECORD_IP_LEN];
	UINT8		timeout;
	void		*csd_record1;
	void		*csd_record2;
	WS_GPRS_RECORD_T *gprs_record;
} WS_RECORD_T;


// приходит в аттаче EV_WEBSESSION_OPERATION
typedef struct
{
	WS_RESULT_T		result;
	UINT8			index;
} WS_EVENT_T;


#ifdef __cplusplus
extern "C" {
#endif

// Получение записи Web-сессии по индексу 
WS_RESULT_T 
DL_DbWebSessionsGetSessionByIndex(	IFACE_DATA_T	*iface_data, 
									UINT8			index, 
									WS_RECORD_T		*ws_record);

// Получение индекса текущей Web-сессии
WS_RESULT_T
DL_DbWebSessionsGetDefaultSessionIndex( UINT8 *index );

// добавить запись
WS_RESULT_T
DL_DbWebSessionsAddSession(		IFACE_DATA_T	*iface_data,
								UINT8			device_type, // =0
								WS_RECORD_T		*ws_record,
								UINT8			unk ); // = 0

// удалить запись
WS_RESULT_T
DL_DbWebSessionsDeleteSessionByIndex(	IFACE_DATA_T	*iface_data,
										UINT8			index,
										UINT8			unk ); // = 0

// кол-во  возможных записей
WS_RESULT_T
DL_DbWebSessionsGetNumberGoodSessions(	UINT8	device_type, // =0
										UINT8	*total_num,
										UINT8	unk ); // = 0

// кол-во используемых записей
WS_RESULT_T
DL_DbWebSessionsGetNumberUsedSessions(	UINT8	device_type, // =0
										UINT8	*used_num,
										UINT8	unk ); // = 0

// кол-во доступных записей
WS_RESULT_T
DL_DbWebSessionsGetNumberAvailableSessions(	UINT8	device_type, // =0
											UINT8	*available_num,
											UINT8	unk ); // = 0

UINT32 DL_DbWebSessionsFindAbsIndexByName(WCHAR* name, UINT8* ind, UINT32 unk); // unk = 4
UINT32 DL_DbWebSessionsSetDefaultSessionIndex(SU_PORT_T* port, UINT8 ind);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __SDK_WEBSESSIONS_H__
