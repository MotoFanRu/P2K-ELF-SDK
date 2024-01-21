// UTF-8 w/o BOM

#ifndef JAVA_SDK_H
#define JAVA_SDK_H

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif


// Приостановить мидлет
UINT32 JavaSuspendApp( void );

// Возобновить мидлет
UINT32 JavaResumeApp( void );

// Запущена ли Java-приложение или нет
UINT32 JavaAppRunning( void );


#define JAVA_APP_ID		UINT32

/*
Правила составления номера мидлета
номер_файла << 8 | 0x20001
*/
#define DL_FS_MID_T_to_JAVA_APP_ID(fs_mid, java_app) { java_app = fs_mid << 8 | 0x20001}


UINT32 JavaStartApp( JAVA_APP_ID java_id,
				WCHAR * str,	// unknown
				UINT64	unk1,	// =0
				UINT32	arg_0,	// =0
				SU_PORT_T port
				);
//UINT32 JavaNotifyAlarm( WCHAR * title,  );

JAVA_APP_ID JavaGetMidletSuite(JAVA_APP_ID java_id);
JAVA_APP_ID JavaGetMIDletID(WCHAR *suite_name, WCHAR *midlet_name, WCHAR *vendor_name);

// вывод текста в яве
extern WCHAR	jsapi_marquee_text[];

UINT32 JavaStartMarquee( const WCHAR *text );
UINT32 JavaStopMarquee( const WCHAR *text ); // text = jsapi_marquee_text

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
