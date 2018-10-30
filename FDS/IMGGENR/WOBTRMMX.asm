.686
.MMX
.XMM

_DATA		SEGMENT	PARA PUBLIC USE32 'DATA'
align 4
TempMMXVar				dq  0
Delta_Left_U            dd  0
Delta_Left_V            dd  0
Delta_Left_R            dd  0
Delta_Left_G            dd  0
Delta_Left_B            dd  0
Delta_Right_U           dd  0
Delta_Right_V           dd  0
Delta_Right_R           dd  0
Delta_Right_G           dd  0
Delta_Right_B           dd  0
Left_U					dd  0
Left_V                  dd  0
Left_R					dd  0
Left_G                  dd  0
Left_B                  dd  0
Right_U                 dd  0
Right_V                 dd  0
Right_R                 dd  0
Right_G                 dd  0
Right_B                 dd  0
du                      dd  0
dv                      dd  0
dr                      dd  0
dg                      dd  0
ddb                     dd  0
lu                      dd  0
lv                      dd  0
lr                      dd  0
lg                      dd  0
lb                      dd  0
ScanLine								dd  0
PointsList              dd  0
Texture		        			dd  0 ; this was in remark
x                       dd  0
y								        dd  0
Tebp       							dd  0
t 						      		dd  0
counter									dd  0
_WobMMX_Size	dd	ASM_FILE_END - _Grid_Texture_MapASMMX_
align 1
;ScreenWidth							equ 320
;ScreenHeight						equ 240
;PointsWidth							equ 40;ScreenWidth/8
EXTRN _MMXWobNumOfHorizontalBlocks  : DWORD 
EXTRN _MMXWobNumOfVerticalBlocks  : DWORD 
EXTRN _MMXWobBlockJump  : DWORD ;(Block Size-1)*ScreenWidth*BPP
EXTRN _MMXWobBPSL : DWORD 

;_WobPointsHeight equ 30;ScreenHeight/8
u												equ 0
v												equ 4
RGBA											equ 8
; rbg - Packed Color
ElementSize             equ 20
_DATA		ENDS


.model flat

		PUBLIC	_WobMMX_Size

		PUBLIC	_Grid_Texture_MapASMMX_

_TEXT	SEGMENT PARA PUBLIC 'CODE'
	ASSUME	cs:_TEXT, ds:_DATA

_Grid_Texture_MapASMMX_:
    mov       [Tebp],ebp
    mov       [ScanLine],edi
    mov       [Texture],ecx
    mov       [PointsList],esi
	mov eax,[_MMXWobNumOfHorizontalBlocks]
	mov [x],eax
	mov eax,[_MMXWobNumOfVerticalBlocks]
	mov [y],eax

@@DrawLoop:
		mov edi, [_MMXWobNumOfHorizontalBlocks]
		mov eax,[esi+u]
		inc edi
		mov ecx,edi
		shl edi ,4
		shl ecx,2

		mov ebx,[esi+v]
		mov [Left_U],eax
		add edi,ecx
		mov [Left_V],ebx
;		mov		  ecx,[esi+ElementSize+ElementSize*PointsWidth+u] ; yes, yes , i know it's looks a LITTLE too much , but then , it's concludes in one add in the end ;)
;		mov		  edx,[esi+ElementSize+ElementSize*PointsWidth+v]
		mov		  ecx,[esi+edi+u] ; yes, yes , i know it's looks a LITTLE too much , but then , it's concludes in one add in the end ;)
		mov		  edx,[esi+edi+v]
		sub		  ecx,eax
		sub		  edx,ebx
		sar		  ecx,3
		mov		  eax,[esi+ElementSize+u]
		sar		  edx,3
		mov		  [Delta_Left_U],ecx
		mov		  [Delta_Left_V],edx
		mov		  ebx,[esi+ElementSize+v]
		mov		  [Right_U],eax
		mov		  [Right_V],ebx
