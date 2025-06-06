UpdDisplInjection %addr_disp% 0x00000004
{
	UpdDisplInjection.bin +0
	{
		UpdateDisplayInjection.o(UpdateDisplayInj)
	}
}

patch %addr_main% 0x00011BBC
{
	patch.bin +0 FIXED
	{
		veneers.o(Veneers, +FIRST)
		*.o(+RO)
		*.a(+RO)
		font.o(+RO, +LAST)
		logo.o(+RO, +LAST)
		palette.o(+RO, +LAST)
	}

	ven +0
	{
		*(Veneer$$Code)
	}
}

RAM %addr_block% 0x000003E9
{
	RAM1 +0 0x00000200
	{
		*.o(+RW,+BSS,+ZI)
	}
}
