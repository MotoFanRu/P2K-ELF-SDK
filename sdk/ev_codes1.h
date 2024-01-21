// UTF-8 w/o BOM

#ifndef EV_CODES_H
#define EV_CODES_H

// USB-кабель
#define EV_DEVICE_ATTACH			0x1000
#define EV_DEVICE_DETACH			0x1001

// анимация
#if !defined(FTR_V600)
#define	EV_FRAME_COMPLETED		 	0x20F1 // фрейм выполнен
#define	EV_CYCLE_COMPLETED		 	0x20F2 // цикл(5 фреймов) выполнен
#define	EV_ANIMATION_COMPLETED	 	0x20F3 // анимация(15 фреймов) выполнена
#else
#define	EV_FRAME_COMPLETED		 	0x20F0 // фрейм выполнен
#define	EV_CYCLE_COMPLETED		 	0x20F1 // цикл(5 фреймов) выполнен
#define	EV_ANIMATION_COMPLETED	 	0x20F2 // анимация(15 фреймов) выполнена
#endif

#define EV_GRANT_TOKEN				30
#define EV_REVOKE_TOKEN				31

#define EV_KEY_PRESS				500 	// key_pressed
#define EV_KEY_RELEASE				501 	// key_pressed


#define UIS_EV_INK_KEY_PRESS 		0x2034
#define UIS_EV_INK_KEY_RELEASE		(UIS_EV_INK_KEY_PRESS + 1)
#define CANVAS_EV_KEY_PRESS			UIS_EV_INK_KEY_PRESS
#define CANVAS_EV_KEY_RELEASE		UIS_EV_INK_KEY_RELEASE
#define EV_INK_KEY_PRESS			UIS_EV_INK_KEY_PRESS
#define EV_INK_KEY_RELEASE			UIS_EV_INK_KEY_RELEASE

// нажатие на качельку громкости в GUI
#if !defined(FTR_V600)
#define EV_VOL_CHANGE_REQUEST 		0x20CA
#else
#define EV_VOL_CHANGE_REQUEST 		0x20C9
#endif

// Возникает при завершении работы диалога
#define EV_DIALOG_DONE            0x201C
// Возникает при нажатии на кнопку "Назад"
#define EV_DONE                   0x2020
#if !defined(FTR_V600)
// Возникает при нажатии джойстика вверх/вниз в списке
#define EV_LIST_NAVIGATE          0x2043 // index
// Возникает при нажатии джойстика в списке или кнопки "Выбор"
#define EV_SELECT                 0x2058 // index
#else
// Возникает при нажатии джойстика вверх/вниз в списке
#define EV_LIST_NAVIGATE          0x2042 // index
// Возникает при нажатии джойстика в списке или кнопки "Выбор"
#define EV_SELECT                 0x2057 // index
#endif
// Посылается списками чтобы запросить очередную порцию данных
#define EV_REQUEST_LIST_ITEMS     0x202C // list_items_req
#if !defined(FTR_V600)
// Посылается запросами
#define EV_NO                     0x2046
#define EV_YES                    0x2079
#else
// Посылается запросами
#define EV_NO                     0x2045
#define EV_YES                    0x2078
#endif
// Отмена
#define EV_CANCEL				  0x2008 

// Приняты данные
#define EV_DATA                   0x200F // EVENT_T::attachment
// ?
#define EV_BROWSE                 0x2006

// Возникает при изменении пользователем какого-либо поля
#define EV_DATA_CHANGE            0x2011 // index
// Запросить данные
#define EV_REQUEST_DATA           0x202B // data_req

// Сработал таймер
#define EV_TIMER_EXPIRED			0x8205A // (DL_TIMER_DATA_T*)EVENT_T::attachment

// получение фокуса
#define EV_GAIN_FOCUS			  20 
// потеря фокуса
#define EV_LOSE_FOCUS 			  21 

#define EV_RENDER 				  25
// закрытие заставки
#define EV_USER_ACTIVITY		  0x7F1
// регистрация в сети
#define EV_REG_NETWORK			0x8200B 
// выключение телефона
#define EV_POWER_DOWN				0x11
// вызывается из APP_DONE
#define EV_APP_DONE					0x0C

