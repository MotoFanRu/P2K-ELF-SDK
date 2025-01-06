/*
 * Project:
 *   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
 *
 * About:
 *   Various C preprocessor hacks for the "Simula" project.
 *
 * Author:
 *   EXL, 06-Jan-2025
 */

#ifndef HACKS_H
#define HACKS_H

#define SEEK_WHENCE_SET                (SEEK_SET)

#define UtilLogStringData(...)         fprintf(stderr, __VA_ARGS__)
#define DL_FsReadFile(a, b, c, d, e)   !fread(a, b, c, d)
#define DL_FsFSeekFile(a, b, c)        fseek(a, b, c)
#define suAllocMem(a, b)               malloc(a)
#define suFreeMem(a)                   free(a)
#define memclr(a, b)                   memset(a, 0, b)

/* BE => LE conversions. */
#if defined(SIMULA)
#define B16(x)                         bswap_16(x)
#define B32(x)                         bswap_32(x)
#else
#define B16(x)                         (x)
#define B32(x)                         (x)
#endif

#endif /* HACKS_H */
