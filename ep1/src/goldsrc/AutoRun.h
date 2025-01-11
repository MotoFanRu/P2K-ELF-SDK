/*
 * Project:
 *   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
 *
 * About:
 *   Entry point + Autorun file parser.
 *
 * Author:
 *   Andy51, 01-Nov-2007
 *
 * Autorun file "auto.run" format:
 *   ; This is comment\r\n
 *   <empty string>\r\n
 *   file://a/Elf/Some.elf\r\n
 *   file://a/Elf/Some.elf 0x1234\r\n
 */

#ifndef AUTO_RUN_H
#define AUTO_RUN_H

#include <typedefs.h>

#define PARS_DONE                      (0)
#define PARS_SKIP                      (1)
#define PARS_EOF                       (2)

extern void AutorunMain(void);
extern void ElfLoaderStart(void);

extern UINT32 ParseString(char *buf, UINT32 *pindex, WCHAR *uri, WCHAR *params);
extern void SkipLine(char *buf, UINT32 *pindex);

extern void APP_SyncML_MainRegister(void);

#endif /* AUTO_RUN_H */
