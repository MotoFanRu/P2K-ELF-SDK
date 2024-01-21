#ifndef __EP2_FEATURES_H__
#define __EP2_FEATURES_H__


#ifndef FTR_
#define FTR_
#endif

#ifndef FTR_PHONE_PLATFORM
#define FTR_PHONE_PLATFORM		"LTE2"
#endif

#ifndef FTR_PHONE_NAME
#define FTR_PHONE_NAME			"L7e"
#endif

#ifndef FTR_EVBASE_CODE
#define FTR_EVBASE_CODE			0x2A000
#endif

#ifndef FTR_LIB_PATH_1
#define FTR_LIB_PATH_1			L"file://b/Elf2/libs/"
#endif

#ifndef FTR_LIB_PATH_2
#define FTR_LIB_PATH_2			L"file://c/Elf2/libs/"
#endif

#ifndef FTR_LIB_PATH_3
#define FTR_LIB_PATH_3			L"file://a/Elf2/libs/"
#endif

#ifndef FTR_AUTORUN_URI
#define FTR_AUTORUN_URI			L"file://b/Elf2/auto.run"
#endif

#ifndef FTR_FASTRUN_ELF
#define FTR_FASTRUN_ELF			L"file://b/Elf2/fastrun.elf"
#endif

#ifndef FTR_FASTRUN_EVENT
#define FTR_FASTRUN_EVENT		0x2A010
#endif

#ifndef FTR_CONSOLE_EVENT
#define FTR_CONSOLE_EVENT		0x2A020
#endif

#ifndef FTR_CONFIG_URI1
#define FTR_CONFIG_URI1			L"file://b/Elf2/elfpack.cfg"
#endif

#ifndef FTR_CONFIG_URI2
#define FTR_CONFIG_URI2			L"file://c/Elf2/elfpack.cfg"
#endif

#ifndef FTR_LIBRARY_URI
#define FTR_LIBRARY_URI			L"file://b/Elf2/library.bin"
#endif

#ifndef FTR_NOAUTORUN_KEY
#define FTR_NOAUTORUN_KEY		KEY_0
#endif

#ifndef FTR_FAST_KEY
#define FTR_FAST_KEY			KEY_FAST_ACCESS
#endif

#ifndef FTR_REPLACED_REGISTER_FUNC
#define FTR_REPLACED_REGISTER_FUNC		APP_SYNCMLMGR_MainRegister
#endif

#ifndef FTR_LOG_FILE_URI
#define FTR_LOG_FILE_URI		L"file://b/ep2.log"
#endif

#ifndef FTR_EV_EP1_LOAD_ELF
#define FTR_EV_EP1_LOAD_ELF		0xA001
#endif


#define KP_NONE					0x00
#define KP_TWO_POLE				0x01
#define KP_THREE_POLE			0x02

#ifndef FTR_KEYPAD_TYPE
#define FTR_KEYPAD_TYPE			KP_NONE
#endif

#ifndef FTR_DUMPS_DIR
#define FTR_DUMPS_DIR			L"file://b/"
#endif


#endif /* __EP2_FEATURE_H__ */
