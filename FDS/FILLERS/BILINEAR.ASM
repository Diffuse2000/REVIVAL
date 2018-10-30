comment &
FPU sections rewrite and full FPU takeover
clock ticks are only estimate for pentium
&


.686
.MMX
.XMM
_DATA   SEGMENT DWORD PUBLIC 'DATA'
align 4
Left_Section            dd      0
Right_Section           dd      0
Left_Section_Height     dd      0
Right_Section_Height    dd      0
Left_X                  dd      0.0 ; float
Delta_Left_X            dd      0.0 ; float
Right_X                 dd      0.0 ; float
Delta_Right_X           dd      0.0 ; float
Left_Z                  dd      0.0 ; float
Delta_Left_Z            dd      0.0 ; float
Left_U                  dd      0.0 ; float
Delta_Left_U            dd      0.0 ; float
Left_V                  dd      0.0 ; float
Delta_Left_V            dd      0.0 ; float
dzdx                    dd      0.0 ; float
dudx                    dd      0.0 ; float
dvdx                    dd      0.0 ; float
RevHeight               dd      0.0 ; float
RevWidth                dd      0.0 ; float
t                       dd      0.0 ; float
Z                       dd      0.0 ; float
U                       dd      0.0 ; float
V                       dd      0.0 ; float
LX                      dd      0
u0                      dd      0
v0                      dd      0
ut1                     dd      0
vt1                     dd      0
du                      dd      0
dv                      dd      0
XWidth                  dd      0
Height                  dd      0
Longest                 dd      0
Left_Array              dd      3 dup(0)
Right_Array             dd      3 dup(0)
Top                     dd      0
Middle                  dd      0
Bottom                  dd      0
ScanLine                dd      0
_DestinationPageD       dd      0
_CurrentTextureD        dd	0
_BTriangleD             dd	0
One                     dd      1.0
Sixteen                 dd      16.0
TwoFiveSix              dd      256.0
SixtyFourK              dd      65536.0
bdu                     db      0
_P_Texture_8_Size	dd	ASM_FILE_END - _P_Texture_8_
;EXTRN  _DestinationPageD    : DWORD
;EXTRN  _CurrentTextureD     : DWORD
;EXTRN  _BTriangleD          : DWORD
EXTRN  _VESA_BPSL           : DWORD
; element positions in struct
Vertex_FX               equ     0
Vertex_FY               equ     4
Vertex_X                equ     8
Vertex_Y                equ     12
Vertex_Z                equ     16
Vertex_U                equ     20
Vertex_V                equ     24
Element_Size            equ     Vertex_V + 4
v1                      equ     ecx
v2                      equ     esi
v3                      equ     edi
d                       equ     dword ptr
align 1
_DATA   ENDS

_TEXT   SEGMENT USE32 DWORD PUBLIC 'CODE'
		ASSUME  cs:_TEXT, ds:_DATA

		PUBLIC  _P_Texture_8_Size

		PUBLIC  _P_Texture_8_
_P_Texture_8_:
push ebp

;vtx in ebx , bitmap in eax , where in ecx

;lea edi,[P_Texture_8_]


mov [_DestinationPageD],ecx  ;       dd      0
mov [_CurrentTextureD],eax   ;       dd			 0
;_BTriangleD             dd	0

;mov [edi + Texture1 - P_Texture_8_],eax
;mov [edi + Texture2 - P_Texture_8_],eax

; update vertex pointers
;mov ebx, [_BTriangleD]
mov ecx,ebx
mov esi,ebx
mov edi,ebx
add esi,Element_Size
add edi,Element_Size * 2

;ecx - vertex 1
;esi - vertex 2
;edi - vertex 3


;cool                       st(0)   st(1)   st(2)   st(3)   st(4)   st(5)
FLD d [v1+Vertex_FY]      ; v1->fy                     ; 1 - 3
FLD d [v2+Vertex_FY]      ; v2->fy  v1->fy             ; 2 - 4
FLD d [v3+Vertex_FY]      ; v3->fy  v2->fy  v1->fy     ; 3 - 5
fxch st(2)                ; v1->fy  v2->fy  v3->fy     ; 4
FISTP d [v1 + Vertex_Y]   ; v2->fy  v3->fy             ; 5 - 10
FISTP d [v2 + Vertex_Y]   ; v3->fy                     ; 11 - 16
FISTP d [v3 + Vertex_Y]   ;                            ; 17 - 22


