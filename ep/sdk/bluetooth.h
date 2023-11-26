// UTF-8 w/o BOM

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <typedefs.h>
#include <time_date.h>
#include <filesystem.h>


typedef struct {
	WCHAR				name[FILEURI_MAX_LEN+1];  // реальное имя файла
	WCHAR				temp_name[80]; // путь к временному файлу
	UINT8				unk1;
	char				*type; // тип контента
	BOOL				unk2;
	UINT32				size;  // размер контента 
	CLK_DATE_T			date_stamp; // дата 
	CLK_TIME_T			time_stamp; // время
	UINT8				unk3;
	UINT32		 		unk4[2];
	UINT32				offset; // оффсет во временном файле, с которого начитается контент
	FILE_HANDLE_REFERENCE_T	file_reference; // для  EMS
	BOOL				unk5;
	UINT32				unk6[2];
	void				*p_buffer; // указатель на контент
	INT32				unk7;
	UINT8				unk8[2];
} PREVIEW_RECORD_T;



#define OBEX_TRANSPORT_BT	0x01

#if defined(FTR_L7E) || defined(FTR_L9)
#define OBEX_OBJECT_FILE	0x06
#else
#define OBEX_OBJECT_FILE	0x05
#endif

#define OBEX_ACTION_COPY	0x00
#define OBEX_ACTION_MOVE	0x01

#define MIME_TYPE_STRING_LEN	50

/*typedef struct {
    UINT32        unk1;            	// 0x05000000
    WCHAR         uri[265];
    char          content_type[50]; // size=??
    UINT32        unk2;            	// 0x00010000 if move / NULL if copy
    UINT32        unk3;            	// 0x00010000
    UINT32        unk4;            	// NULL
} send_via_bt_attach_LTE;  */


typedef struct {
	UINT8			b_vobjectid;					// OBEX_OBJECT_FILE
	UINT8			pad1[3];
	WCHAR			aw_filename[FS_MAX_URI_NAME_LENGTH+1];
	char			ab_mimetype[MIME_TYPE_STRING_LEN+1];
	UINT8			obex_action;					// OBEX_ACTION_*
	UINT8			pad2[3];
	UINT16			selected_transport_type;		// OBEX_TRANSPORT_BT
	UINT8			pad3[2];
	UINT32			transport_list_size;			// 0
} SEND_VIA_BT_ATTACH_LTE;


/*typedef struct
{
     UINT8       	folder; //  5
     UINT8        	unk0[0x24C-sizeof(UINT8)-4];
     UINT16        	type; //0-copy,1-move
     UINT16       	unk4;
     DL_FS_MID_T 	*p_ids;
     UINT32        	num_files;
     UINT8        	unk1[2452-8-sizeof(UINT8)-(0x24C-sizeof(UINT8))-sizeof(DL_FS_MID_T)+1];
     UINT8        	unk2; // 1
     UINT8        	unk3[6];
} send_via_bt_attach_LTE2;*/


typedef struct {
	UINT8			b_vobjectid;					// OBEX_OBJECT_FILE
	UINT8			pad1[3];
	WCHAR			aw_filename[FS_MAX_URI_NAME_LENGTH+1];
	char			ab_mimetype[MIME_TYPE_STRING_LEN+1];
	UINT8			obex_action;					// OBEX_ACTION_*
	UINT8			pad2[3];
	UINT32			*p_file_ids;					// 
	UINT32			num_file_ids;					// 
	UINT8			pad3[2440-592];
	UINT16			selected_transport_type;		// OBEX_TRANSPORT_BT
	UINT8			pad4[2];
	UINT32			transport_list_size;			// 0
} SEND_VIA_BT_ATTACH_LTE2;


/*typedef struct
{
	UINT16			unk1; // = 1
	UINT8   		unk2[6];
	UINT8			unk3; // = 6
	UINT8			unk4[0x24B];
	DL_FS_MID_T 	*p_ids;
    UINT32        	num_files;
	UINT8			unk5[0x738];
} end_via_bt_attach_NEW;*/


typedef struct {
	UINT16			selected_transport_type;		// OBEX_TRANSPORT_BT
	UINT8			pad4[2];
	UINT32			transport_list_size;			// 0
	UINT8			b_vobjectid;					// OBEX_OBJECT_FILE
	UINT8			pad1[3];
	WCHAR			aw_filename[FS_MAX_URI_NAME_LENGTH+1]; // 265*2=530
	char			ab_mimetype[MIME_TYPE_STRING_LEN+1];   // 51
	UINT8			obex_action;					// OBEX_ACTION_*
	UINT8			pad2[3];
	UINT32			*p_file_ids;					// 
	UINT32			num_file_ids;					// 
	UINT8			pad3[2440-592];

} SEND_VIA_BT_ATTACH_NEW;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
