// UTF-8 w/o BOM

#ifndef __SDK_DSMA_H__
#define __SDK_DSMA_H__


// protocols
#define PROTOCOL_TCP		1
#define PROTOCOL_SSL		2
#define PROTOCOL_UDP		3
#define PROTOCOL_IP			4

#define USE_CSD				1
#define USE_GPRS			2


#define PROXY_NULL			0


typedef UINT32		CMD_NUM;

typedef struct {

} SOCKET_BUFFER_T;

typedef struct _SOCKET_T SOCKET_T;				// Инкапсуляция структуры

// proxy_state
enum
{
	PROXY_STATE_ACTIVE,		// включено
	PROXY_STATE_INACTIVE,	// выключено (когда само выключается)
	PROXY_STATE_FAILED,		// ошибка
	PROXY_STATE_LINK_AVAILABLE, // Matching datalink activated
	PROXY_STATE_DEFAULT_LINK_UPDATED, // Default link has being updated.
	PROXY_STATE_LINK_ATTACH_COUNT, // Number of clients using matching link changed
	PROXY_STATE_GPRS_SUSPEND,
	PROXY_STATE_GPRS_RESUME,
	PROXY_STATE_NONE = 0xff
};
typedef UINT8 PROXY_STATE_T;

// buf_state
enum
{
	BUFFER_STATE_XON,		// можно посылать данные
	BUFFER_STATE_XOFF,		// нельзя посылать данные - нужно подождать
	BUFFER_STATE_DATA,		// новые данные - нужно считать
	BUFFER_STATE_NONE = 0xff
};
typedef UINT8 BUFFER_STATE_T;

// атрибуты
enum
{
	DSMA_ATTR_NULL = 0,

	DSMA_ATTR_TRANS_PROT = 0x1000,
	DSMA_ATTR_LOCAL_PORT,
	DSMA_ATTR_REMOTE_PORT,
	DSMA_ATTR_PRIORITY,

	DSMA_ATTR_NET_PROT = 0x2000,
	DSMA_ATTR_LOCAL_IP,
	DSMA_ATTR_REMOTE_IP,

	DSMA_ATTR_LINK_PROT = 0x3000,
	DSMA_ATTR_USER_NAME,
	DSMA_ATTR_PW,
	DSMA_ATTR_AUTH_TYPE,
	DSMA_ATTR_DNS_1,
	DSMA_ATTR_DNS_2,

	DSMA_ATTR_PHYS_PROT = 0x4000,
	DSMA_ATTR_APN,

	DSMA_ATTR_LINK_IS_SHAREABLE = 0x4000 + 7,
	DSMA_ATTR_LINK_IS_DEFAULT,
	DSMA_ATTR_LINK_IS_PERSISTANT
};
typedef UINT16 DSMA_ATTR_T;


typedef struct
{
	DSMA_ATTR_T		attr;
	UINT32			link_desc_ind;
	void *			value;
	UINT32			size;
	UINT32			result;
} DSMA_ATTRIBUTE_T;


typedef struct
{
	UINT8		source;
	UINT8		appCallCause;
	UINT8		dlSigCallCause;
	UINT8		networkCallCause;
	UINT8		gprsError;
	INT8		dfspError;
	UINT8		dsmaCause;
} DSMA_CAUSE_T;


typedef struct
{
	UINT8		foregroundClients;
	UINT8		backgroundClientst;
	UINT8		listeningClients;
} DSMA_LINK_ATTACH_COUNT_T;


typedef struct
{
	UINT8 unk1;
	UINT16 unk2;
	UINT8 unk3;
} DSMA_QOS_INFO_T;


typedef struct
{
	UINT32						proxy_id;		// Proxy identifier.
	UINT8						status;			// статус предыдущего запроса (RESULT_OK/RESULT_FAIL)
	CMD_NUM						cmd;			// уникальный номер - можно использовать для идентификации
	SU_PORT_T					port;			// вот на этот порт (не сетевой) приходят ивенты
	PROXY_STATE_T				proxy_state;	// Active, Inactive, Activate failed.
	BUFFER_STATE_T				buf_state;
	UINT32						attr_num;
	DSMA_ATTRIBUTE_T  *			attr_list;
	UINT32						lp_handle;
	DSMA_CAUSE_T				cause;
	DSMA_LINK_ATTACH_COUNT_T	linkAttachCount;
	UINT32						handle;

#if defined(FTR_L7E) || defined(FTR_L9)
	UINT8						sessionId;
	DSMA_QOS_INFO_T				qosInfo;
	UINT8						localPortLength;
	UINT16						*localPortInfo;
	DSMA_QOS_INFO_T				minReqQosInfo;
#endif

} DSMA_DATA_T;	// [72]


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// После каждой команды запоминаем CMD_NUM и сравниваем его с CMD_NUM из аттача к ивенту чтобы убедиться что это наш ивент

// создает прокси, дальше ловим ивент EV_DSMA_PROXY_CREATE
CMD_NUM
DL_DSMA_ProxyCreate( SU_PORT_T port );

// удаляет/освобождает прокси
CMD_NUM
DL_DSMA_ProxyDestroy( UINT32 proxy_id );

// Выставить атрибуты, ответ в EV_DSMA_PROXY_ATTR
CMD_NUM
DL_DSMA_ProxyAttributeSet( UINT32 proxy_id, UINT32 attr_count, DSMA_ATTRIBUTE_T * attr_list );

// Получить атрибуты, ответ в EV_DSMA_PROXY_ATTR
CMD_NUM
DL_DSMA_ProxyAttributeGet( UINT32 proxy_id, UINT32 attr_count, DSMA_ATTRIBUTE_T * attr_list );

// Активирует прокси, создает соединение в соответствии с настройками, ответ в EV_DSMA_PROXY_STATE
CMD_NUM
DL_DSMA_ProxyActivate( UINT32 proxy_id );

// Деактивирует прокси, закрывает сокеты и разрывает соединение
CMD_NUM
DL_DSMA_ProxyDeactivate( UINT32 proxy_id );

// Деактивирует прокси, закрывает сокеты (соединение остается, т.о. можно быстро активировать)
CMD_NUM
DL_DSMA_ProxyDeactivateSocket( UINT32 proxy_id );

// Отправить данные, возвращает кол-во байт
UINT32
DL_DSMA_ProxyWrite( UINT32 proxy_id, BYTE * buf, UINT32 size );

// Прочитать данные, возвращает кол-во байт
UINT32
DL_DSMA_ProxyRead( UINT32 proxy_id, BYTE * buf, UINT32 size );

BOOL dl_wvim_alloc_socket_buff( SOCKET_BUFFER_T * buff, size_t size, BOOL param2 );	// param2 = FALSE
void dl_wvim_socket_close( SOCKET_T * socket );

// ф-ция сама выделяет память под буфер сокера :-\, возвращает сколько байт прочитано
size_t dl_wvim_socket_read( SOCKET_T * socket );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __SDK_DSMA_H__
