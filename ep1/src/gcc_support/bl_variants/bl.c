void funcT(unsigned int num_pairs, unsigned int cnt);
void funcA(unsigned int num_pairs, unsigned int cnt);
void funcTL(unsigned int num_pairs, unsigned int cnt);
void funcAL(unsigned int num_pairs, unsigned int cnt);

#ifdef DEFINES
#define funcT  ((void (*)(unsigned int, unsigned int)) (0x10867508 | 1))
#define funcA  ((void (*)(unsigned int, unsigned int)) (0x10867500 | 0))
#define funcTL ((void (*)(unsigned int, unsigned int)) (0x00000100 | 1))
#define funcAL ((void (*)(unsigned int, unsigned int)) (0x00000200 | 0))
#endif

void UtilLogStringData(void) {
	int a;
	a = 0;
	funcT(a, 0x5151);
	a = 1;
	funcA(a, 0x5151);
	a = 2;
	funcTL(a, 0x5151);
	a = 3;
	funcAL(a, 0x5151);
}