#if defined(FTR_L7E)
	#define EV_CREATE_SHORTCUT			0x20C9	// Зажатие меню
#elif defined(FTR_V600)
	#define EV_CREATE_SHORTCUT			0x20C4	// Зажатие меню
#else
	#define EV_CREATE_SHORTCUT			0x20C5	// Зажатие меню
#endif

#define WS_EVENT				0x82143 // в аттаче WS_EVENT_T
#define EV_MSG_READ				0x82068 // в аттаче MSG_READ_T

#define EV_SS_OPERATION 0x82028
#define EV_SS_USSD_NOTIFY 0x82039
#define EV_SS_USSD_REQUEST 0x8203A
#define EV_SS_USSD_COMPLETE 0x8203B

#define EV_DSMA_PROXY_CREATE	0x821B1
#define EV_DSMA_PROXY_STATE		0x821B2
#define EV_DSMA_BUFF_STATE		0x821B3
#define EV_DSMA_PROXY_ATTR		0x821B4

#define EV_DBK_ASYNC_OPERATION		0x8206A
#define EV_DBK_ADD_EVENT			0x8206B
#define EV_DBK_DATABASE_QUERY		0x8206C		// ответ на запрос
#define EV_DBK_DATABASE_SLOT		0x8206D
#define EV_DBK_DATABASE_ALTERED		0x8206E		// какие-то изменения в базе
#define EV_DBK_DATABASE_ALARM		0x8206F		
#define EV_DBK_NUM_OF_EVENTS		0x82070
#define EV_DBK_DATABASE_IDLE		0x820D5		// база ничем не занята
#define EV_DBK_WEEK_VIEW			0x82126		// ответ на запрос недельного вида
#define EV_DBK_MONTH_VIEW			0x822CF		// ответ на запрос месячного вида

// пропущенный вызов
#ifdef PATCH 
	extern const UINT32 EV_MISSEDCALLS;
	#define EV_MISSED_CALLS (EV_MISSEDCALLS)
#else
	extern const UINT32 *EV_MISSEDCALLS;
	#define EV_MISSED_CALLS (*EV_MISSEDCALLS)
#endif

// получение фокуса рабочим столом
#ifdef PATCH 
	extern const UINT32 EV_IDLE_GAIN_FOCUS;
	#define EV_IDLE_FOCUS			(EV_IDLE_GAIN_FOCUS)
#else
	extern const UINT32 *EV_IDLE_GAIN_FOCUS;
	#define EV_IDLE_FOCUS			(*EV_IDLE_GAIN_FOCUS)
#endif

// входящий звонок
// в аттаче номер телефона
#ifdef PATCH 
	extern const UINT32 CALL_ALERT;
	#define EV_CALL_ALERT			(CALL_ALERT)
#else
	extern const UINT32 *CALL_ALERT;  
	#define EV_CALL_ALERT			(*CALL_ALERT)
#endif

// сработал будильник
// в аттаче номер будильника
#ifdef PATCH 
	extern const UINT16 ALMCLK_REMINDER_START; 
	#define EV_ALMCLK_REMINDER_START (ALMCLK_REMINDER_START)
#else
	extern const UINT16 *ALMCLK_REMINDER_START; 
	#define EV_ALMCLK_REMINDER_START (*ALMCLK_REMINDER_START)
#endif

// событие календаря
#ifdef PATCH 
	extern const UINT32 DATEBOOK_REMINDER;
	#define EV_DATEBOOK_REMINDER	(DATEBOOK_REMINDER)
#else
	extern const UINT32 *DATEBOOK_REMINDER;
	#define EV_DATEBOOK_REMINDER	(*DATEBOOK_REMINDER)
#endif

// выключение активности дисплея
#ifdef PATCH 
	extern const UINT32 EV_DisplayTimeout;
	#define EV_DISPLAY_NO_ACTIVE	(EV_DisplayTimeout)
#else
	extern const UINT32 *EV_DisplayTimeout;
	#define EV_DISPLAY_NO_ACTIVE	(*EV_DisplayTimeout)
#endif

// включение дисплея
#ifdef PATCH 
	extern const UINT32 EV_DisplayActivity;
	#define EV_DISPLAY_ACTIVE	(EV_DisplayActivity)
