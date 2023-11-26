// UTF-8 w/o BOM

#ifndef RES_DEF_H
#define RES_DEF_H

// -----------------------------------------------------------------------------------------
// RES_TYPE_GRAPHICS

#define RES_GIF_FOLDER				0x120000CE
#define RES_GIF_FILE_VIDEO			0x120000D5
#define RES_GIF_FILE_AUDIO			0x120000CD
#define RES_GIF_FILE_IMAGE			0x120000CF
#define RES_GIF_SELECTED			0x120000CC

#define RES_GIF_ICQ_ONLINE_STATUS	0x1200059A

#define RES_GIF_IMAGE_BIG			(0x12000000 | 474)
#define RES_GIF_IMAGE_BIG_2			(0x12000000 | 475)
#define RES_GIF_NEW_IMAGE			(0x12000000 | 477)
#define RES_GIF_NEW_ALBUM			(0x12000000 | 478)

#define RES_GIF_ARROW				(0x12000000 | 1429)
#define RES_GIF_2_ARROWS			(0x12000000 | 1497)

#define RES_GIF_DELETE				(0x12000000 | 9)
#define RES_GIF_FAIL				(0x12000000 | 24)
#define RES_GIF_OK					(0x12000000 | 25)
#define RES_GIF_WAIT				(0x12000000 | 26)
#define RES_GIF_INFO				(0x12000000 | 27)
#define RES_GIF_ATTENTION			(0x12000000 | 69)
#define RES_GIF_DOWNLOAD			(0x12000000 | 85)
#define RES_GIF_MSG					(0x12000000 | 88)
#define RES_GIF_QUESTION			(0x12000000 | 524)
#define RES_GIF_SPEAKER				(0x12000000 | 528)
#define RES_GIF_EMAIL				(0x12000000 | 675)
#define RES_GIF_CALENDAR			(0x12000000 | 1387)
#define RES_GIF_KEY					(0x12000000 | 1453)
#define RES_GIF_VOICE_MESSAGE		(0x12000000 | 1852)

#define RES_GIF_15x15_SAVE			(0x12000000 | 1652)
#define RES_GIF_15x15_FAIL			(0x12000000 | 656)
#define RES_GIF_15x15_INFO			(0x12000000 | 657)
#define RES_GIF_15x15_ATENTION		(0x12000000 | 658)
#define RES_GIF_15x15_OK			(0x12000000 | 660)


// -----------------------------------------------------------------------------------------
// RES_TYPE_STRING