; sort verticies - obsolete with frustrum clipper
mov eax,[ecx+Vertex_Y]
cmp eax,[esi+Vertex_Y]
jle @@sort1
xchg ecx,esi
@@sort1:
mov eax,[ecx+Vertex_Y]
cmp eax,[edi+Vertex_Y]
jle @@sort2
xchg ecx,edi
@@sort2:
mov eax,[esi+Vertex_Y]
cmp eax,[edi+Vertex_Y]
jle @@sort3
xchg esi,edi
@@sort3:

; if the height is zero - bye bye
mov ebx,[edi+Vertex_Y]
sub ebx,[ecx+Vertex_Y]
jz TextureEnd


mov [Height] , ebx


; ouch ... FULL FPU TAKEOVER  = 21 + 23 + 22 = 66 cycles + ÷100 = ÷166
; if you do all calculations before hand it takes 1 clock cycle less
; but then , there is misused calculations if nothing is being drawed
;cool                       st(0)   st(1)   st(2)   st(3)   st(4)   st(5)
FLD d [v1+Vertex_FX];    1  v1->fx
FLD d [v2+Vertex_FX];    2  v2->fx  v1->fx
FLD d [v3+Vertex_FX];    3  v3->fx  v2->fx  v1->fx
fxch st(2)          ;    3  v1->fx  v2->fx  v3->fx
FISTP d [v1 + Vertex_X];    v2->fx  v3->fx         4 - 9
FISTP d [v2 + Vertex_X];    v3->fx                 10 - 15
FISTP d [v3 + Vertex_X];                           16 - 21

FILD d [v1+Vertex_X]; 1 - 3 v1->x
FILD d [v1+Vertex_Y]; 2 - 5 v1->y   v1->x
FILD d [v2+Vertex_X]; 3 - 6 v2->x   v1->y   v1->x
FILD d [v2+Vertex_Y]; 4 - 7 v2->y   v2->x   v1->y   v1->x
FILD d [v3+Vertex_X]; 5 - 8 v3->x   v2->y   v2->x   v1->y   v1->x
FILD d [v3+Vertex_Y]; 6 - 9 v3->y   v3->x   v2->y   v2->x   v1->y   v1->x
fxch st(5)          ; 10    v1->x   v3->x   v2->y   v2->x   v1->y   v3->y
FSTP d [v1 + Vertex_FX];    v3->x   v2->y   v2->x   v1->y   v3->y  11 - 13
FSTP d [v3 + Vertex_FX];    v2->y   v2->x   v1->y   v3->y          14 - 15
FSTP d [v2 + Vertex_FY];    v2->x   v1->y   v3->y                  16 - 17
FSTP d [v2 + Vertex_FX];    v1->y   v3->y                          18 - 19
FSTP d [v1 + Vertex_FY];    v3->y                                  20 - 21
FSTP d [v3 + Vertex_FY];                                           22 - 23



; claculate longest scanline and stuff (for constant deltas)
;   float t=(float)(v2->y - v1->y) / (float)height;
;   float RevWidth = 16.0 / (t * (v3->x - v1->x) + (v1->x - v2->x));

;   int longest = (int) (RevWidth * 65536.0);
;   if(longest == 0)
;   return;

;cool(÷100 cycles) (saves atleast 39 cycles per scanline (prolly more))
;                           st(0)                    st(1)    st(2)   st(3)
FILD  d [Height]          ; height = x
FLD   d [v2 + Vertex_FY]  ; v2->fy                   x
FSUB  d [v1 + Vertex_FY]  ; [v2->fy - v1->fy] = y    x
FDIVR                     ; [y / x] = t
FLD   d [v3 + Vertex_FX]  ; v3->fx                   t
FSUB  d [v1 + Vertex_FX]  ; [v3->fx - v1->fx] = z    t
FLD   d [Sixteen]         ; 16.0                     z        t
FXCH  ST(2)               ; t                        z        16.0
FLD   d [v1 + Vertex_FX]  ; v1->fx                   t        z       16.0
FSUB  d [v2 + Vertex_FX]  ; [v1->fx - v2->fx] = w    t        z       16.0
FXCH  ST(1)               ; t                        w        z       16.0
FMUL  ST(2),ST(0)         ; t                        w        t*z      16.0
FSTP  d [t]               ; w                        t*z       16.0
FADD                      ; t*z+w                    16.0
FIST  d [Height]          ; t*z+w                    16.0
FDIVP ST(1),ST(0)         ; 16.0/(t*z+w)
FSTP  d [RevWidth]        ; RevWidth