#else
	extern const UINT32 *EV_DisplayActivity;
	#define EV_DISPLAY_ACTIVE	(*EV_DisplayActivity)
#endif

// эвент блокировки клавы
#ifdef PATCH 
	extern const UINT32 EV_LOC_KPAD;
	#define EV_KEYPAD_LOCK 			(EV_LOC_KPAD)
#else
	extern const UINT32 *EV_LOC_KPAD;
	#define EV_KEYPAD_LOCK 			(*EV_LOC_KPAD)
#endif

//эвент разблокировки клавы
#ifdef PATCH 
	extern const UINT32 EV_UNLOC_KPAD;
	#define EV_KEYPAD_UNLOCK		(EV_UNLOC_KPAD)
#else
	extern const UINT32 *EV_UNLOC_KPAD;
	#define EV_KEYPAD_UNLOCK		(*EV_UNLOC_KPAD)
#endif

// отправка файлов по Bluetooth
#ifdef PATCH 
	extern const UINT32 EVENT_BT;
	#define EV_BT_SEND				(EVENT_BT)
#else
	extern const UINT32 *EVENT_BT;
	#define EV_BT_SEND				(*EVENT_BT)
#endif

// запуск предпросмотра при приеме файлов по Bluetooth и wap
// в аттаче имя принимаемого файла и имя временного файла с путём
#ifdef PATCH 
	extern const UINT32 EV_PREVIEW_START; 
	#define EV_START_PREVIEW		(EV_PREVIEW_START)
#else
	extern const UINT32 *EV_PREVIEW_START; 
	#define EV_START_PREVIEW		(*EV_PREVIEW_START)
#endif

#ifdef PATCH
	extern const UINT32 EV_UIS_REFRESH;
	#define EV_REQUEST_RENDER		(EV_UIS_REFRESH)
#else
	extern const UINT32 *EV_UIS_REFRESH;
	#define EV_REQUEST_RENDER		(*EV_UIS_REFRESH)
#endif

// офсетт по которому лежит IMEI
#ifdef PATCH 
	extern const UINT32 SEEM_IMEI;
	#define DB_FEATURE_IMEI				(SEEM_IMEI)
#else
	extern const UINT32 *SEEM_IMEI;
	#define DB_FEATURE_IMEI				(*SEEM_IMEI)
#endif

// офсетт по которому лежит IMSI
#ifdef PATCH 
	extern const UINT32 SEEM_IMSI;
	#define DB_FEATURE_IMSI				(SEEM_IMSI)
#else
	extern const UINT32 *SEEM_IMSI;
	#define DB_FEATURE_IMSI				(*SEEM_IMSI)
#endif

// состояние клавиатуры: 1 - заблок. 0 - разблок
#ifdef PATCH 
	extern const UINT32 KEYPAD_STATE;
	#define DB_FEATURE_KEYPAD_STATE		(KEYPAD_STATE)
#else
	extern const UINT32 *KEYPAD_STATE;
	#define DB_FEATURE_KEYPAD_STATE		(*KEYPAD_STATE)
#endif

#if defined(FTR_V600)
	#define EV_USER_ACTIVITY_TIMEOUT 0x074F
	#define EV_SCREENSAVER_TIMEOUT 0x0751
	#define EV_DISPLAY_TIMEOUT 0x0752
	#define EV_BACKLIGHT_TIMEOUT 0x0753
	#define EV_INACTIVITY_TIMEOUT 0x0754
#else
	#define EV_USER_ACTIVITY_TIMEOUT 0x07EE
	#define EV_SCREENSAVER_TIMEOUT 0x07F0
	#define EV_DISPLAY_TIMEOUT 0x07F1
	#define EV_BACKLIGHT_TIMEOUT 0x07F2
	#define EV_INACTIVITY_TIMEOUT 0x07F3
#endif

#define EV_ADD_MISSED_CALL 0x8201B
#define EV_REMOVE_MISSED_CALL 0x0398

#define EV_FLIP_CLOSED 0x20BE
#define EV_FLIP_OPENED 0x20BF

#endif // EV_CODES_H
