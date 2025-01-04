/*
 * Project:
 *   EP1, EG1: ElfPack 1.x by Andy51 for Motorola P2K phones.
 *
 * About:
 *   Various experiments with ARM/Thumb Branch Links.
 *
 * Author:
 *   EXL, 30-Dec-2024
 */

void funcTS(unsigned int num_pairs, unsigned int cnt);
void funcAS(unsigned int num_pairs, unsigned int cnt);
void funcTL(unsigned int num_pairs, unsigned int cnt);
void funcAL(unsigned int num_pairs, unsigned int cnt);

#if defined(LIB_DEFINES)
#define funcTS ((void (*)(unsigned int, unsigned int)) (0x10867508 | 1))
#define funcAS ((void (*)(unsigned int, unsigned int)) (0x10867500 | 0))
#define funcTL ((void (*)(unsigned int, unsigned int)) (0x00000100 | 1))
#define funcAL ((void (*)(unsigned int, unsigned int)) (0x00000200 | 0))
#endif

void AutorunMain(void) {
	int a;

	a = 0;
	funcTS(a, 0x5151);
	a = 1;
	funcAS(a, 0x5151);
	a = 2;
	funcTL(a, 0x5151);
	a = 3;
	funcAL(a, 0x5151);
}