cmp   d [Height] , 0      ; Width * 64k

; now let's go handle some left/right sections procs (aim , then shoot)
; (1 hour later...)
; finished.

jz TextureEnd
jge LongestOverZero


mov [Top],ecx
mov ebp,ecx
mov [Middle],esi
mov [Bottom],edi
mov [Right_Array],edi
mov [Right_Array+4],esi
mov [Right_Array+8],ecx
mov [Right_Section],2

mov [Left_Array],edi
mov [Left_Array+4],ecx
mov [Left_Section],1

call CalcLeftSection
cmp ebx,0
jle TextureEnd

call CalcRightSection
cmp ebx,0
jg FLSNZ  ; First Right Section Is Not Zero
mov eax,[Right_Section]
dec eax
mov [Right_Section],eax
call CalcRightSection
cmp ebx,0
jle TextureEnd
jmp FLSNZ

LongestOverZero:
mov [Top],ecx
mov ebp,ecx
mov [Middle],esi
mov [Bottom],edi
mov [Left_Array],edi
mov [Left_Array+4],esi
mov [Left_Array+8],ecx
mov [Left_Section],2

mov [Right_Array],edi
mov [Right_Array+4],ecx
mov [Right_Section],1

call CalcRightSection
cmp ebx,0
jle TextureEnd

call CalcLeftSection
cmp ebx,0
jg FLSNZ  ; First Left Section Is Not Zero
mov eax,[Left_Section]
dec eax
mov [Left_Section],eax
call CalcLeftSection
cmp ebx,0
jle TextureEnd


FLSNZ:
comment &
calculation of constant deltas for z,u,v :

  float dzdx = (t*(v3->z - v1->z)+(v1->z - v2->z)) * RevWidth;
  float dudx = (t*(v3->u - v1->u)+(v1->u - v2->u)) * RevWidth;
  float dvdx = (t*(v3->v - v1->v)+(v1->v - v2->v)) * RevWidth;
&

mov v1,[Top]
mov v2,[Middle]
mov v3,[Bottom]

;                               st(0)   st(1)   st(2)   st(3)   st(4)   st(5)
FLD   d [t];    1               t
FLD   d [v3 + Vertex_Z];2       v3->z   t
FSUB  d [v1 + Vertex_Z];3 - 5   d3z     t
FLD   d [v3 + Vertex_U];4       v3->u   d3z     t
FSUB  d [v1 + Vertex_U];5 - 7   d3u     d3z     t
FLD   d [v3 + Vertex_V];6       v3->v   d3u     d3z     t
FSUB  d [v1 + Vertex_V];7 - 9   d3v     d3u     d3z     t
FXCH  ST(3)            ;7       t       d3u     d3z     d3v
FMUL  ST(2),ST(0)      ;8 - 10  t       d3u     d3z*t   d3v
FMUL  ST(1),ST(0)      ;9 - 11  t       d3u*t   d3z*t   d3v
FMULP ST(3),ST(0)      ;10 - 12 d3u*t   d3z*t   d3v*t

;                               st(0)   st(1)   st(2)   st(3)   st(4)   st(5)
FLD   d [v1 + Vertex_Z];11      v2->z   d3u*t   d3z*t   d3v*t
FSUB  d [v2 + Vertex_Z];12 - 14 d2z     d3u*t   d3z*t   d3v*t
FLD   d [v1 + Vertex_U];13      v2->u   d2z     d3u*t   d3z*t   d3v*t
FSUB  d [v2 + Vertex_U];14 - 16 d2u     d2z     d3u*t   d3z*t   d3v*t
FLD   d [v1 + Vertex_V];15      v2->v   d2u     d2z     d3u*t   d3z*t   d3v*t
FSUB  d [v2 + Vertex_V];16 - 18 d2v     d2u     d2z     d3u*t   d3z*t   d3v*t
FXCH  ST(2)            ;16      d2z     d2u     d2v     d3u*t   d3z*t   d3v*t

