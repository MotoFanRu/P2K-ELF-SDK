
#ifndef AUTORUN_H
#define AUTORUN_H

#include "ElfLoader.h"

#define PARS_DONE				0
#define PARS_SKIP				1
#define PARS_EOF				2

extern void  ElfLoaderStart(void);

void  AutorunMain(void);

UINT32  ParseString(char* buf, UINT32* pindex, W_CHAR *uri, W_CHAR *params);
void    SkipLine(char *buf, UINT32 *pindex);

UINT32 DL_AudPlayTone( UINT32 tone,  UINT8 volume );

#endif
