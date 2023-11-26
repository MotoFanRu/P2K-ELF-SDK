// UTF-8 w/o BOM
/********************************
  Сообщения
********************************/

#ifndef __SDK_SMS_H__
#define __SDK_SMS_H__

#include <typedefs.h>

typedef enum
{
	MSG_FOLDER_INBOX			  = 0, // Входящие 
	MSG_FOLDER_OUTBOX			 = 1, // Исходящие 
	MSG_FOLDER_INFOSRV_INBOX	  = 2,
	MSG_FOLDER_QUICKNOTE		  = 3,
	MSG_FOLDER_DRAFTS_FOLDER	  = 4,
	MSG_FOLDER_EMAIL_INBOX		= 5,
	MSG_FOLDER_TEMP_FOLDER		= 15,
	MSG_FOLDER_VOICEMAIL		  = 16,
	MSG_FOLDER_BROWSER_MSGS	   = 19,
	MSG_FOLDER_ALL_FOLDERS		= 23,
	MSG_FOLDER_INVALID			= 255
} MSG_FOLDER_T;

typedef enum
{
	MSG_TYPE_DEFAULT = 0,
	MSG_TYPE_IMMEDIATE,
	MSG_TYPE_DATE_TIME,
	MSG_TYPE_EMERGENCY,
	MSG_TYPE_SIM_SPECIFIC,
	MSG_TYPE_SIM_SPECIFIC_FTM_OTA,
	MSG_TYPE_BROWSER,
	MSG_TYPE_PREPAY,
	MSG_TYPE_INTERACTIVE,
	MSG_TYPE_FAX_NOTIFY_SCIM,
	MSG_TYPE_FAX_NOTIFY_DEFAULT,
	MSG_TYPE_EMAIL_NOTIFY_SCIM,
	MSG_TYPE_EMAIL_NOTIFY_DEFAULT,
	MSG_TYPE_UNSPECIFIED,
	MSG_TYPE_STATUS_REPORT_SCIM,
	MSG_TYPE_STATUS_REPORT_DEFAULT,
	MSG_TYPE_NOT_GIVEN
} MSG_TYPE_MSG_T;

typedef enum
{
	MSG_ATTR_READ_FLAG_INVALID = 0x00,
	MSG_ATTR_READ_FLAG_VALID = 0x01
} MSG_ATTR_MASK_T;

typedef enum
{
	MSG_MEM_READ_DELETED,
	MSG_MEM_STORED_SENT,
	MSG_MEM_RECEIVED,
	MSG_MEM_UNSPECIFIED
} MSG_MEMORY_STORAGE_T;

typedef struct
{
#if defined(FTR_V600)
	UINT8 mask;
#else
	UINT16 mask;
#endif
	UINT8 read_flag;
	UINT8 sent_flag;
	UINT8 priority;
	UINT8 ack_flag;
	UINT8 ems_flag;
#if !defined(FTR_V600)
	UINT8 attach_flag;
	UINT8 msg_cont_type;
#endif
	UINT8 locked_flag;
	UINT8 only_one_part_available;
	UINT8 is_complete;
#if !defined(FTR_V600)
	UINT8 addnl_attrib;
#endif
	UINT8 privacy;
	BOOL manual_ack;
	BOOL delivery_ack;
#if !defined(FTR_V600)
	BOOL reply_info;
#endif
} MSG_ATTR_T;

typedef struct
{
	UINT8		pad[0x6c];
	WCHAR		phone_number[0x22];//  номер 
	UINT8		pad2[0x1C8];
	WCHAR		text[0x100]; // текст 
} MSG_READ_T;

typedef enum {
	MSG_SMS,
	MSG_EMAIL,
	MSG_LONG,
	MSG_UNKNOWN
} MSG_TYPE_T;

typedef struct
{
	WCHAR		address[51];
	UINT8		addr_type; // 0 -SMS, 1 - EMAIL, 2 - No, 3 - Long Msg ??? 
	WCHAR		contents[512]; // максимальный размер 0x7BFE, только для SMS, думаю, это много
} SEND_TEXT_MESSAGE_T;

#ifdef __cplusplus
extern "C" {
#endif

// прочитать сообщение по id
// ответ должен придти по ивенту EV_MSG_READ 
UINT8 DL_DbMessageGenericGetMessage(	IFACE_DATA_T	*iface_data, 
										MSG_FOLDER_T	msg_folder, 
										UINT16			msg_id,
										UINT16			part_num, // 0 or 1
										UINT8			unk ); // = 0

// получить список id сообщений
UINT8 DL_DbMessageGenericGetMessageIDList(	MSG_FOLDER_T msg_folder,
											UINT16		*total_number,
											UINT16		*msg_id_list, // спиcок id сообщений, максимум 245
											UINT8		unk ); // = 0


// колличество непрочитанных смс
UINT32 MsgUtilGetUnreadMsgsInAllFolders( UINT16 *msg_total );
UINT32 MsgUtilGetUnreadMsgs(UINT16 *msgs_count, MSG_FOLDER_T folder, MSG_TYPE_MSG_T type);
UINT32 DL_DbMessageGenericGetTotalMessage(
	MSG_FOLDER_T folder_id,
	MSG_ATTR_T message_attribute,
	UINT16 *number_of_messages_ptr,
	MSG_MEMORY_STORAGE_T storage
);

// Отправка сообщений 
UINT32 DL_SigMsgSendTextMsgReq(	IFACE_DATA_T			*port, 
								SEND_TEXT_MESSAGE_T		*msg_ptr );

#ifdef __cplusplus
}
#endif

#endif // __SDK_SMS_H__
