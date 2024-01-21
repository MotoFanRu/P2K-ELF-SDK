IFNDEF FINDPATTERNS_INC
FINDPATTERNS_INC equ <1>

;gotoxy				PROTO C :DWORD, :DWORD
printf				PROTO C :VARARG


PFLAG_NOTFOUND		EQU		00b
PFLAG_FOUND			EQU		01b
PFLAG_FAIL			EQU		10b

MAX_MATCHES			EQU		1024

		

PATTERN_T struct
		patname		DWORD	?
		pattext		DWORD	?
		patmask 	DWORD	?
		value		DWORD	?
		mode		BYTE	?
		load		BYTE	?
		len			WORD	?
		nmatch		WORD	?
		found		WORD	?
		flags		WORD	?
		offs		WORD	2 DUP(?)
PATTERN_T ends


libpatCallback TYPEDEF PROTO STDCALL :DWORD,:DWORD

EXTERN C	gBufLength:DWORD, gBuffer:DWORD, gPatCount:DWORD, gPatterns:DWORD, gStatusCallback:DWORD


ELSE
echo -----------------------------------------
echo WARNING Duplicate include file batmeter.inc
echo -----------------------------------------
ENDIF
