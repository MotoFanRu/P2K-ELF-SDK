// UTF-8 w/o BOM

#ifndef SYNSOCK_API_H
#define SYNSOCK_API_H

#include <typedefs.h>
#include <events.h>

// менеджер сокетов

#define MAX_HOST_LEN				63
#define MAX_SOCKETS					8

#define SOCK_TCP					1
#define SOCK_UDP					2

#ifdef EP2
#include <loader2.h>

#define SockReqAPI(add_data) \
		AFW_CreateInternalQueuedEvAuxD(ldrGetConstVal(SOCKET_MGR_MAIN_REGISTER),&add_data,FBF_LEAVE,0,NULL);

#else

#define SockReqAPI(add_data) \
		AFW_CreateInternalQueuedEvAuxD(SOCKET_MGR_MAIN_REGISTER,&add_data,FBF_LEAVE,0,NULL);

#endif

enum {
	SOCK_ANSW_CREATE,
	SOCK_ANSW_READY,
	SOCK_ANSW_DATA,
	SOCK_ANSW_DELETE,
	SOCK_ANSW_ERROR,
	
	// tim apple mod
	SOCK_ANSW_NOWRITE,
	SOCK_ANSW_CLOSED
};
typedef UINT32 SOCK_ANSW_T;

typedef int				HSOCK;
#define HSOCK_INVALID	-1


#ifdef __cplusplus
extern "C" {
#endif


// SOCKET API

typedef HSOCK	socket_f( int type, UINT32 ev_callback );
typedef int		socket_connect_f( HSOCK hsock, const char * host, int port );
typedef UINT32	socket_write_f( HSOCK hsock, const void * buf, UINT32 size );
typedef UINT32	socket_read_f( HSOCK hsock, void * buf, UINT32 size );
typedef int		socket_bind_f( HSOCK hsock, int loc_port );
typedef int		socket_close_f( HSOCK hsock );
typedef int		socket_delete_f( HSOCK hsock );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* SYNSOCK_API_H */
