// UTF-8 w/o BOM

#ifndef _CONSTS_H_
#define _CONSTS_H_

// dl events
#define EV_DEVICE_ATTACH          0x1000
#define EV_DEVICE_DETACH          0x1001
#define EV_REG_NETWORK            0x1002
#define EV_SHORTCUT_READ_RECORD   0x1003
#define EV_SHORTCUT_READ_URL      0x1004
#define EV_TIMER_EXPIRED          0x1005
#define EV_PHONEBOOK_READ_RECORD  0x1006
#define EV_WEBSESSION_OPERATION   0x1007
#define EV_DBK_ADD_EVENT          0x1008
#define EV_DBK_DATABASE_QUERY     0x1009
#define EV_MSG_READ               0x100A
#define EV_USSD_NOTIFY            0x100B
#define EV_USSD_REQUEST           0x100C
#define EV_USSD_COMPLETE          0x100D
#define EV_USSD_OPERATION         0x100E
#define EV_DBK_MONTH_VIEW         0x100F
#define EV_DBK_WEEK_VIEW          0x1010
#define EV_DSMA_PROXY_CREATE      0x1011
#define EV_DSMA_PROXY_STATE       0x1012
#define EV_DSMA_BUFF_STATE        0x1013
#define EV_DSMA_PROXY_ATTR        0x1014
#define EV_WEBSESSION_CHANGE      0x1015

// calls
#define EV_CALLS_INCOMING         0x1050
#define EV_CALLS_TERMINATED       0x1051
#define EV_CALLS_MISSED           0x1052
#define EV_CALL_ALERT             0x1053

// display
#define EV_DISPLAY_NO_ACTIVE      0x1100
#define EV_DISPLAY_ACTIVE         0x1101
#define EV_USER_ACTIVITY          0x1102
#define EV_RENDER                 0x1103
#define EV_REQUEST_RENDER         0x1104

// keypad
#define EV_KEYPAD_LOCK            0x1150
#define EV_KEYPAD_UNLOCK          0x1151

// uis events: navigate, data, actions...
#define EV_VOL_CHANGE_REQUEST     0x1200
#define EV_DIALOG_DONE            0x1201
#define EV_DONE                   0x1202
#define EV_LIST_NAVIGATE          0x1203
#define EV_SELECT                 0x1204
#define EV_REQUEST_LIST_ITEMS     0x1205
#define EV_NO                     0x1206
#define EV_YES                    0x1207
#define EV_CANCEL                 0x1208
#define EV_DATA                   0x1209
#define EV_BROWSE                 0x120A
#define EV_DATA_CHANGE            0x120B
#define EV_REQUEST_DATA           0x120C
#define EV_PB_SET_PERCENT         0x120D
#define EV_PB_SET_VALUE           0x120E
#define EV_PB_SET_NAME            0x120F

// afw events: focus, tokens, power
#define EV_GRANT_TOKEN            0x1300
#define EV_REVOKE_TOKEN           0x1301
#define EV_IDLE_FOCUS             0x1302
#define EV_GAIN_FOCUS             0x1303
#define EV_LOSE_FOCUS             0x1304
#define EV_POWER_DOWN             0x1305

// keypress
#define EV_KEY_PRESS              0x1350
#define EV_KEY_RELEASE            0x1351
#define EV_INK_KEY_PRESS          0x1352
#define EV_INK_KEY_RELEASE        0x1353

// other apps events
#define EV_CREATE_SHORTCUT        0x1400
#define EV_ALMCLK_REMINDER_START  0x1401
#define EV_DATEBOOK_REMINDER      0x1402
#define EV_MSG_NOTIF              0x1403
#define EV_PHBK_SEARCH            0x1404
#define EV_BT_SEND                0x1405
#define EV_START_PREVIEW          0x1406
#define SC2ELF_MAIN_REGISTER      0x1407
#define EXTMGR_MAIN_REGISTER      0x1408
#define SOCKET_MGR_MAIN_REGISTER  0x1409
#define WEATHER_MGR_MAIN_REGISTER 0x140A
#define EV_BT_POWER               0x140B

// canvas
#define EV_FRAME_COMPLETED        0x1450
#define EV_CYCLE_COMPLETED        0x1451
#define EV_ANIMATION_COMPLETED    0x1452

// mme
#define EV_MME_OPEN_SUCCESS       0x1500
#define EV_MME_OPEN_ERROR         0x1501
#define EV_MME_SEEK_SUCCESS       0x1502
#define EV_MME_SEEK_ERROR         0x1503
#define EV_MME_PLAY_COMPLETE      0x1504
#define EV_MME_CLOSE_COMPLETE     0x1505
#define EV_MME_STOP_COMPLETE      0x1506
#define EV_MME_STOPPED_AT_TIME    0x1507

// elfpack events
#define EV_LDR_ELFLIST_CHANGED    0x1900  // don't change!
#define EV_PM_API_EXIT            0x1901

// Features for DL
#define BEGIN_4A__IN_DB           0x2000
#define SEEM_IMEI                 0x2001
#define SEEM_IMSI                 0x2002
#define KEYPAD_STATE              0x2003
#define SESSION_TIME              0x2004
#define SENDED_TRAFFIC            0x2005

// Values for UIS_SetStatus
#define GPRS_STATUS               0x2100
#define NETWORK_STATUS            0x2101
#define ITUNES_STATUS             0x2102
#define AUDIO_STATUS              0x2103
#define READY_STATUS              0x2104
#define PROVIDER_NAME_STATUS      0x2105
#define CALL_MISSED_STATUS        0x2106

// UIS dialog types for UIS_GetActiveDialogType
#define UIS_NULL_DIALOG_TYPE      0x2200

#define UIS_FONT_INFO             0x2300

#endif
