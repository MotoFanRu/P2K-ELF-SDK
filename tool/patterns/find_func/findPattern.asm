.686
.model flat,stdcall
option casemap:none
.xmm

INCLUDE findPatterns.inc

.data

matches		DD		MAX_MATCHES DUP(0)

.code

findPatternSSEa PROC C pattern:DWORD
	LOCAL	i, j, nmatch, pbuflen
	ASSUME	esi:PTR PATTERN_T
	
			push	esi
			push	edi
			push	ebx

			
			mov		nmatch, 0

			mov		esi,	pattern

			movzx	eax, [esi].len				; gPatterns[k].length
			mov		pbuflen, eax
			
			mov		ebx, [esi].patmask			; gPatterns[k].mask
			mov		edx, [esi].pattext			; gPatterns[k].text
			
			movaps	xmm4, [ebx]					; mask
			movaps	xmm5, [edx]					; pat

			
		mainloopstart:
		
			mov		j, 0
			;mov		j, 2968DEh
			;mov		j, 28DCACh
			
			;mov		j, 0A99C30h

			
		
		preloadtext:
		; fill the primary buffer
			mov		edi, j
			add		edi, gBuffer
			movaps	xmm1, [edi]			; primary
			

		loopJ:
			mov		eax, j
			cmp		eax, gBufLength
			jge		finished
			
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

			mov		[esi].found, bx
			
			cmp		[esi].nmatch, 0				; single-match pattern check
			je		singlematch

			cmp		[esi].nmatch, bx			; multi-match pattern check
			jne		loopJnext
			
			; pat.nmatch == nmatch => f=FOUND, continue
			mov		eax, j
			mov		[esi].value, eax
			or		[esi].flags, PFLAG_FOUND
			jmp		finished

			
		singlematch:	;ebx = nmatch

			cmp		nmatch, MAX_MATCHES
			jge		exit

			dec		ebx
			lea		eax, [OFFSET matches + ebx * (TYPE matches)]
			mov		ebx, j
			mov		[eax], ebx

			or		[esi].flags, PFLAG_FOUND
			
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
			

		finished:

			;cmp		gStatusCallback, 0
			;je		exit

			;mov		eax, gStatusCallback
			;invoke	(TYPE libpatCallback) PTR eax, esi, 0

		exit:
			mov		eax, OFFSET matches
			
			
			pop		ebx
			pop		edi
			pop		esi
			

		ret
findPatternSSEa endp

end