;                               st(0)   st(1)   st(2)   st(3)       st(4)
FADDP ST(4),ST(0)      ;17 - 19 d2u     d2v     d3u*t   d3z*t+d2z   d3v*t

;                               st(0)   st(1)       st(2)       st(3)
FADDP ST(2),ST(0)      ;18 - 20 d2v     d3u*t+d2u   d3z*t+d2z   d3v*t

;                               st(0)       st(1)       st(2)
FADDP ST(3),ST(0)      ;19 - 21 d3u*t+d2u   d3z*t+d2z   d3v*t+d2v

;                               st(0)   st(1)   st(2)   st(3)
FLD   d [RevWidth]     ;20      RW      dtu     dtz     dtv
FMUL  ST(2),ST(0)      ;21 - 23 RW      dtu     dtz*RW  dtv
FMUL  ST(1),ST(0)      ;22 - 24 RW      dtu*RW  dtz*RW  dtv
FMULP ST(3),ST(0)      ;23 - 25 dtu*RW  dtz*RW  dtv*RW
FXCH  ST(1)            ;23      dtz*RW  dtu*RW  dtv*RW
FSTP  d [dzdx]         ;24 - 25 dtu*RW  dtv*RW
FSTP  d [dudx]         ;26 - 27 dtv*RW
FSTP  d [dvdx]         ;28 - 29 dtv*RW
; ouch

mov edi,[ebp+Vertex_Y]
mov eax,[_VESA_BPSL]
mul edi
add eax,[_DestinationPageD]
mov [ScanLine],eax

OuterLoop:
;Outer Loop Starts Here (Oh Yeah)
;                           st(0)   st(1)   st(2)   st(3)
FLD   d [Left_X] ; 1        Lx
FLD   d [Right_X]; 2        Rx      Lx
FXCH             ; 2        Lx      dx
FISTP d [LX]     ; 3 - 8    dx
FISTP d [XWidth] ; 9 - 14

mov ebp,[XWidth]
mov eax,[LX]
sub ebp,eax
cmp ebp,0
mov [XWidth],ebp
jle UpdateValues

mov edi,[ScanLine]
mov ebx,[Left_Z]
mov ecx,[Left_U]
mov edx,[Left_V]
mov [Z],ebx
mov [U],ecx
mov [V],edx

; edi - screen pointer
lea edi,[edi+eax]

;                           st(0)   st(1)   st(2)   st(3)
FLD  d [One] ; 1            1.0
FDIV d [Z]   ; 2 - 40       1.0 / Z

; (Screen) warm up cache (hhhhhhhhhhhnnnnnnnnn)
mov cl,[Edi]

; preperations for first interpolated segment
; fixup u/v                     st(0)       st(1)       st(2)
FMUL d [TwoFiveSix] ; 1 - 3     Rz * 256
FLD  d [U]          ; 2         U           Rz * 256
FLD  d [V]          ; 3         V           U           Rz * 256
FMUL ST(0),ST(2)    ; 4 - 6     Rz*256*V    U           Rz * 256
FXCH ST(2)          ; 4         Rz * 256    U           Rz*256*V
FMUL                ; 5 - 7     Rz*256*U    Rz*256*V
FXCH ST(1)          ; 5         Rz*256*V    Rz*256*U
FISTP dword ptr [v0]; 7 - 12    Rz*256*U
FISTP dword ptr [u0]; 13 - 18

