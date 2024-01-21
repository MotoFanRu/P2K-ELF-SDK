#ifndef LANGSTRINGS_H
#define LANGSTRINGS_H

// -----------------------------------------------------------------------------------------
// RES_TYPE_STRING

#if !defined(FTR_L7E)
	#define LANG_CLOSE				0x01001CBB  // Закрыть
	#define LANG_ADD				0x01000DDC  // Добавить
	#define LANG_PROPERTIES			0x01001CED  // Свойства 
	#define LANG_HIDDEN				0x01001E20  // Скрытый 
	#define LANG_FILETYPE			0x01001CC2  // Тип файла
	#define LANG_AUDIO				0x01000DAC  // Аудио
#else
	#define LANG_CLOSE				0x01001D10  // Закрыть 
	#define LANG_ADD				0x0100197D  // Добавить 
	#define LANG_PROPERTIES			0x01001CD8  // Свойства 
	#define LANG_HIDDEN				0x01002706  // Скрытый
	#define LANG_FILETYPE			0x01001CDF  // Тип файла 
	#define LANG_AUDIO				0x01000ADE  // Аудио
#endif

#define LANG_SYNC_WITH_SERVER   0x01000D7E // Синхронизация с сервером

#define LANG_ERROR				0x010004B8	// Ошибка
#define LANG_COMPLETE			0x01000558 // Завершено

#define LANG_EXIT				0x01000A64	// Выход
#define LANG_BACK				0x01001927  // Назад
#define LANG_MENU				0x01000BCB // Меню

#define LANG_DELETE				0x01000078	// Удалить
#define LANG_DELETE_ALL			0x010001CA	// Удалить всё
#define LANG_COPY				0x010001C4	// Копировать
#define LANG_CUT				0x010009E2	// Вырезать


#define LANG_MOVE				0x01000D59  // Переместить
#define LANG_RENAME				0x01000877  // Переименовать
#define LANG_SAVE				0x01000211	//  Сохранить
#define LANG_SAVE_AS			0x01000AD8	//  Сохранить как
#define LANG_STORED				0x0100052C  // Сохранено
#define LANG_CREATE				0x01000D34  // Создать
#define LANG_RUN				0x01001283	// Запустить
#define LANG_USE				0x01000E0A // Применить

#define LANG_SETTINGS			0x01000129 // "Параметры"

#define LANG_BLUETOOTH			0x01000379	// Bluetooth

#define LANG_COPYED				0x010009E5	// "Скопировано"
#define LANG_CUTED				0x010009E6	// "Вырезано"

#define LANG_BEGIN				0x010009F0	// Начало

#define LANG_FILENAME			0x0100086F  // Имя файла


#define LANG_MEDIA				0x01000DAD  // Медиа
#define LANG_VIDEO				0x01000DB0  // Видео

#define LANG_PLAY				0x010005BE  // Воспр.

#define LANG_FILES				0x01001160  // файл(ов)
#define LANG_FOLDER				0x01000D43	// "Папка"

#define LANG_TYPE				0x01000157	// "Тип"
#define LANG_SIZE				0x0100087D	// "Размер"
#define LANG_FILE_TYPE			0x01000A48  // Тип файла:
#define LANG_FILE_SIZE			0x01000A49  // Размер файла:
#define LANG_FILE_LENGTH		0x01000B2A  // Продолжительность:
#define LANG_BITRATE			0x01000DD5  // Битрейт:
#define LANG_TYPE2				0x01000B11	// Тип:
#define LANG_NAME 				0x010007B9	// Имя:

#define LANG_BYTE				0x0100080C	// "байт"
#define LANG_SECONDS			0x01000122	// "секунд"

#define LANG_WAIT				0x01000588  // Подождите

#define LANG_PHONEBOOK			0x010000FE  // Справочник
#define LANG_SHORTCUTS			0x0100012C  // Метки

#define LANG_VOLUME			    0x01000164  // Громкость
#define LANG_ABOUT			    0x0100049E  // О Программе
#define LANG_AUTHOR			    0x01000B28  // Автор:

#define LANG_RECORD				0x010001FB // Запись
#define LANG_STOP				0x01000A2B // Стоп

#define LANG_NEXT				0x01000A4B // Следующий

#define LANG_TRANSLATE			0x010006C7 // перевести

#define LANG_CONTACTLIST    	0x01001358 // Список контактов
#define LANG_CONNECT			0x010000BA // Соединение
#define LANG_STATUS				0x0100049F //  статус
#define LANG_ADD_CONTACT        0x0100134D // Добавить контакт
#define LANG_DEL_CONTACT        0x0100135E // Удалить контакт
#define LANG_REN_CONTACT		0x01001B30 // Изменить контакт
#define LANG_MENU_CONTACTLIST	0x0100135A // Меню списка контактов

#endif
