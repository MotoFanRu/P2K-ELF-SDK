
#ifndef LIBPAT__H
#define LIBPAT__H

#if !defined(WIN32)
#define __stdcall
#endif

#if defined(WIN32)
#ifdef LIBPAT_EXPORTS
#define LIBPAT_API __declspec(dllexport)
#else
#define LIBPAT_API __declspec(dllimport)
#endif
#else
#define LIBPAT_API
#endif

typedef unsigned char	u8;
typedef signed char		s8;
typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned long	u32;
typedef signed long		s32;

#if !defined(WIN32)
typedef u8 BOOL;
#define TRUE (BOOL)(1)
#define FALSE (BOOL)(0)
#endif

typedef struct
{
	u32			patcount;
	float		avglen;
	u32			maxlen;
	u32			len16cnt;

} LIBPAT_LDSTAT;

#pragma pack(push,1)

typedef struct
{
	char		*name;
	u8			*text;
	u8			*mask;
	u32			value;
	char		mode;
	u8			load;
	u16			length;
	u16			nmatch;
	u16			found;
	u16			flags;
	s16			offs[2];
} PATTERN_T;

#pragma pack(pop)


/* ��������� ��� �������-������� (��. libpatSetCallback).
 ��� ������������� ��������� � libpatFindAllPatterns:
 curPat - ��������� ��������� ���������
 count - ���������� ����� ��������� � ����������� ������ �������� */
typedef void (__stdcall *libpatCallback) ( PATTERN_T *curPat, u32 count );


#ifdef __cplusplus
extern "C" {
#endif


/* ��������� ��� ������ �������� ���� (CG1 ������) � ���������� ��������� �� ����,
 � ����� ������ ����� u32 *size. */
LIBPAT_API void* __stdcall libpatLoadBinary( char *path, u32 *size );


/* ��������� � ���� � ���������� � ��� ��������� ������� *.pat � ��������� ������ ������ */
LIBPAT_API BOOL __stdcall libpatLoadPatterns( char *path );


/* ������� ��� ���������� �������� �������, ������ - ��� � *.pat ����� */
LIBPAT_API BOOL __stdcall libpatAddPattern( char *pattern );


/* ��������� ���������� ������ libpatFindAllPatterns � ����� ������� *.sym */
LIBPAT_API void __stdcall libpatSaveSymfile( char *path );


/* ������������� �������� CG1. �� ��������� - 0x10080000 */
LIBPAT_API void __stdcall libpatSetOffset( u32 off );


/* ������ ����� ���� �������� ������� */
LIBPAT_API int	__stdcall libpatFindAllPatterns( );


/* ����� �������� ��������. ���������� ������ ��������� �������� � count=���-�� ��������.
 ���������� ������ ������ �������� ������ ���� � �������� �� ������ ������� ����� ���������  */
LIBPAT_API u32* __stdcall libpatFindPattern( char *pattern, u32 *count );


/* ������ libpatFindPattern ��� �������� ������ ��������� ��������. ����� ����������
 ��������� ��������, ���� 0, ���� �������� �� ������� ��� �� ������� ������ ������. */
LIBPAT_API u32 __stdcall libpatFindPatternSingle(char *pattern);


/* ���������� ���������� �� �������� ��������� */
LIBPAT_API void __stdcall libpatGetStats( LIBPAT_LDSTAT *result );


/* ������������� �������-�������, ������� ����� ���������� ��� ���������� ���������
 �������� ��� ������ �������� libpatFindAllPatterns.
 ����� � NULL ��������� ������� */
LIBPAT_API void __stdcall libpatSetCallback( libpatCallback fn );


/* ���������� ��� �������� �������� */
LIBPAT_API void __stdcall libpatResetPatterns( );


/* ��������/��������� ��������� ���������� RamTrans-�� (������� ��� ���������� ������������� � IRAM ������ ������)
 �� ��������� ��������� */
LIBPAT_API void __stdcall libpatEnableRamTrans(BOOL enable);


/* ��������� ������ �� ������ �������, ������� ��� ������ �������-�������. */
LIBPAT_API void __stdcall libpatCrawlStart();


/* ��������� �������� ������ �� ������ �������. ���������� �� �������-�������. */
LIBPAT_API void __stdcall libpatCrawlStop();

LIBPAT_API void __stdcall libpatInit();
LIBPAT_API void __stdcall libpatTerm();
LIBPAT_API void RamTransTerm();

#ifdef __cplusplus
}
#endif






#endif