#define LANG_NONE					0x01000874	// "(None)"
#define LANG_OK						0x010002F4	// Ok
#define LANG_ABOUT					0x01000898	// "О продукте..."
#define LANG_ABOUT1					0x0100049E  // О Программе
#define LANG_ACT_RENAME				0x01000877
#define LANG_ADD_BOOKMARK			0x010007DE
#define LANG_ADDED					0x01000C89
#define LANG_ALL_SELECTED_FILES		0x01000B38
#define LANG_APP_ERROR				0x010004B1
#define LANG_AUTHOR					0x01000ADF	// "Автор"
#define LANG_AUTHOR_				0x01000B28	// "Автор:"
#define LANG_AUTHOR1				(LANG_AUTHOR_)
#define LANG_BACK					0x01001927
#define LANG_BEGIN					0x010009F0	// Начало
#define LANG_BITRATE				0x01000DD5  // Битрейт:
#define LANG_BLUETOOTH				0x01000F66	// Bluetooth
#define LANG_BLUETOOTH1				0x01000379	// Bluetooth
#define LANG_BOOKMARKS				0x0100113C
#define LANG_BOOKMARKS_FOOL			0x01001497
#define LANG_BYTE					0x0100080C	// "байт"
#define LANG_CODE					0x01000E9E // "Код:"
#define LANG_COMPLETE				0x01000558 // Завершено
#define LANG_CONNECT				0x0100136F
#define LANG_CONNECTING				0x01001372
#define LANG_COPY					0x010009E1	// Copy
#define LANG_COPY1					0x010001C4 //
#define LANG_COPYED					0x010009E5	// "Скопировано"
#define LANG_CREATE					0x01000D34  // Создать
#define LANG_CUT					0x010009E2	// Cut
#define LANG_CUTED					0x010009E6	// "Вырезано"
#define LANG_DEL1					0x01000078 //
#define LANG_DELETE					0x010009D6	// Delete
#define LANG_DELETE_ALL				0x010001CA	// Удалить всё
#define LANG_DELETED				0x010007A1
#define LANG_DELETED1				0x01000267
#define LANG_DELETING				0x01000ACF
#define LANG_DISCONNECT				0x01001370
#define LANG_DONE					0x01000A0D	// Готово
#define LANG_DONE1					0x01000A1E
#define LANG_DONE2					0x010002E1
#define LANG_EDIT					0x01000D8C
#define LANG_EMPTY					0x01000398
#define LANG_ERROR					0x010004B8
#define LANG_EXIT					0x01000A64	// Exit
#define LANG_EXIT1					0x010002E3	// Exit
#define LANG_FILE_LENGTH			0x01000B2A  // Продолжительность:
#define LANG_FILE_SIZE				0x01000A49	// "Размер файла:"
#define LANG_FILE_TYPE				0x01000A48	// "Тип файла:"
#define LANG_FILENAME				0x0100086F
#define LANG_FILES					0x01001160 // файл(ов)
#define LANG_FOLDER					0x01000D43	// "Папка"
#define LANG_FOLDER1				0x01000A14
#define LANG_FOR					0x0100057B
#define LANG_FREE					0x01000B03	// "Свободно:"
#define LANG_GROUP					0x010015B0	// "Group"
#define LANG_ICQ					0x01001596
#define LANG_ICQ_NUMBER				0x01001597
#define LANG_IP_					0x01001817	// "IP:"
#define LANG_LIST					0x01000A38
#define LANG_LOADING___				0x01000A28
#define LANG_MEDIA					0x01000DAD
#define LANG_MEMORY_CARD			0x0100152D	// "Memory Card"
#define LANG_MENU					0x01000BCB
#define LANG_MOVE					0x01000D59 // Переместить
#define LANG_NAME					0x010000DB	// "Имя"
#define LANG_NAME1					0x010007B9	// "Имя:"
#define LANG_NO_CONTENT				0x01000996
#define LANG_NOT_SUPPORTED			0x0100028A
#define LANG_OFF_SETTINGS			0x0100137C
#define LANG_PASS_WORD				0x010000F5
#define LANG_PASS_WORD_				0x010004B5
#define LANG_PAUSE					0x01000A2A
#define LANG_PHONE_MEMORY			0x010000F9	//
#define LANG_PHONEBOOK				0x010000FE  // Справочник
#define LANG_PLAY					0x010005BE  // Воспр.
#define LANG_QUOTATION_MARK			0x010004D9
#define LANG_RELOCATE_TO			0x01000583
#define LANG_RENAME					0x01000877  // Переименовать
#define LANG_RUN					0x01001283	// Запустить
#define LANG_SAVE					0x01000375
#define LANG_SAVE1					0x01000211	//  Сохранить
#define LANG_SAVE_AS				0x01000AD8
#define LANG_SETTINGS				0x01000129 // "Параметры"
#define LANG_SHOW					0x0100007D
#define LANG_SECONDS				0x01000122	// "секунд"
#define LANG_SHORTCUTS				0x0100012C  // Метки
#define LANG_SIZE					0x0100087D	// "Размер"
#define LANG_SIZE_					0x01000808	// "Размер:"
#define LANG_SIZE1					(LANG_SIZE_)
#define LANG_SPACE					0x01000DD2
#define LANG_START					0x01001283
#define LANG_STATE					0x01001375
#define LANG_STOP					0x01000A2B
#define LANG_STORE					0x01000211
#define LANG_STORED					0x0100052C  // Сохранено
#define LANG_SYNC_WITH_SERVER		0x01000D7E // Синхронизация с сервером
#define LANG_TIME					0x0100080A	// "Время:"
#define LANG_TOTAL					0x01000B10	// "Total:"
#define LANG_TYPE					0x01000157	// "Тип"
#define LANG_TYPE2					0x01000B11
#define LANG_USED					0x01000B14	// "Used:"
#define LANG_VERSION				0x0100071D	// "Версия"
#define LANG_VERSION1				0x01000809	// "Версия:"
#define LANG_VIDEO					0x01000DB0
#define LANG_VOLUME					0x01000164  // Громкость
#define LANG_WAIT					0x01000588  // Подождите
#define LANG_WRONG_PW				0x01000BC7

