
		IMPORT	elfpackEntry
		IMPORT	UpdateDisplayInjection


		AREA Veneers, CODE, READONLY


		CODE16

		EXPORT	elfpackEntry_ven
elfpackEntry_ven
		PUSH	{LR}
		BL		elfpackEntry
		POP		{R3}
		BX		R3

		LTORG


		CODE32

		EXPORT	UpdateDisplayInjection_ven
UpdateDisplayInjection_ven
		STMFD	SP!, {LR}
		BL		UpdateDisplayInjection
		LDR		LR, [SP],#4
		BX		LR

		LTORG


		END