;		mov		  ecx,[esi+ElementSize*2+ElementSize*PointsWidth+u]
;		mov		  edx,[esi+ElementSize*2+ElementSize*PointsWidth+v]
		mov		  ecx,[esi + edi + u + ElementSize]
		mov		  edx,[esi + edi + v + ElementSize]
		sub		  ecx,eax
		sub		  edx,ebx
		sar		  ecx,3
		mov [Delta_Right_U],ecx
		mov [Delta_Right_V],edx

		; MMX stuff
		; MM4 - Left RGBA
		; MM5 - Delta Left RGBA 
		; MM6 - Right RGBA
		; MM7 - Delta Right RGBA 
		movq	mm4, [esi + RGBA]			; Left RGBA
		movq	mm5, [esi + edi + RGBA]		; Bottom Left RGBA
		psubw	mm5, mm4
		psraw	mm5, 3
		movq    mm6, [esi + ElementSize + RGBA]			; Right RGBA
		movq    mm7, [esi + edi + ElementSize + RGBA]	; Bottom Right RGBA
		psubw	mm7, mm6
		psraw	mm7, 3

		mov [PointsList],esi
		mov edi,[ScanLine]
		mov [counter],8
	@@Inner8:
			; MMX stuff
			; MM2 - Left RGBA
			; MM3 - Delta RGBA
			movq	mm2, mm4
			movq	mm3, mm6
			psubw   mm3, mm2
			psraw	mm3, 3
			mov eax,[Left_U]
			mov ebx,[Left_V]
			mov ecx,[Right_U]
			mov edx,[Right_V]
			sub ecx,eax
			sub edx,ebx
			sar ecx,3
			mov ebx,0
			sar edx,3
			mov [du],ecx
			mov [dv],edx
			mov ecx, edx 
			sar ecx, 32
			xor ecx ,-1
			and ecx,00FF0000h
			mov edx,0
			mov bl,byte ptr [Left_U]
			shl ebx,24
			mov bh,byte ptr [Left_V+1]
			mov eax,0
			mov bl,byte ptr [Left_V]
			mov al,byte ptr [du]
			mov ebp,0
			shl eax,24
			or  ecx,eax
			mov dl,byte ptr [Left_U+1]
			mov dh,bh
			mov eax,0
			mov al,byte ptr [du+1]
			mov cl,byte ptr [dv] ; chk this
			mov ch,byte ptr [dv+1] ; chk this
			mov ebp,eax
			mov esi,[Texture]
; inner loop (unrolled)

;				mov			eax, [esi+edx*4]
				punpcklbw	mm0, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm0, mm3
				add   ebx, ecx
;				mov   [edi],eax
				adc   edx, ebp
				mov   dh, bh

;				mov   eax, [esi+edx*4]
				punpcklbw	mm1, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm1, mm3
				packuswb	mm0,mm1
				add   ebx, ecx
;				mov   [edi+4],eax
				movq  [edi], mm0
				adc   edx, ebp
				mov   dh, bh

				punpcklbw	mm0, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm0, mm3
				add   ebx, ecx
				adc   edx, ebp
				mov   dh, bh
				punpcklbw	mm1, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm1, mm3
				packuswb	mm0,mm1
				add   ebx, ecx
				movq  [edi + 8], mm0
				adc   edx, ebp
				mov   dh, bh

				punpcklbw	mm0, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm0, mm3
				add   ebx, ecx
				adc   edx, ebp
				mov   dh, bh
				punpcklbw	mm1, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm1, mm3
				packuswb	mm0,mm1
				add   ebx, ecx
				movq  [edi+16], mm0
				adc   edx, ebp
				mov   dh, bh

				punpcklbw	mm0, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm0, mm3
				add   ebx, ecx
				adc   edx, ebp
				mov   dh, bh
				punpcklbw	mm1, [esi+edx*4]
				mov   dh,0
				pmulhuw		mm1, mm3
				packuswb	mm0,mm1
				add   ebx, ecx
				movq  [edi+24], mm0
				adc   edx, ebp
				mov   dh, bh

; inner loop end (unrolled)
			sub		  edi,32
			add       edi,[_MMXWobBPSL]
			paddw	  mm4, mm5		;	Left RGBA
			paddw	  mm6, mm7		;	Left RGBA
			mov       eax,[Left_U]
			mov       ebx,[Left_V]
			mov       ecx,[Delta_Left_U]
			mov       edx,[Delta_Left_V]
			add       eax,ecx
			add       ebx,edx
			mov       [Left_U],eax
			mov       [Left_V],ebx
			mov       ebx,[Right_V]
			mov       eax,[Right_U]
			mov       ecx,[Delta_Right_U]
			mov       edx,[Delta_Right_V]
			add       eax,ecx
			add       ebx,edx
			mov       [Right_U],eax
			mov       [Right_V],ebx
			mov       esi,[Counter]
			dec       esi
			mov       [Counter],esi
			jne       @@Inner8
		mov       eax,[ScanLine]
		add       eax,32
		mov       [ScanLine],eax
		mov       esi,[PointsList]
		mov       ebx,[x]
		dec       ebx
		mov       [x],ebx
	jne       @@DrawLoop
		mov		  ecx, [_MMXWobNumOfHorizontalBlocks]
		mov		  [x],ecx
		mov       ecx,[ScanLine]
		mov       ebx,[y]
		add		  ecx, [_MMXWobBlockJump];(8-1)*ScreenWidth*4
		add		  esi,ElementSize
		mov       [PointsList],esi
		mov       [ScanLine],ecx
		dec       ebx
		mov       [y],ebx
	jne       @@DrawLoop
    mov       ebp,[Tebp]
    emms      
    ret       

ASM_FILE_END	equ	$

_TEXT		ENDS
END