#define LANG_YES					0x0100016A
#define LANG_NO						0x010000E6

#define LANG_SELECT_COLOR			0x01000829
#define LANG_TBL_RECORDS			0x01000826
#define LANG_SELECT					0x0100180C
#define LANG_ADD					0x01000474  // Добавить
#define LANG_NEXT					0x01000475
#define LANG_AUDIO					0x01000ADE  // Аудио
#define LANG_CANCEL					0x01000E9A
#define LANG_CHANGE					0x010002D9

#define LANG_DISCARD_CHANGES		0x01000D3E
#define LANG_NOT_FOUND				0x010000E8
#define LANG_STATE_					0x010006F6





// [19B1 - 1A84] & >
#if defined(FTR_L7E)
#define LANG_CLOSE					0x01001D10  // Закрыть
#define LANG_ADD					0x0100197D  // Добавить 
#define LANG_HIDDEN					0x01002706  // Скрытый
#define LANG_AUDIO					0x01000ADE  // Аудио
#define LANG_PROPERTIES				0x01001CD8
#define LANG_FILETYPE				0x01001CDF
#define LANG_WRONG_UIN_PW			0x01001F23
#define LANG_WRONG_UIN				0x010019B2
#define LANG_DISCONNECTED_BY_ICQ	0x010019D7
#elif defined(FTR_L9)
#define LANG_CLOSE					0x01001CBB  // Закрыть
#define LANG_ADD					0x01000DDC  // Добавить
#define LANG_HIDDEN					0x01001E20
#define LANG_AUDIO					0x01000DAC
#define LANG_PROPERTIES				0x01001CED
#define LANG_FILETYPE				0x01001CC2
#define LANG_WRONG_UIN_PW			0x01001D68
#define LANG_DISCONNECTED_BY_ICQ	0x010019BB
#endif



// -----------------------------------------------------------------------------------------
// RES_TYPE_ACTION

// 0x0600yyyy										// soft/list
#define RES_ACTION_BACK				0x06000001	// 11
#define RES_ACTION_CALL				0x06000002	// 11
#define RES_ACTION_CALLBACK			0x06000003	// 01
#define RES_ACTION_CANCEL			0x06000004	// 11
#define RES_ACTION_CHANGE			0x06000005	// 11
#define RES_ACTION_CLEANUP_ALL		0x06000006	// 01
#define RES_ACTION_CNF_NEW			0x06000007	// 10
#define RES_ACTION_CONFIRM_NO		0x06000008	// 11
#define RES_ACTION_CONFIRM_YES		0x06000009	// 11
#define RES_ACTION_DELETE			0x0600000A	// 01
#define RES_ACTION_EDIT				0x0600000B	// 01
#define RES_ACTION_SETTING			0x0600000C	// 01
#define RES_ACTION_DELETE2			0x0600000D	// 11
#define RES_ACTION_DELETE_ALL		0x0600000E	// 01
#define RES_ACTION_DELETE3			0x0600000F	// 10

#define RES_ACTION_SAVE				0x06000018	// 11
#define RES_ACTION_DONE				0x06000019	// 11	// to left_soft
#define RES_ACTION_EDIT_1			0x0600001A	// 11
#define RES_ACTION_OK				0x0600001D	// 11


// -----------------------------------------------------------------------------------------
// RES_TYPE_FIELD

#define RES_FIELD_NUMBER			0x0D00001A
#define RES_FIELD_PASS_WORD			0x0D0001AD
#define RES_FIELD_VOLUME			0x0D0002E2
#define RES_FIELD_NAME				0x0D00019B




#endif	// RES_DEF_H