; update z/u/v
;                                   st(0)       st(1)       st(2)
FLD  d [Z]              ; 1         Z
FADD d [dzdx]           ; 2 - 4     Z + dxdz
FLD  d [U]              ; 3         U           Z + DZ
FADD d [dudx]           ; 4 - 6     U + dxdu    Z + dxdz
FLD  d [V]              ; 5         V           U + dxdu    Z + dxdz
FADD d [dvdx]           ; 6 - 8     V + dxdv    U + dxdu    Z + dxdz
FXCH ST(2)              ; 6         Z + dxdz    U + dxdu    V + dxdv
FSTP d [Z]              ; 7 - 8     U + dxdu    V + dxdv
FSTP d [U]              ; 9 - 10    V + dxdv
FSTP d [V]              ; 11 - 12

;                           st(0)   st(1)   st(2)   st(3)
FLD  d [One] ; 1            1.0
FDIV d [Z]   ; 2 - 40       1.0 / Z1

; (Texture) warm up cache (hhhhhhhhhhhnnnnnnnnn)
mov ebx,0
mov ecx,[u0]
mov edx,[v0]
mov bl, ch
mov bh, dh
mov ecx,[_CurrentTextureD]
mov dl,[ecx+ebx]


; fixup u/v                     st(0)       st(1)       st(2)
FMUL d [TwoFiveSix]  ; 1 - 3    Rz * 256
FLD  d [U]           ; 2        U1          Rz1 * 256
FLD  d [V]           ; 3        V1          U1          Rz1* 256
FMUL ST(0),ST(2)     ; 4 - 6    Rz1*256*V1  U1          Rz1* 256
FXCH ST(2)           ; 4        Rz1*256     U1          Rz1*256*V1
FMUL                 ; 5 - 7    Rz1*256*U1  Rz1*256*V1
FXCH ST(1)           ; 5        Rz1*256*V1  Rz1*256*U1
FISTP dword ptr [vt1]; 7 - 12   Rz1*256*U1
FISTP dword ptr [ut1]; 13 - 18


InnerOuterLoop:


; update z/u/v
;                                   st(0)       st(1)       st(2)
FLD  d [Z]              ; 1         Z
FADD d [dzdx]           ; 2 - 4     Z + dxdz
FLD  d [U]              ; 3         U           Z + DZ
FADD d [dudx]           ; 4 - 6     U + dxdu    Z + dxdz
FLD  d [V]              ; 5         V           U + dxdu    Z + dxdz
FADD d [dvdx]           ; 6 - 8     V + dxdv    U + dxdu    Z + dxdz
FXCH ST(2)              ; 6         Z + dxdz    U + dxdu    V + dxdv
FSTP d [Z]              ; 7 - 8     U + dxdu    V + dxdv
FSTP d [U]              ; 9 - 10    V + dxdv
FSTP d [V]              ; 11 - 12


;                           st(0)   st(1)   st(2)   st(3)
FLD  d [One] ; 1            1.0
FDIV d [Z]   ; 2 - 40       1.0 / Z

; inner loop preperations
mov esi,[ut1]
mov edx,[vt1]
sub esi,[u0]
sub edx,[v0]
sar esi,4
mov eax,[XWidth]
sar edx,4
mov ebp,eax
sub eax,16
cmp ebp,16
jle @@NoOver16
mov ebp,16
@@NoOver16:
mov [dv],edx
mov [du],esi
mov [XWidth],eax


comment &

Registers| 31 - 24 | 23 - 16 | 15 - 8  |  7 - 0  |
--------------------------------------------------
EAX      | frac DU |000000000| int DV  | frac DV |
--------------------------------------------------
EBX      | frac U  |000000000| int V   | frac V  |
--------------------------------------------------
ECX      |         |         |         | int U   |
--------------------------------------------------
bdu      |         |         |         | int DU  |
--------------------------------------------------
EDX      | texture                               |
--------------------------------------------------
ESI      | Counter                               |
--------------------------------------------------
EDI      | screen                                |
--------------------------------------------------
EBP      | Holds Pixel                           |
--------------------------------------------------
&
;mov esi,[_CurrentTextureD]

; Create EAX
mov eax,esi
shl eax,24
mov ax,dx
; Create EBX
mov ebx,[u0]
shl ebx,24
mov ecx,[v0]
mov bx,cx
; Create ECX
mov ecx,[u0]
shr ecx,8
; Create EDX
mov edx,esi
mov [bdu],dh
mov edx,[_CurrentTextureD]
;shr edx,8
; Create ESI
mov esi,ebp
;shl esi,1
add edi,esi
xor esi,-1
inc esi
and ecx,0ffffh
; Create EDI
mov ebp,eax


		mov ch,bh
