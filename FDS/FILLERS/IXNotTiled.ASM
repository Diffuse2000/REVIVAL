;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CODING LOG
;; fabricators operative 29.06.02
;; interpolation levels -
;;	level 0 - preprocessing
;;	level 1 - outer/outer (ngon)
;;	level 2 - outer/inner (section)
;;	level 3 - inner/outer (span)
;;	level 4 - inner/inner (interval)
;; 29.06.02 mapper works with the following characteristics
;;		flat spanloop. 
;;		interpolation level: ZBuffer:1, Gouraud:2, Texture:2.
;; Current: spanloop preperations / interface definition
;; 02.07.02
;;		gouraud zbuffer spanloop.
;;		interpolation level: ZBuffer:4, Gouraud:4, Texture:2.
;;		ZBuffer interpolation errors
;; 03.07.02 corrections
;; - added interpolation across span sections
;; - corrected variable ordering for constant d/dX
;; - corrected x prestepping
;; - corrected gouraud interpolation innerloop on nodraw
;; - zBuffering operational
;; - zBuffering has precision problems
;; 22.07.02
;; + fixed a bug in subspan d[uv]dx (used psubw instead of psubd!!) 
;;  - total work = 12 coder hours
;; - interpolation level: ZBuffer:4, Gouraud:4, Texture:4.
;; + added self-modifying code support
;;  - mapper works on all rectangular textures up to 256x256.
;; TODO list
;; + add support for generic texture block scribbling 
;;	- inner/outer, write mask update code + SMC
;;	- inner/inner, write mask update SMC
;; - precision correction: use a N-R iteration
;;   x=rcpss(y), x = x+x - y*x*x;  (or: x += x*(1-y*x))
;; - test removal of (u,v) update within inner/outer. 
;;   this will cause error of (3 + log(#spans))bit in the fractional coordinate.
;;	 using small fixups, expected error can be reduced to 
;;   (3 + log(log(#spans)) bits.
;; - test robustness of wBuffer vs zBuffer.
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; ASSEMBLER VERSION OF THE IX MAPPER ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;										;;
;	Highly optimized texture/gouraud     ;
;   mapper using PentiumIII w/ SSE		 ;
;		Supports Z-buffering			 ;
;;										;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; All rights reserved to Flood inc.  ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; ASSEMBLER DIRECTIVES ;;;
.686
.MMX
.XMM

_DATA   SEGMENT PARA PUBLIC 'DATA'
align 16

;;; Left/Right Section ;;;
	extrn	_VESA_BPSL		:	DWORD
	extrn	_PageSize		:	DWORD
	extrn	_XRes			:	DWORD
	extrn   _g_zscale256	:	DWORD
	extrn	_g_zscale		:	DWORD

	;;; Left section
	Left_RZ			dd		0.0
	Left_UZ			dd		0.0
	Left_VZ			dd		0.0
	Left_X			dd		0.0
					
	Left_dRZ		dd		0.0
	Left_dUZ		dd		0.0
	Left_dVZ		dd		0.0
	Left_dX			dd		0.0
					
	Left_R			dd		0.0
	Left_G			dd		0.0
	Left_B			dd		0.0
	Left_A			dd		0.0
					
	Left_dR			dd		0.0
	Left_dG			dd		0.0
	Left_dB			dd		0.0
	Left_dA			dd		0.0
					
	; Right section
	Right_R			dd		0.0
	Right_G			dd		0.0
	Right_B			dd		0.0
	Right_A			dd		0.0

	Right_dR		dd		0.0
	Right_dG		dd		0.0
	Right_dB		dd		0.0
	Right_dA		dd		0.0

	; Constant deltas over X
	_dRZdx		dd		0.0
	_dUZdx		dd		0.0
	_dVZdx		dd		0.0	
	_filler1	dd		0.0
	; Scaled deltas over X
	_sdRZdx		dd		0.0
	_sdUZdx		dd		0.0
	_sdVZdx		dd		0.0	
	_filler2	dd		0.0
	; Lighting deltas over X for current scan line
	_dRdx		dw		0
	_dGdx		dw		0
	_dBdx		dw		0
	_dAdx		dw		0


; innerloop variables (cacheline 0)
	dudx			dd		0
	dvdx			dd		0
	SubspanZ		dd		0
	SubspanCounter	dd		0
	Right_X			dd		0.0
	Right_dX		dd		0.0
	save_esp		dd		0
	align32			dd		0

; innerloop variables (cacheline 1)

; outerloop variables (cacheline 0? this isn't aligned)
	Left_Height		dd		0.0
	Left_ScanLines	dd		0
	Left_Vert		dd		0

	Right_Height	dd		0.0
	Right_rHeight	dd		0.0
	Right_ScanLines	dd		0
	Right_Vert		dd		0
	Right_Size		dd		0

	_SectionHeight	dd		0
	_HeightCounter  dd		0

; nonaligned variable region
	LastLogWidth	dd		0
	LastLogHeight	dd		0
	_tebp			dd		0
	t_edi			dd		0


	_Page		dd		0
	_L2Size		dd		0
	_ZBuffer	dd		0
	_Verts		dd		0

	_L2SPANSIZE	equ		4
	_SPANSIZE	equ		16
	_fSPANSIZE	equ		f16
	_pfSPANSIZE	equ		pf16

; Float tables
align	16

;	mask tables for standard textures (1x1 blocks)
	_T0		dd		000010000h,
					000030000h,
					000070000h,
					0000f0000h,
					0001f0000h,
					0003f0000h,
					0007f0000h,
					000ff0000h,

	_T1		dd		0fffe8000h,
					0fffc8000h,
					0fff88000h,
					0fff08000h,
					0ffe08000h,
					0ffc08000h,
					0ff808000h,
					0ff008000h


;	mask tables for tiled textures (4x4 blocks - 2 bits for inner tile position)
	_T0_22	dd		0000000000000001000000000000b,
					0000000000000011000000000000b,
					0000000000100011000000000000b,
					0000000011000011000000000000b,
					0000001110000011000000000000b,
					0000111100000011000000000000b,
					0011111000000011000000000000b,
					1111110000000011000000000000b,


	_T1_22	dd		0000000000000000100000000000b,
					0000000000000000100000000000b,
					1111111100011100100000000000b,
					1111110000111100100000000000b,
					1111000001111100100000000000b,
					1100000011111100100000000000b,
					1000000111111100100000000000b,
					0000001111111100100000000000b,



	f65536	equ		pf65536
	pf65536	dd		65536.0, 65536.0, 65536.0, 65536.0
	pf32768	dd		32768.0, 32768.0, 32768.0, 32768.0
	f256	equ		pf256
	pf256	dd		256.0, 256.0, 256.0, 256.0
	pf128	dd		128.0, 128.0, 128.0, 128.0
	f16		equ		pf16
	pf16	dd		16.0, 16.0, 16.0, 16.0
	pf0		dd		0.0, 0.0, 0.0, 0.0
	f1		dd		1.0	

	; reciporal table (rf0[x] == 1/x)
	rf0		dd		0.0
	rf1		dd		1.0
	rf2		dd 		0.5
	rf3		dd		0.333333
	rf4		dd		0.25
	rf5		dd		0.2
	rf6		dd		0.16666667
	rf7		dd		0.142857142857
	rf8		dd		0.125
	rf9		dd		0.11111111
	rf10	dd		0.1
	rf11	dd		0.09090909091
	rf12	dd		0.08333333
	rf13	dd		0.076923
	rf14	dd		0.071429
	rf15	dd		0.06666667
	rf16	dd		0.0625
	rf17	dd		0.0588235
	rf18	dd		0.055556
	rf19	dd		0.052632
;	rf20	dd		0.05

	; assorted values (debug)
	whitecolor dd	0ffffffh

	; Vertex Structure
	Vertex_RZ	equ		0h
	Vertex_UZ	equ		4h
	Vertex_VZ	equ		8h
	Vertex_X	equ		0Ch
	Vertex_R	equ		10h
	Vertex_G	equ		14h
	Vertex_B	equ		18h
	Vertex_A	equ		1Ch
	Vertex_Y	equ		20h
	VertexSize	equ		30h


;;; STATIC VARIABLES ;;;

	public			_IX_Code_Start
	public			_IX_Code_Size
	_IX_Code_Start	dd	ASM_FILE_START
	_IX_Code_Size	dd	ASM_FILE_END - ASM_FILE_START

;;; CODE SEGMENTS ;;;

align 1
_DATA   ENDS

_TEXT   SEGMENT PARA PUBLIC 'CODE'
	ASSUME  cs:_TEXT, ds:_DATA

;;; EXPORTED VARIABLES ;;;

public _IXAsmFiller

ASM_FILE_START	equ	$

;;;;;;;;;;;
;
; Purpose:		Calculate right section of the n-gon
; Arguments:	esi - V1
;				edi	- V2
; Returns:		Number of scan lines
;;;;;;;;;;;
CalcRightSection	proc	near

	V1		equ		esi
	V2		equ		edi

	push ebx

; calc integer number of scanlines
	movss	xmm0,	[V1 + Vertex_Y]
	movss	xmm1,	[V2 + Vertex_Y]
	cvtss2si ebx,	xmm0
	cvtss2si eax,	xmm1
	sub		eax,	ebx
	mov		[Right_ScanLines], eax

; return if scanlines = 0
	jz		JmpRet
; xmm1 = float height
	subss	xmm1,	xmm0
; calculate xmm2 = 1.0/height
; should consider revising using rcpss
;	movss   xmm2,	[f1]

; forward loading of vertex lighting
	movaps	xmm4, [V2 + Vertex_R]

;	divss	xmm2,	xmm1
	rcpss	xmm2,	xmm1

; forward loading of vertex X	xmm0 = V1 X
	movss	xmm0,	[V1 + Vertex_X]


; Right.dX = (V2->X - V1->X) * rHeight;
; sdword FPRevHeight;

	movss  xmm3,   [V2 + Vertex_X]
	subss	xmm3,	xmm0

; forward loading of vertex lighting
	movaps	xmm5,	[V1 + Vertex_R]

; xmm3 now contains X delta
	mulss   xmm3,	xmm2

; shuffle xmm2 = 1.0/height
	shufps	xmm2,	xmm2,	00000000b

; Right.dR = (V2->R - V1->R) * RevHeight;
; Right.dG = (V2->G - V1->G) * RevHeight;
; Right.dB = (V2->B - V1->B) * RevHeight;
; Right.dA = (V2->A - V1->A) * RevHeight;
; xmm4 - V2 Light	xmm5 - V1 Light
	subps	xmm4,	xmm5
; xmm4 = RGBA Deltas
	mulps   xmm4,	xmm2

;float prestep = ((float)iy1 - V1->Y);
; Right.X  = V1->X  + Left.dX  * prestep;
; Right.UZ = V1->UZ + Left.dUZ * prestep;
; Right.VZ = V1->VZ + Left.dVZ * prestep;
; Right.RZ = V1->RZ + Left.dRZ * prestep;

; ebx = ceil(V1->Y)
	cvtsi2ss	xmm1,	ebx
; load V1 X
	subss		xmm1,	[V1 + Vertex_Y]
	movss		xmm6,	xmm1
	shufps		xmm6,	xmm6,	00000000b
; multiply delta (xmm3) by prestep (xmm1)
	mulss		xmm1,	xmm3
; xmm0 = prestepped X
	addss		xmm0,	xmm1
	
; Left.R = V1->R + prestep * Left.dR;
; Left.G = V1->G + prestep * Left.dG;
; Left.B = V1->B + prestep * Left.dB;
; Left.A = V1->A + prestep * Left.dA;
; xmm5 = V1 Light	xmm6 - prestep	xmm4 - RGBA deltas
	mulps		xmm6,	xmm4
; xmm5 = prestepped RGBA
	addps		xmm5,	xmm6
	

; late storing of X and delta
	movss	[Right_X],	xmm0
	movss	[Right_dX],	xmm3

	movaps	[Right_R],	xmm5
	movaps	[Right_dR],	xmm4
; multiply RGBA(xmm5), dRGBA(xmm4) by 65536.0 for fixed point
; and store as RGBA(mm0, mm1) and dRGBA(mm2, mm3)
;	movaps  xmm0,	[f65536]
;	mulps	xmm5,	xmm0
;	mulps	xmm4,	xmm0
;	cvtps2pi mm1,	xmm5
;	cvtps2pi mm3,	xmm4
;	movhlps	xmm4,	xmm4
;	movhlps	xmm5,	xmm5
;	cvtps2pi mm0,	xmm5
;	cvtps2pi mm2,	xmm4

;	movq qword ptr [Right_R],	mm0
;	movq qword ptr [Right_B],	mm1
;	movq qword ptr [Right_dR],	mm2
;	movq qword ptr [Right_dB],	mm3


JmpRet:
	pop ebx
	ret

CalcRightSection	endp


;;;;;;;;;;;
;
; Purpose:		Calculate left section of the n-gon
; Arguments:	esi - IXVertex *V1
;				edi	- IXVertex *V2
; Returns:		Number of scan lines
;;;;;;;;;;;
CalcLeftSection		proc	near

	V1		equ		esi
	V2		equ		edi

	push ebx

; calc integer number of scanlines
	movss	xmm0,	[V1 + Vertex_Y]
	movss	xmm1,	[V2 + Vertex_Y]
	cvtss2si ebx,	xmm0
	cvtss2si eax,	xmm1
	sub		eax,	ebx
	mov		[Left_ScanLines], eax

; return if scanlines = 0
	jz		JmpRet
; xmm1 = float height
	subss	xmm1,	xmm0

; should consider revising using rcpss
; calculate xmm2 = 1.0/height
;	movss   xmm2,	[f1]

; forward loading of vertex lighting
	movaps	xmm4, [V2 + Vertex_R]

;	divss	xmm2,	xmm1
	rcpss	xmm2,	xmm1

; forward loading of vertex ZUVX	xmm0 = V1 ZUVX
	movaps	xmm0,	[V1 + Vertex_RZ]


; IX.Left.dX = (V2->X - V1->X) * RevHeight;
; IX.Left.dUZ = (V2->UZ - V1->UZ) * RevHeight;
; IX.Left.dVZ = (V2->VZ - V1->VZ) * RevHeight;
; IX.Left.dRZ = (V2->RZ - V1->RZ) * RevHeight;

	movaps  xmm3,   [V2 + Vertex_RZ]
	subps	xmm3,	xmm0

; shuffle xmm2 = 1.0/height
	shufps	xmm2,	xmm2,	00000000b

; forward loading of vertex lighting
	movaps	xmm5,	[V1 + Vertex_R]

; xmm3 now contains XUVZ deltas	
	mulps   xmm3,	xmm2

; Left.dR = (V2->R - V1->R) * RevHeight;
; Left.dG = (V2->G - V1->G) * RevHeight;
; Left.dB = (V2->B - V1->B) * RevHeight;
; Left.dA = (V2->A - V1->A) * RevHeight;
; xmm4 - V2 Light	xmm5 - V1 Light
	subps	xmm4,	xmm5
; xmm4 = RGBA Deltas
	mulps   xmm4,	xmm2

;float prestep = ((float)iy1 - V1->Y);
; Left.X  = V1->X  + Left.dX  * prestep;
; Left.UZ = V1->UZ + Left.dUZ * prestep;
; Left.VZ = V1->VZ + Left.dVZ * prestep;
; Left.RZ = V1->RZ + Left.dRZ * prestep;

; ebx = ceil(V1->Y)
	cvtsi2ss	xmm1,	ebx
; load V1 XUVZ
	subss		xmm1,	[V1 + Vertex_Y]
	shufps		xmm1,	xmm1,	00000000b
	movaps		xmm6,	xmm1
; multiply deltas (xmm3) by prestep (xmm1)
	mulps		xmm1,	xmm3
; xmm0 = prestepped XUVZ
	addps		xmm0,	xmm1
	
; Left.R = V1->R + prestep * Left.dR;
; Left.G = V1->G + prestep * Left.dG;
; Left.B = V1->B + prestep * Left.dB;
; Left.A = V1->A + prestep * Left.dA;
; xmm5 = V1 Light	xmm6 - prestep	xmm4 - RGBA deltas
	mulps		xmm6,	xmm4
; xmm5 = prestepped RGBA
	addps		xmm5,	xmm6

; late storing of XUVZ and deltas (yesh)
	movaps	[Left_RZ],	xmm0
	movaps	[Left_dRZ],	xmm3

	movaps	[Left_R],	xmm5
	movaps	[Left_dR],	xmm4
; multiply RGBA(xmm5), dRGBA(xmm4) by 65536.0 for fixed point
; and store as RGBA(mm0, mm1) and dRGBA(mm2, mm3)
;	movaps  xmm0,	[f65536]
;	mulps	xmm5,	xmm0
;	mulps	xmm4,	xmm0
;	cvtps2pi mm1,	xmm5
;	cvtps2pi mm3,	xmm4
;	movhlps	xmm4,	xmm4
;	movhlps	xmm5,	xmm5
;	cvtps2pi mm0,	xmm5

;	cvtps2pi mm2,	xmm4
;
;	movq qword ptr [Left_R],	mm0
;	movq qword ptr [Left_B],	mm1
;	movq qword ptr [Left_dR],	mm2
;	movq qword ptr [Left_dB],	mm3

JmpRet:
	pop ebx
	ret

CalcLeftSection		endp


;	Summery: Span loop Input
;	mm0, mm1:	RGB, delta
;	ebx:		span width
;	edi:		span ptr
;	esi:		Zbuffer ptr
;	xmm0:		ZUVX initial left
;	xmm2:		ZUVX constant d/dx
;	xmm3:		ZUVX scaled d/dx
SpanLoop			proc	near

	; Flat inner
comment &
	mov ecx, ebx
	mov eax, 07f7f7f7fh
	rep stosd
	ret
&

	; WE ARE FUCK'N OUT OF FREE REGISTERS!!!
	; so we use esp as a replacement.
	; why? because we can.
	; DOS INCOMPATIBLE
	mov [save_esp],esp
	mov esp,edi
;	mov [t_edi], edi

	; releases ebx.
	mov [SubspanCounter], ebx
	
	; logic
	; z = 1/interpolated rz
	; Z = 0xFF80 - Fist(g_zscale * z);

	; allocation
	; xmm4 = z
	; eax = Z, edx = dZdx
	mov			eax,	0ff80h

	rcpss		xmm1,	xmm0
;	movss		xmm1,	[f1]
;	divss		xmm1,	xmm0

	movss		xmm4,	xmm1
	mulss		xmm1,	_g_zscale
	cvtss2si	ebx,	xmm1
	sub			eax,	ebx

	; xmm4 = UV affine (16bit fractional)
	movss		xmm5,	[pf65536]
	mulss		xmm4,	xmm5
	shufps		xmm4,	xmm4,	00000000b
	mulps		xmm4,	xmm0
	shufps		xmm4,	xmm4,	00001001b

	; mm2 = [Ui:Uf]        [Vi:Vf] (frees xmm4)
	; ebp = [Ui:Uf], edi = [Vi:Vf] (frees mm2)
	cvtps2pi	mm2,	xmm4


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; placeholder: ebp/edi init


	; step ZUVX
	addps		xmm0,	xmm3
	
Outer:


	; ebp = u wwww ww00 0000 00ww 0fff ffff ffff ffff
	movd		ebp,	mm2
	movd		ebx,	mm2
	shr			ebp,	1
	
	and			ebx,	000ff0000h		; T0(logWidth)
	org			$-4
	_T0Wd0		dd		000ff0000h
	
	and			ebp,	07fffh
	or			ebp,	ebx	

	pshufw		mm2,	mm2,	01001110b

	; edi = v wwww wwww 0000 0000 0fff ffff ffff ffff
	movd		edi,	mm2
	movd		ebx,	mm2
	shr			edi,	1
	
	and			ebx,	000ff0000h      ; T0(logHeight)
	org			$-4
	_T0Hd0		dd		000ff0000h
	
	shl			ebx,	8				; logWidth
	org			$-1
	_Wb0		db		8

	and			edi,	07fffh
	or			edi,	ebx

	pshufw		mm2,	mm2,	01001110b

	; xmm4 = z
	; edx = Z
	rcpss		xmm7,	xmm0
;	movss		xmm7,	[f1]
;	divss		xmm7,	xmm0

	mov			edx,	0ff80h
	movss		xmm4,	xmm7
	mulss		xmm7,	_g_zscale
	cvtss2si	ecx,	xmm7
	sub			edx,	ecx

	; xmm4 = UV affine (16bit fractional)
	movss		xmm5,	[pf65536]
	mulss		xmm4,	xmm5
	shufps		xmm4,	xmm4,	00000000b
	mulps		xmm4,	xmm0
	shufps		xmm4,	xmm4,	00001001b

	; mm3 = [dUi:dUf] [dVi:dVf] (frees xmm4)
	; stores Z
	; edx = dzdx
	cvtps2pi	mm3,	xmm4
	movq		mm7,	mm3
	mov			[SubspanZ], edx
	psubd		mm3,	mm2
	sub			edx,	eax
	psrad		mm3,	4
	sar			edx,	4

	; not so fucked up code
	; dudx = [Ui:Uf], dvdx = [Vi:Vf] (frees mm3)

	; dudx = 1111 1111 wwww wwww 1fff ffff ffff ffff
	movd		ecx,	mm3
	movd		ebx,	mm3
	shr			ecx,	1
	
	and			ebx,	000ff0000h  ;   T0(logWidth)
	org			$-4
	_T0Wd1		dd		000ff0000h

	or			ebx,	0ff008000h	;	T1(logWidth)
	org			$-4
	_T1Wd2		dd		0ff008000h

	and			ecx,	07fffh
	or			ebx,	ecx
	mov			[dudx],	ebx

	;		 0000 0000 wwww wwww ffff ffff ffff fff*
	; dvdx = wwww wwww 1111 1111 1fff ffff ffff ffff
	pshufw		mm3,	mm3,	01001110b
	movd		ecx,	mm3
	movd		ebx,	mm3
	shr			ecx,	1
	and			ebx,	000ff0000h	;	T0(logHeight)
	org			$-4
	_T0Hd1		dd		000ff0000h

	shl			ebx,	8			;	logWidth
	org	$ - 1
	_Wb1		db		8
	and			ecx,	07fffh
	or			ebx,	000ff8000h	;	T0(logHeight) + 8000h
	org			$-4
	_T0Hd2		dd		000ff8000h
	or			ecx,	ebx
	mov			[dvdx],	ecx

	pshufw		mm3,	mm3,	01001110b

	; step ZUVX
	addps		xmm0,	xmm3

	mov			ecx,	16
	mov			ebx,	[SubspanCounter]
	cmp			ebx,	ecx
	cmova		ebx,	ecx

	lea ecx,[ebx+ebx]
	neg ecx

	sub esp, ecx
;	sub [t_edi], ecx
	sub esi, ecx
	sub esp, ecx
;	sub [t_edi], ecx

; Register allocation table

; Generic
; esp - Page ptr

; ZBuffer
; eax - current Z (low word)
; edx - dZ/dx (low word)
; esi - ZBuffer ptr
; ecx - offset counter (low byte)

; Gouraud
; mm0 - Color
; mm1 - dColor/dx

; Texture
; mm2 - u, v
; mm7 - next u, v
; Required: u, v, du, dv (8:8 each); Texture ptr (dword)
; add (u, v), (du, dv) will be made on an mmx register
; texture ptr will be smeared on an entire mmx register and added to (u, v) on a seperate reg.
; unloaded to a memory location and loaded into ebp then accessed directly using [ebp]
; or maybe we need a different plan.. because addr isn't always = u+v*256

; Free registers
; CPU: none
; MMX: mm3-6
; SSE: xmm1,xmm4-7


;align 16
	Inner:
		cmp ax, word ptr [esi+ecx]
		jbe nodraw

		; get texture
		lea		ebx,	[edi+ebp]
		shr		ebx,	16

		mov  word ptr [esi+ecx], ax

		pxor	mm4,	mm4
		
		movd	mm5,	[ebx*4 + 060000000h]
		org $ - 4
		SMC_Texture dd 0

;		movd		mm5,	[whitecolor]

		punpcklbw	mm5,	mm4
		pmulhuw		mm5,	mm0
		psllw		mm5,	1
		packuswb	mm5,	mm5
		
		; write to screen
		movd [esp+ecx*2], mm5

;		xor [t_edi] , edi
;		xor edi,  [t_edi]
;		xor [t_edi] , edi

;		movd [edi+ecx*2], mm5

;		xor [t_edi] , edi
;		xor edi,  [t_edi]
;		xor [t_edi] , edi
nodraw:
		; step gouraud
		paddw	mm0,	mm1

		; step texture
		add ebp,	[dudx]
		add edi,	[dvdx]

		and ebp,	000ff7fffh   ; T0(LogWidth) : 7fffh
		org $ - 4
		_T0Wd3	dd	000ff7fffh

		and	edi,	0ff007fffh   ; T1(LogWidth) - 1
		org $ - 4
		_T1Hd3	dd	0ff007fffh
		
		; step current Z
		add eax, edx

		; step to next pixel
		add ecx, 2
		jnz Inner

;	mov dword ptr [esp-4], 000ffffffh

	
	movq	mm2,	mm7
;	pslld	mm3,	4
;	paddd	mm2,	mm3
	
	mov eax, [SubspanZ]	
	mov edx, [SubspanCounter]
	sub edx, 16
	mov [SubspanCounter], edx
	jg Outer

	; restore esp, so procedure pointers won't fuck up and shit.
	mov esp,[save_esp]
	ret
SpanLoop			endp

;;;;;;;;;;;;;;;;
_IXAsmFiller		proc	near

;static void IXAsmFiller(IXVertex *Verts, dword numVerts, void *Texture, void *Page, dword logWidth, dword logHeight)

;;; ARGUMENTS ;;;
	Arg_Verts		equ		esp + 4h
	Arg_NumVerts	equ		esp + 8h
	Arg_Texture		equ		esp + 0Ch
	Arg_Page		equ		esp + 10h
	Arg_LogWidth	equ		esp + 14h
	Arg_LogHeight	equ		esp + 18h
	
;IX.Texture = Texture;
	mov	eax,		[Arg_Texture]
	;mov	[_Texture],	eax
	; SMC code
	mov	ds:[SMC_Texture],	eax

;IX.L2Size = logWidth; // Change this to "LogSize" when changing filler arguments
	mov	eax,		[Arg_LogWidth]
	mov	[_L2Size], eax

;IX.Page = Page;
	mov eax,		[Arg_Page]
	mov	[_Page],	eax

;// ZBuffer data starts at the end of framebuffer
;IX.ZBuffer = (word *) ((dword)Page + PageSize);
	add	eax,		[_PageSize]
	mov [_ZBuffer],	eax

;	SMC: Update masks based on texture dimensions
	mov eax, [Arg_LogWidth]
	mov	ebx, [LastLogWidth]
	mov ecx, [Arg_LogHeight]
	mov edx, [LastLogHeight]
	cmp ebx, eax
	jne SMC_update
	cmp edx, ecx
	je after_SMC_update

SMC_update:
	mov	[LastLogWidth],		eax
	mov [LastLogHeight],	ecx
	mov	ebx,	[_T0 + eax * 4 - 4]	; T0[Arg_LogWidth]
	mov	edx,	[_T0 + ecx * 4 - 4]	; T0[Arg_LogHeight]
	lea	esi,	[ebx + 7fffh]
	lea	edi,	[edx + 8000h]
	mov	ds:[_T0Wd0],	ebx
	mov	ds:[_T0Hd0],	edx
	mov	ds:[_Wb0],		al
	mov	ds:[_T0Wd1],	ebx
	mov	ds:[_T0Hd1],	edx
	mov ds:[_T0Hd2],	edi
	mov ds:[_T0Wd3],	esi

	mov	ebx,	[_T1 + eax * 4 - 4]	; T1[Arg_LogWidth]
	mov	edx,	[_T1 + ecx * 4 - 4]	; T1[Arg_LogHeight]

	mov ds:[_Wb1],		al

;	calculates mask width*(height-1) : 7fffh
	mov edi,	1
	shl	edi,	cl
	sub edi,	1

	mov cl,		al
	add cl,		16
	shl edi,	cl
	or	edi,	07fffh

	mov ds:[_T1Wd2],	ebx
	mov ds:[_T1Hd3],	edi


after_SMC_update:

;Left.Vert = Verts + 1;
;Right.Vert = Verts + numVerts - 1;
	
;; esi - Verts
	mov	esi,			[Arg_Verts]
	mov eax,			VertexSize
	mul dword ptr		[Arg_NumVerts]
	lea ebx,			[eax - VertexSize]

;; ebx - Verts + Arg_NumVerts - 1
	add ebx,			esi

;; edi - Verts + 1
	lea edi,			[esi + VertexSize]

;CalcLeftSection (Verts, Verts + 1);

	call	CalcLeftSection

;	while (IX.Left.ScanLines == 0)
;	{
;		CalcLeftSection(Verts + IX.Left.Index, Verts + IX.Left.Index + 1);
;		IX.Left.Index++;
;		if (IX.Left.Index > numVerts - 1)
;		{
;			return;
;		}
;	}

CalcLeftLoop:

	cmp eax, 0
	jnz short LeftCalculated

		cmp edi, ebx
		jae JmpRet

		mov esi,	edi
		add edi,	VertexSize
		call CalcLeftSection


	jmp CalcLeftLoop


LeftCalculated:

	mov [Left_Vert], edi

;CalcRightSection (Verts, Verts + IX.Right.Index]);

	mov esi,	[Arg_Verts]
	mov edi,	ebx
	call CalcRightSection


;	while (IX.Right.ScanLines == 0)
;	{
;		CalcRightSection (Verts + IX.Right.Index, Verts + IX.Right.Index - 1);
;		IX.Right.Index--;
;	}

CalcRightLoop:

	cmp eax, 0
	jnz short RightCalculated

		mov esi, edi
		sub edi, VertexSize
		call CalcRightSection

	jmp CalcRightLoop

RightCalculated:

	mov [Right_Vert], edi


; Calculate constant deltas

;	float dy01 = Verts[1].Y - Verts[0].Y;
;	float dy02 = Verts[2].Y - Verts[0].Y;
;	float invArea = 1.0 / ((Verts[1].X - Verts[0].X) * dy02 - (Verts[2].X - Verts[0].X) * dy01);
;	ddx.dUZdx = invArea * ((Verts[1].UZ - Verts[0].UZ) * dy02 - (Verts[2].UZ - Verts[0].UZ) * dy01);
;	ddx.dVZdx = invArea * ((Verts[1].VZ - Verts[0].VZ) * dy02 - (Verts[2].VZ - Verts[0].VZ) * dy01);
;	ddx.dRZdx = invArea * ((Verts[1].RZ - Verts[0].RZ) * dy02 - (Verts[2].RZ - Verts[0].RZ) * dy01);

	; make sure that ebx points to first vertex
	mov			ebx,		[Arg_Verts]
	; xmm0 = Verts[0].Y
	movss	xmm0,	[ebx + Vertex_Y]
	; xmm1 = Verts[1].Y
	movss	xmm1,	[ebx + VertexSize + Vertex_Y]
	; xmm2 = Verts[2].Y
	movss	xmm2,	[ebx + VertexSize * 2 + Vertex_Y]

	; xmm3 = V[1] - V[0]	xmm4 = V[2] - V[0]	xmm5 = V[0]
	movaps	xmm5,	[ebx + Vertex_RZ]
	; xmm1 = dy01
	subss	xmm1,	xmm0
	movaps	xmm3,	[ebx + VertexSize + Vertex_RZ]
	shufps	xmm1,	xmm1,	00000000b
	; xmm2 = dy02
	subss	xmm2,	xmm0
	movaps	xmm4,	[ebx + VertexSize * 2 + Vertex_RZ]
	shufps	xmm2,	xmm2,	00000000b
	subps	xmm3,	xmm5
	subps	xmm4,	xmm5

	; xmm3 = (V[1] - V[0]) * dy02
	mulps	xmm3,	xmm2
	; xmm4 = (V[2] - V[0]) * dy01
	mulps	xmm4,	xmm1

	; xmm3 = (V[1] - V[0]) * dy02 - (V[2] - V[0]) * dy01 (xmm3 - xmm4)
	subps	xmm3,	xmm4
	
	; store Area in xmm0
	movaps	xmm7,	xmm3
	shufps	xmm7,   xmm7, 11111111b
	; xmm1[0] = invArea
	; could use rcpps
	rcpss	xmm0,	xmm7
;	movss	xmm0,   [f1]
;	divss	xmm0,	xmm7

	; fill xmm0 with invArea
	shufps	xmm0,   xmm0, 00000000b
	; xmm1 = Constant texture deltas
	mulps	xmm0,	xmm3
	; store constant deltas
	movaps	[_dRZdx],	xmm0
	; multiply constant deltas by span size
	mulps	xmm0,	[_pfSPANSIZE]
	; store constant deltas * Span Size
	movaps	[_sdRZdx],	xmm0

;	y = Fist(Verts[0].Y);
;	Page = (dword *)((dword)Page + VESA_BPSL * y);
;	word *ZBuffer = (word *)((dword)Page + PageSize + sizeof(word) * XRes * y);

	; edi = _Page
	movss		xmm0,		[ebx + Vertex_Y]
	cvtss2si	eax,		xmm0
	mov			ecx,		[_VESA_BPSL]
	mov			edi,		[_Page]
	mov			esi,		eax
	; Calculate Screen Pointer
	mul			ecx
	add			edi,		eax
	mov			[_Page],	edi
	mov			eax,		esi
	mov			ecx,		[_XRes]
	; Calculate ZBuffer Pointer
	mul			ecx
	mov			edx,		[_ZBuffer]
	; eax = XRes * y * 2
	add			eax,		eax
	add			edx,		eax
	mov			[_ZBuffer],	edx

;	SectionHeight = (Left.ScanLines < Right.ScanLines) ? Left.ScanLines : Right.ScanLines;
	mov			ebx,	[Left_ScanLines]
	mov			eax,	[Right_ScanLines]
	cmp			eax,	ebx
	cmovb		ebx,	eax

	mov			[_SectionHeight], ebx
	mov			[_HeightCounter], ebx

;	// Iterate over sections
SectionLoop:
	
;	// Iterate over scanlines
ScanLineLoop:

			;lx = Fist(Left.X);
			;dword *LinePagePtr = Page + lx;
			;word *LineZPtr = ZBuffer + lx;
			movss		xmm0,	[Left_X]
			; eax = lx, xmm0 = Left_X
			cvtss2si	eax,	xmm0
			; esi = LineZPtr
			mov			esi,	[_ZBuffer]
			lea			ebx,	[eax + eax]
			add			esi,	ebx
			; edi = LineScrPtr
			mov			edi,	[_Page]
			add			ebx,	ebx
			add			edi,	ebx

;			// Calculate scan-line width	
;			rx = Fist(Right.X);
;			Width = rx - lx;
			movss		xmm1,	[Right_X]
			cvtss2si	ebx,	xmm1
			; ebx = Width, xmm1 = Right_X
			sub			ebx,	eax	
;			if (Width <= 0)
;				goto AfterScanConv;
			jle AfterScanConv

;			Texture delta calculation
;			xmm7 used as temporary.

			; forward loading 
			movaps		xmm2, [_dRZdx]
			movaps		xmm3, [_sdRZdx]
			movaps		xmm6, [Left_RZ]

			; xmm1 = Scan Line Width
			subss		xmm1,	xmm0

			; xmm0 = span prestep
			cvtsi2ss	xmm7, eax
			subss		xmm7, xmm0
			shufps	xmm7,	xmm7,	00000000b

			; xmm7 = prestep * deltas
			; xmm2 = constant d/dx
			; xmm3 = scaled d/dX
			; xmm0 = left before prestep
;			movaps		xmm2, [pf0]
;			movaps		xmm3, [pf0]
			movaps		xmm0, xmm6
			mulps		xmm7, xmm2
			
			
			; xmm0 = initial left
			addps		xmm0, xmm7


;			Gouraud delta calculation
;			xmm6,xmm7 used as temporary

;			sdword rWidth;
;			if (Width>1)
;			{
;				rWidth = Fist(65536.0 / (Right.X - Left.X));
;				sdword delta;
;				delta = (sdword)Right.R - (sdword)Left.R >> 8;
;				dRdx = delta * rWidth >> 16;
;				delta = (sdword)Right.G - (sdword)Left.G >> 8;
;				dGdx = delta * rWidth >> 16;
;				delta = (sdword)Right.B - (sdword)Left.B >> 8;
;				dBdx = delta * rWidth >> 16;
;				delta = (sdword)Right.Z - (sdword)Left.Z >> 8;
;				dZdx = delta * rWidth >> 16;
;			} else {
;				dRdx = dGdx = dBdx = 0;
;			}


			movaps		xmm6,	[Left_R]
			cmp	ebx,1
			je	ZeroDeltas
;			this section places RGBA 8:8 deltas into mm0
			; xmm1 = 1.0 / Scan Line Width
			rcpss		xmm1,	xmm1		
			movaps		xmm7,	[Right_R]
			shufps		xmm1,	xmm1,	0000000b
			; xmm4 = Right_RGBA - Left_RGBA
			subps		xmm7,	xmm6
			; xmm4 = RGBA deltas
			mulps		xmm7,	xmm1			

			; Convert deltas to 8.8 fixed point in mm1
			mulps		xmm7,	[pf256]
			cvtps2pi	mm1,	xmm7
			movhlps		xmm7,	xmm7
			cvtps2pi	mm0,	xmm7
			packssdw	mm1,	mm0
;			psllw		mm1,	1

			jmp			CallSpanLoop
ZeroDeltas:
			pxor		mm0,	mm0			

CallSpanLoop:

;   prepare left RGB (mm1)
			mulps		xmm6,	[pf128]
			cvtps2pi	mm0,	xmm6
			movhlps		xmm6,	xmm6
			cvtps2pi	mm2,	xmm6
			packssdw	mm0,	mm2
			psllw		mm0,	1

;	Summery: Span loop Input
;	mm1, mm0:	RGB, delta
;	ebx:		span width
;	edi:		span ptr
;	esi:		Zbuffer ptr
;	xmm0:		ZUVX initial left
;	xmm2:		ZUVX constant d/dx
;	xmm3:		ZUVX scaled d/dx

;			push edi
			Call		SpanLoop
;			pop edi
;			mov dword ptr [edi], 0FFFFFFFFh

AfterScanConv:
;			forward loading : left Edge values
			movaps		xmm0,		[Left_RZ]
			movaps		xmm4,		[Left_dRZ]
			movaps		xmm1,		[Left_R]
			movaps		xmm5,		[Left_dR]

;			_Page(eax) += VESA_BPSL(ecx);
;			_ZBuffer(ebx) += XRes(edx);
			mov			edx,	[_XRes]
			mov			eax,	[_Page]
			mov			ebx,	[_ZBuffer]
			mov			ecx,	[_VESA_BPSL]
			add			edx,	edx
			add			eax,	ecx
			add			ebx,	edx
			mov			[_Page],	eax
			mov			[_ZBuffer],	ebx

;			forward loading : right Edge values
			movaps		xmm2,		[Right_R]
			movaps		xmm6,		[Right_dR]
			movss		xmm3,		[Right_X]
			movss		xmm7,		[Right_dX]

;			Update Edge Values
			addps		xmm0,		xmm4
			addps		xmm1,		xmm5

			addps		xmm2,		xmm6
			addss		xmm3,		xmm7

			movaps		[Left_RZ],	xmm0
			movaps		[Left_R],	xmm1
			movaps		[Right_R],	xmm2
			movss		[Right_X],	xmm3

			; Decrease section height and jump to next iterate if needed
			mov			ecx,	[_HeightCounter]
			sub			ecx,	1
			mov			[_HeightCounter], ecx
			jnz		ScanLineLoop


; IX.Left.ScanLines -= SectionHeight;

; while (IX.Left.ScanLines == 0)
; {
;	if (IX.Left.Index == IX.Right.Index)
;	{
;		return; // End mapper!
;	}	
;	CalcLeftSection (&Verts[IX.Left.Index], &Verts[IX.Left.Index + 1]);
;	IX.Left.Index ++;
; }

;; edx = SectionHeight
		mov edx,	 [_SectionHeight]		
		mov eax,	 [Left_ScanLines]
		sub eax,	 edx
		mov [Left_ScanLines],	 eax

;; esi = Left_Vert		
		mov edi,	[Left_Vert]
;; ebx = Right_Vert
		mov ebx,	[Right_Vert]

		jnz ReCalcRight
LeftAgain:
		cmp	edi,	ebx
		jz	JmpRet

		mov esi,	 edi
		add edi,	 VertexSize
		call CalcLeftSection

		test eax, eax
		jz LeftAgain

		mov [Left_Vert],	edi

ReCalcRight:

;		Right.ScanLines -= SectionHeight;
;		while (Right.ScanLines == 0)
;		{
;			CalcRightSection (&Verts[Right.Index], &Verts[Right.Index - 1]);
;			Right.Index --;
;		}

		mov eax,	[Right_ScanLines]
		sub eax,	edx;		
		mov [Right_ScanLines],	 eax

		jnz CalcSectionHeight
		mov edi, ebx
RightAgain:
		mov esi,	edi
		sub edi,	VertexSize
		call CalcRightSection

		
		test eax, eax
		jz RightAgain

		mov [Right_Vert], edi

CalcSectionHeight:

;	SectionHeight = (Left.ScanLines < Right.ScanLines) ? Left.ScanLines : Right.ScanLines;
	mov			ebx,	[Left_ScanLines]
	mov			eax,	[Right_ScanLines]
	cmp			eax,	ebx
	cmovb		ebx,	eax

	mov			[_SectionHeight], ebx
	mov			[_HeightCounter], ebx
	
	jmp SectionLoop

JmpRet:
	emms
	ret
_IXAsmFiller				endp
;;;;;;;;;;;;;;;;





_SubInnerLoopAsm	proc near

;;; ARGUMENTS ;;;


	;;; Calculate sub-texelling
	;;; Prepare data for span-loop
	;;; Handle first span
	;;; The span-loop starts
		;;; Prepare data for inner-loop
		;;; The inner-loop starts
		;;; Prepare next span
	;;; Handle last span
	;;; Return to mapper
	ret
		
_SubInnerLoopAsm	endp

ASM_FILE_END	equ	$

_TEXT   ENDS

END