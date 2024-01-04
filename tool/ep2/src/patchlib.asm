;
; TODO: хранить в sym[i].st_name сразу адрес строки (DONE)
;

			EXPORT patchLib

			AREA patchLib, DATA, READONLY
			ALIGN 4

LIBNAME_MAXLEN	EQU		31
PATHLIB_CNT		EQU		37
strTab0			EQU		0

; Header
name		DCB		"PatchLib", 0
name0		SPACE	(LIBNAME_MAXLEN + 1 - (name0-name))

count		DCD		PATHLIB_CNT
refs		DCD		0
link		DCD		0
strTab		DCD		strTab0_
physBase	DCD		0

Symbols		DCD		strTab0_ - strTab0
			DCD		ldrGetElfsList

			DCD		strTab1 - strTab0
			DCD		ldrFindElf

			DCD		strTab2 - strTab0
			DCD		ldrIsLoaded

			DCD		strTab3 - strTab0
			DCD		ldrRequestEventBase

			DCD		strTab4 - strTab0
			DCD		ldrSendEvent

			DCD		strTab5 - strTab0
			DCD		ldrSendEventToApp

			DCD		strTab6 - strTab0
			DCD		ldrSendEventToAppEmpty

			DCD		strTab7 - strTab0
			DCD		ldrLoadElf

			DCD		strTab8 - strTab0
			DCD		ldrUnloadElf

			DCD		strTab9 - strTab0
			DCD		UtilLogStringData

			DCD		strTab10 - strTab0
			DCD		u_utoa

			DCD		strTab11 - strTab0
			DCD		ldrDisplayCbkReg

			DCD		strTab12 - strTab0
			DCD		ldrDisplayCbkUnReg

			DCD		strTab13 - strTab0
			DCD		ldrGetConstVal

			DCD		strTab14 - strTab0
			DCD		ldrInitEventHandlersTbl

			DCD		strTab15 - strTab0
			DCD		ldrFindEventHandlerTbl

			DCD		strTab16 - strTab0
			DCD		cprint

			DCD		strTab17 - strTab0
			DCD		cprintf

			DCD		strTab18 - strTab0
			DCD		ldrGetPhoneName

			DCD		strTab19 - strTab0
			DCD		ldrGetPlatformName

			DCD		strTab20 - strTab0
			DCD		ldrGetFirmwareVersion

			DCD		strTab21 - strTab0
			DCD		ldrToggleConsole

			DCD		strTab22 - strTab0
			DCD		unloadElf

			DCD		strTab23 - strTab0
			DCD		dlopen

			DCD		strTab24 - strTab0
			DCD		dlclose

			DCD		strTab25 - strTab0
			DCD		dlsym

			DCD		strTab26 - strTab0
			DCD		dlerror

			DCD		strTab27 - strTab0
			DCD		ldrGetAhiDrvInfo

			DCD		strTab28 - strTab0
			DCD		ldrGetAhiDevice

			DCD		strTab29 - strTab0
			DCD		ldrSetDefaultConfig

			DCD		strTab30 - strTab0
			DCD		ldrLoadConfig

			DCD		strTab31 - strTab0
			DCD		ldrGetConfig

			DCD		strTab32 - strTab0
			DCD		setConfig

			DCD		strTab33 - strTab0
			DCD		copyConfig

			DCD		strTab34 - strTab0
			DCD		freeConfig

			DCD		strTab35 - strTab0
			DCD		ldrLoadDefLibrary

			DCD		strTab36 - strTab0
			DCD		ldrEnableConsole

; StrTab
strTab0_	DCB		"ldrGetElfsList", 0
strTab1		DCB		"ldrFindElf", 0
strTab2		DCB		"ldrIsLoaded", 0
strTab3		DCB		"ldrRequestEventBase", 0
strTab4		DCB		"ldrSendEvent", 0
strTab5		DCB		"ldrSendEventToApp", 0
strTab6		DCB		"ldrSendEventToAppEmpty", 0
strTab7		DCB		"ldrLoadElf", 0
strTab8		DCB		"ldrUnloadElf", 0
strTab9		DCB		"UtilLogStringData", 0
strTab10	DCB		"u_utoa", 0
strTab11	DCB		"ldrDisplayCbkReg", 0
strTab12	DCB		"ldrDisplayCbkUnReg", 0
strTab13	DCB		"ldrGetConstVal", 0
strTab14	DCB		"ldrInitEventHandlersTbl", 0
strTab15	DCB		"ldrFindEventHandlerTbl", 0
strTab16	DCB		"cprint", 0
strTab17	DCB		"cprintf", 0
strTab18	DCB		"ldrGetPhoneName", 0
strTab19	DCB		"ldrGetPlatformName", 0
strTab20	DCB		"ldrGetFirmwareVersion", 0
strTab21	DCB		"ldrToggleConsole", 0
strTab22	DCB		"ldrUnloadElfByAddr", 0
strTab23	DCB		"dlopen", 0
strTab24	DCB		"dlclose", 0
strTab25	DCB		"dlsym", 0
strTab26	DCB		"dlerror", 0
strTab27	DCB		"ldrGetAhiDrvInfo", 0
strTab28	DCB		"ldrGetAhiDevice", 0
strTab29	DCB		"ldrSetDefaultConfig", 0
strTab30	DCB		"ldrLoadConfig", 0
strTab31	DCB		"ldrGetConfig", 0
strTab32	DCB		"ldrSetConfig", 0
strTab33	DCB		"ldrCopyConfig", 0
strTab34	DCB		"ldrFreeConfig", 0
strTab35	DCB		"ldrLoadDefLibrary", 0
strTab36	DCB		"ldrEnableConsole", 0

; Imports
			IMPORT	ldrGetElfsList
			IMPORT	ldrFindElf	
			IMPORT	ldrIsLoaded
			IMPORT	ldrRequestEventBase
			IMPORT	ldrSendEvent
			IMPORT	ldrSendEventToApp
			IMPORT	ldrSendEventToAppEmpty
			IMPORT	ldrLoadElf
			IMPORT	ldrUnloadElf
			IMPORT	UtilLogStringData
			IMPORT	u_utoa
			IMPORT	ldrDisplayCbkReg
			IMPORT	ldrDisplayCbkUnReg
			IMPORT	ldrGetConstVal
			IMPORT	ldrInitEventHandlersTbl
			IMPORT	ldrFindEventHandlerTbl
			IMPORT	cprint
			IMPORT	cprintf
			IMPORT	ldrGetPhoneName
			IMPORT	ldrGetPlatformName
			IMPORT	ldrGetFirmwareVersion
			IMPORT	ldrToggleConsole
			IMPORT	unloadElf
			IMPORT	dlopen
			IMPORT	dlclose
			IMPORT	dlsym
			IMPORT	dlerror
			IMPORT	ldrGetAhiDrvInfo
			IMPORT	ldrGetAhiDevice
			IMPORT	ldrSetDefaultConfig
			IMPORT	ldrLoadConfig
			IMPORT	ldrGetConfig
			IMPORT	setConfig
			IMPORT	copyConfig
			IMPORT	freeConfig
			IMPORT	ldrLoadDefLibrary
			IMPORT	ldrEnableConsole

			END