;		db 08ah,081h
;		Texture1 dd 0
		mov al,[edx+ecx]
		add ebx,ebp
		mov ch,bh
		adc cl,[bdu]

@@InnerLoop:

		add ebx,ebp
		mov [edi+esi],al
;		db 08ah,081h
;		Texture2 dd 0
		mov al,[edx+ecx]
		mov ch,bh
		adc cl,dl
		add esi,1
		jnz @@InnerLoop




comment &
mov ebx,0
mov ecx,[u0]
mov edx,[v0]
;sar ecx,8
;sar edx,8
mov   bl, ch
mov   bh, dh


 @@InnerLoop:
    add   edx, [dv]                 ; update v  1 - 2
    add   ecx, [du]                 ; update u  2 - 3
    mov   ax, [esi+ebx*2]           ;           4
    mov   bl, ch                    ;           5
    mov   [edi], ax                 ;           6
    mov   bh, dh                    ;           7
    add   edi,2                     ;           7
    dec   ebp                       ;           8
    jnz   @@InnerLoop               ;           8
&
mov eax,[ut1]
mov ecx,[vt1]
mov [u0],eax
mov [v0],ecx



mov eax,[XWidth]
; fixup u/v                     st(0)       st(1)       st(2)
FMUL d [TwoFiveSix]  ; 1 - 3    Rz * 256
FLD  d [U]           ; 2        U1          Rz1 * 256
FLD  d [V]           ; 3        V1          U1          Rz1* 256
FMUL ST(0),ST(2)     ; 4 - 6    Rz1*256*V1  U1          Rz1* 256
FXCH ST(2)           ; 4        Rz1*256     U1          Rz1*256*V1
FMUL                 ; 5 - 7    Rz1*256*U1  Rz1*256*V1
FXCH ST(1)           ; 5        Rz1*256*V1  Rz1*256*U1
FISTP dword ptr [vt1]; 7 - 12   Rz1*256*U1
FISTP dword ptr [ut1]; 13 - 18

cmp eax,0
jg InnerOuterLoop

;Inner Outer Loop End Here

UpdateValues:
mov eax,[ScanLine]
mov ebx,[Left_Section_Height]
add eax,[_VESA_BPSL]
dec ebx
mov [ScanLine],eax
mov [Left_Section_Height],ebx
jg @@Update_Left_UVXZ
mov ebx,[Left_Section]
dec ebx
jle TextureEnd
mov [Left_Section],ebx
call CalcLeftSection
cmp ebx,0
jle TextureEnd
jmp RightSectionCheck
@@Update_Left_UVXZ:

; update left edge deltas
;                               st(0)   st(1)   st(2)   st(3)
FLD  d [Left_X]      ; 1        LX
FADD d [Delta_Left_X]; 2 - 4    LX1
FLD  d [Left_Z]      ; 3        LZ      LX1
FADD d [Delta_Left_Z]; 4 - 6    LZ1     LX1
FLD  d [Left_U]      ; 5        LU      LZ1     LX1
FADD d [Delta_Left_U]; 6 - 8    LU1     LZ1     LX1
FLD  d [Left_V]      ; 7        LV      LU1     LZ1     LX1
FADD d [Delta_Left_V]; 8 - 10   LV1     LU1     LZ1     LX1
FXCH ST(3)           ; 8        LX1     LU1     LZ1     LV1
FSTP d [Left_X]      ; 9 - 10   LU1     LZ1     LV1
FSTP d [Left_U]      ; 11 - 12  LZ1     LV1
FSTP d [Left_Z]      ; 13 - 14  LV1
FSTP d [Left_V]      ; 15 - 16


RightSectionCheck:
mov ebx,[Right_Section_Height]
dec ebx
mov [Right_Section_Height],ebx
jg @@Update_Right_UVXZ
mov ebx,[Right_Section]
dec ebx
jle TextureEnd
mov [Right_Section],ebx
call CalcRightSection
cmp ebx,0
jle TextureEnd
jmp OuterLoop

