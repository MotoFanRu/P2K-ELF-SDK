
		IMPORT DAL_WriteDisplayRegion
		IMPORT cbkDisplayHookPre
		IMPORT cbkDisplayHookPost
		IMPORT UpdateDisplayInjection_ven


		AREA UpdateDisplayInj, CODE, READONLY
		CODE32

		BL UpdateDisplayInjection_ven

		LTORG


;ATIRasterizer::UpdateDisplay
;    ...
; 0x107C5A2C:
;   AND     R3, R0, #0xFF
;   ADD     R0, SP, #0x20+var_1C
;   MOV     R2, R6
;   MOV     R1, R5
;   BL      DAL_WriteDisplayRegion   ; << BL    UpdateDisplayInjection
;   MOV     R0, R4
;   BL      Invalidate__9RectangleFv ; Rectangle::Invalidate((void))
;   B       loc_107C59E4

;==========================================

		AREA UpdateDisplayInjCode, CODE, READONLY
		CODE32

		EXPORT UpdateDisplayInjection
UpdateDisplayInjection
		
		STMFD	SP!, {R0-R3,LR}
		
		ADD		LR, PC, #4
		LDR		R3, =cbkDisplayHookPre
		BX		R3
		
		LDMFD	SP, {R0-R3}
		
		BL		DAL_WriteDisplayRegion
		
		LDMFD	SP!, {R0-R3}
		
		ADD		LR, PC, #4
		LDR		R3, =cbkDisplayHookPost
		BX		R3
		
		LDR		LR, [SP],#4
		BX		LR

		LTORG


		END
