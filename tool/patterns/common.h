#ifndef COMMON__H
#define COMMON__H

#if !defined(WIN32)
#include <xmmintrin.h>
#endif

#include "parser.h"
#include "ramtrans.h"


#define CG1_OFFSET_DEFAULT		0x10080000
#define LENGTH_ALIGNMENT		16
#define LENGTH_ALIGNMENT_MASK	(LENGTH_ALIGNMENT-1)
#define MAX_PATTERNS			1024


static inline u32 E32(u32 val) {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_bswap32(val);
#elif defined(_MSC_VER)
	return _byteswap_ulong(val);
#else
	// Fallback for other compilers
	return ((val & 0xFF000000) >> 24) |
		   ((val & 0x00FF0000) >>  8) |
		   ((val & 0x0000FF00) <<  8) |
		   ((val & 0x000000FF) << 24);
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

long findAllPatternsSSEa(void);
u32* findPatternSSEa(PATTERN_T *pattern);

void libpatInit();
void libpatTerm();

void translateValue(PATTERN_T *pat, u32 *value);
u32 createPattern(PATTERN_T *pattern, PARSED_DATA_T *pdata);
u32 addPattern(PARSED_DATA_T *pdata);
void freePattern(PATTERN_T *pattern);

extern u32					gCG1Offset;
extern u8					*gBuffer;
extern long					gBufLength;
extern PATTERN_T			*gPatterns;
extern int					gPatCount;

extern LIBPAT_LDSTAT		gStats;
extern u32					gStatTotalLen;

extern libpatCallback		gStatusCallback;
extern u32					*gFindPatternResult;

extern BOOL					gRamTransEnabled;
extern BOOL					gCrawlStarted;


#ifdef __cplusplus
}
#endif




#endif
