.686
.model flat,stdcall
option casemap:none
.xmm

INCLUDE findPatterns.inc


.const

fmt			db		"%d/%d, %f%%"
koef		real4	100.0

.data

pbuflen		dd		?


.code

findAllPatternsSSEa proc C
	LOCAL	i,j,k,nmatch,totalfound
	ASSUME	esi:PTR PATTERN_T
	
			push	esi
			push	edi
			push	ebx

			mov		totalfound, 0
			
		; Preload some patterns
			mov		k, 0
			
			
		loopK:
			mov		eax,	gPatCount
			cmp		k,		eax
			jge		exit

			mov		nmatch, 0
			
			mov		eax,	TYPE PATTERN_T
			mul		k
			add		eax,	gPatterns
			mov		esi,	eax


			movzx	eax, [esi].len				; gPatterns[k].length
			mov		pbuflen, eax
			
			mov		ebx, [esi].patmask			; gPatterns[k].mask
			mov		edx, [esi].pattext			; gPatterns[k].text
			
			movaps	xmm4, [ebx]					; mask
			movaps	xmm5, [edx]					; pat
			
						
;			cmp		eax, 2						; length
;			jl		endcache
			
;			add		ebx, 16
;			add		edx, 16
			
;		cachexmm2:
;			movaps	xmm6, [ebx]	; mask
;			movaps	xmm7, [edx]	; pat

;		endcache:

			
		mainloopstart:
		
			mov		j, 0
			;mov		j, 2968DEh
			;mov		j, 28DCACh
			
			;mov		j, 0A99C30h

			
		
		preloadtext:
		; fill the primary and secondary buffers
			mov		edi, j
			add		edi, gBuffer
			movaps	xmm1, [edi]			; primary
			;movaps	xmm2, [edi+16]		; secondary
			

		loopJ:
			mov		eax, j
			cmp		eax, gBufLength
			jge		loopKnext
			
			and		eax, 15				; load text every 16 bytes
			cmp		eax, 0
			jne		skipload
			
			
		loadtext:
			; fill the secondary buffer
			movaps	xmm2, [edi+16]		; text

			
		skipload:
			; copy the primary buffer to the workbuffer
			movaps	xmm0, xmm1
			
			; mask
			andps	xmm0, xmm4

			; pat
			pcmpeqd	xmm0, xmm5
			;cmpneqps xmm0, xmm5			;erroneous in some cases

			pmovmskb eax, xmm0
			;movmskps eax, xmm0				;harder to convert eax (4 bits value)

			not		ax
			cmp		eax, 0
			jne		loopJnext				; no match
				
		; is there anything more we have to compare?
			cmp		pbuflen, 16
			jle		havematch
			
		; DEBUG!!
			;jmp		havematch
			
		longpat:
			; now we have a long pattern >16 bytes long
			
			movzx	ecx, [esi].len				; gPatterns[k].length
			sub		ecx, 16						; already processed
			shr		ecx, 3						; length/8
			
			mov		ebx, [esi].patmask			; gPatterns[k].mask
			mov		edx, [esi].pattext			; gPatterns[k].text
			
			add		ebx, 16
			add		edx, 16
			push	edi
			add		edi, 16
			mov		eax, 0

			
			longpatloop:
				movq	mm0, QWORD PTR [edi]	; text
				pand	mm0, QWORD PTR [ebx]	; mask
				pcmpeqd mm0, QWORD PTR [edx]	; pattern
				pmovmskb eax, mm0
				not		al
				
				cmp		eax, 0
				jne		longpatloopnomatch		; at least one qword did not match
				
				add		ebx, 8
				add		edx, 8
				add		edi, 8
				
				loop	longpatloop	
				pop		edi
				jmp		havematch

			longpatloopnomatch:
				pop		edi
				jmp		loopJnext
			
		havematch:
			inc		nmatch
			
			mov		ebx, nmatch					; EBX = nmatch
			
			cmp		[esi].nmatch, 0				; single-match pattern check
			je		singlematch

			cmp		[esi].nmatch, bx			; multi-match pattern check
			jne		loopJnext
			
			; pat.nmatch == nmatch => f=FOUND, continue
			mov		eax, j
			mov		[esi].value, eax
			or		[esi].flags, PFLAG_FOUND
			jmp		loopKnext

			
		singlematch:

			cmp		ebx, 1
			; nmatch > 1 => f=NOTFOUND, continue
			jg		singlematch_nmatchGR1
			
			; nmatch == 1 => f=FOUND, scan next
			mov		eax, j
			mov		[esi].value, eax
			or		[esi].flags, PFLAG_FOUND
			jmp		loopJnext
			
			; nmatch > 1
			singlematch_nmatchGR1:
				and		[esi].flags, not PFLAG_FOUND
				jmp		loopKnext

		loopJnext:

			inc		j
			inc		edi
			
		; shift the buffers. shifts inverted because of little endian
			psrldq	xmm1, 1				; primary >> 8
			movaps	xmm0, xmm2			; secondary -> work buf
			pslldq	xmm0, 15			; temp << 120
			orps	xmm1, xmm0			; shift 1 byte from the secondary buffer to the primary
			psrldq	xmm2, 1				; secondary >> 8

			jmp		loopJ
			
		loopKnext:
			inc		k

		checkfound:
			test	[esi].flags, PFLAG_FOUND
			jz		printprogress
		
			inc		totalfound

		printprogress:
;			mov		eax, j
;			and		eax, 4095			; j mod 4096
;			cmp		eax, 0
;			jne		loopJ

;			invoke	gotoxy, 0,5

			emms
;			fild	k
;			fidiv	gPatCount
;			fmul	koef
;			sub		esp, 8
;			fstp	QWORD PTR [esp]

;			push	gPatCount
;			push	k
;			push	OFFSET fmt
;			call	printf
;			add		esp, 20

			cmp		gStatusCallback, 0
			je		loopK

			mov		eax, gStatusCallback
			invoke	(TYPE libpatCallback) PTR eax, esi, k

			jmp		loopK

		exit:
			mov		eax, totalfound
			
			
			pop		ebx
			pop		edi
			pop		esi
			

		ret
findAllPatternsSSEa endp

end