@@Update_Right_UVXZ:

; update right edge deltas
;                               st(0)   st(1)   st(2)   st(3)
FLD  d [Right_X]      ; 1       RX
FADD d [Delta_Right_X]; 2 - 4   RX1
FSTP d [Right_X]      ; 5 - 6

jmp OuterLoop

TextureEnd:
pop ebp
ret


CalcLeftSection proc near

mov eax,[Left_Section]
mov v1,[Left_Array+eax*4]
;sub eax,1
mov v2,[Left_Array+eax*4-4]
mov ebx,[v2+Vertex_Y]
sub ebx,[v1+Vertex_Y]
jz @@CLSend
mov [Height],ebx
;cool                    st(0)   st(1)   st(2)   st(3)   st(4)

FILD d [Height]; 1 - 3   hgt
FLD  d [One];    2       1.0     hgt
FLD  d [v2+Vertex_FX];3  v2->fx  1.0     hgt
FXCH ST(2); 3            hgt     1.0     v2->fx
FDIV;    4 - 42          1.0/hgt v2->fx
FXCH ST(1); 4            v2->fx  1.0/hgt
FSUB d [v1+Vertex_FX];5  dx      1.0/hgt
FLD  d [v2+Vertex_Z]; 6  v2->z   dx      1.0/hgt
FSUB d [v1+Vertex_Z]; 7  dz      dx      1.0/hgt
FLD  d [v2+Vertex_U]; 8  v2->u   dz      dx      1.0/hgt
FSUB d [v1+Vertex_U]; 9  du      dz      dx      1.0/hgt

mov  eax,[v1+Vertex_FX] ;10
mov  edx,[v1+Vertex_Z]  ;10
mov  [Left_X],eax       ;11
mov  [Left_Z],edx       ;11
mov  eax,[v1+Vertex_U]  ;12
mov  edx,[v1+Vertex_V]  ;12
mov  [Left_U],eax       ;13
mov  [Left_V],edx       ;13

;cool                       st(0)   st(1)   st(2)   st(3)   st(4)
FLD   d [v2+Vertex_V];14    v2->v   du      dz      dx      1.0/hgt
FSUB  d [v1+Vertex_V];14    dv      du      dz      dx      1.0/hgt
FXCH  ST(4); 14             1.0/hgt du      dz      dx      dv
FMUL  ST(1),st(0);1 - 3     1.0/hgt du/hgt  dz      dx      dv
FMUL  ST(2),st(0);2 - 4     1.0/hgt du/hgt  dz/hgt  dx      dv
FMUL  ST(3),st(0);3 - 5     1.0/hgt du/hgt  dz/hgt  dx/hgt  dv
FMULP ST(4),st(0);4 - 6     du/hgt  dz/hgt  dx/hgt  dv/hgt
FSTP  d [Delta_Left_U];5-6  dz/hgt  dx/hgt  dv/hgt
FSTP  d [Delta_Left_Z];7-8  dx/hgt  dv/hgt
FSTP  d [Delta_Left_X];9-10 dv/hgt
FSTP  d [Delta_Left_V];10-11
@@CLSend:
mov [Left_Section_Height],ebx
ret
CalcLeftSection endp



CalcRightSection proc near

mov eax,[Right_Section]
mov v1,[Right_Array+eax*4]
;sub eax,1
mov v2,[Right_Array+eax*4-4]
mov ebx,[v2+Vertex_Y]
sub ebx,[v1+Vertex_Y]
jz @@CRSend
mov [Height],ebx

;cool                           st(0)   st(1)

FILD d [Height]; 1 - 3          hgt
FLD  d [v2+Vertex_FX];2         v2->fx  hgt
FSUB d [v1+Vertex_FX];3 - 5     dx      hgt
mov  eax,[v1+Vertex_FX];4
mov  [Right_X],eax  ;5
FDIVR;    5 - 43                dx/hgt
FSTP d [Delta_Right_X]

@@CRSend:
mov [Right_Section_Height],ebx
ret
CalcRightSection endp

ASM_FILE_END	equ	$

_TEXT   ENDS

END
