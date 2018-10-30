#ifdef _C_WATCOM
#include <i86.H>
#endif

#include "..\FDS_Defs.H"
#include "..\FDS_Vars.H"
#include "..\FDS_Decs.H"

#ifdef _C_WATCOM
void Fist(long *dst, float f);
#pragma aux Fist = \
		"fistp dword [eax]"\
		parm [eax] [8087] modify [8087];
#else
void Fist(long *dst, float f)
{
	__asm 
	{
		FLD dword ptr [f]
		FISTP dword ptr [dst]
	}
}
#endif

struct TVertex
{
	float X,Y;
	float U,V,Z;
};

struct IRate
{
	long dXdY;
	float dUdY,dVdY,dZdY;
};

float *InvTbl;

void Init_InvTbl(long Entries)
{
	long I;
	float f;
	InvTbl = new float [Entries];
	for(I=1;I<Entries;I++)
		InvTbl[I] = f = 1.0f/(float)I;
}

static char *PageAddr;
static char *Tex,*WPtr,*HPtr;
static long BPSL = 320; //Bytes per Scan Line.

//static long dUdX,dVdX;
static float dUdX,dVdX,dZdX;
static float dUdX16,dVdX16,dZdX16;

static long width,w16;
static long u_,v_,ru_,rv_,du_,dv_,ddu_,ddv_;
static long Height_;

// this should be calced once per polygon. (before clipping).
void CalcGrads(TVertex *VB)
{
	float Det = (VB[0].X - VB[2].X) * (VB[1].Y - VB[2].Y) -
							(VB[1].X - VB[2].X) * (VB[0].Y - VB[2].Y);
	float iDet = 1.0/Det;
	dUdX =  ((VB[0].U - VB[2].U) * (VB[1].Y - VB[2].Y) -
					 (VB[1].U - VB[2].U) * (VB[0].Y - VB[2].Y)) * iDet;
	dVdX =  ((VB[0].V - VB[2].V) * (VB[1].Y - VB[2].Y) -
					 (VB[1].V - VB[2].V) * (VB[0].Y - VB[2].Y)) * iDet;
	dZdX =  ((VB[0].Z - VB[2].Z) * (VB[1].Y - VB[2].Y) -
					 (VB[1].Z - VB[2].Z) * (VB[0].Y - VB[2].Y)) * iDet;
	dUdX16 = dUdX*16.0f;
	dVdX16 = dVdX*16.0f;
	dZdX16 = dZdX*16.0f;
}

inline void TexInner_Affine()
{
	unsigned int u = u_,v = v_,du = du_,dv = dv_;
	while (width--)
	{
		*WPtr++ = Tex[((u>>16)&0xFF)+((v>>8)&0xFF00)];
		u+=du;
		v+=dv;
	}
}

#ifdef _WATCOM_C
void TexInner_Affine_ASM(void * dst, int width, void * src, long u, long v)
#pragma aux TexInner_Affine_ASM=\
		"    push  ebp                                      "\
		"    rol   eax, 16                                  "\
		"    rol   edx, 16                                  "\
		"    mov   bl, al                                   "\
		"    mov   bh, dl                                   "\
		"    mov   esi, [du_]                             	"\
		"    mov   ebp, [dv_]                             	"\
		"    shl   esi, 16                                  "\
		"    mov   dl, byte ptr [du_+2]         	          "\
		"    shl   ebp, 16                                  "\
		"    mov   dh, byte ptr [dv_+2]       	            "\
		"    lea   edi, [edi+ecx]                           "\
		"    xor   ecx, -1                                  "\
		"    inc   ecx                                      "\
		" next:                                             "\
		"    mov   al, [ebx]                                "\
		"    add   edx, ebp                                 "\
		"    adc   bh, dh                                   "\
		"    add   eax, esi                                 "\
		"    adc   bl, dl                                   "\
		"    mov   [edi+ecx], al                            "\
		"    inc   ecx                                      "\
		"    jnz   next                                     "\
		"    pop   ebp                                      "\
	parm [edi] [ecx] [ebx] [eax] [edx] modify exact [eax ebx ecx edx esi edi ebp]
#else
void TexInner_Affine_ASM(void * dst, int width, void * src, long u, long v)
{
	__asm
	{
		mov edi, [dst]
		mov ecx, [width]
		mov ebx, [src]
		mov eax, [u]
		mov edx, [v]
		push  ebp
		rol   eax, 16              
		rol   edx, 16              
		mov   bl, al               
		mov   bh, dl               
		mov   esi, [du_]           
		mov   ebp, [dv_]           
		shl   esi, 16              
		mov   dl, byte ptr [du_+2] 
		shl   ebp, 16              
		mov   dh, byte ptr [dv_+2] 
		lea   edi, [edi+ecx]       
		xor   ecx, -1              
		inc   ecx                  
	next:                      
		mov   al, [ebx]            
		add   edx, ebp             
		adc   bh, dh               
		add   eax, esi             
		adc   bl, dl               
		mov   [edi+ecx], al        
		inc   ecx                  
		jnz   next                 
		pop   ebp                  
	}
}
#endif
// Buffer tables
//        24-31     |    16-23     |     8-15    |    0-7
//------------------------------------------------------------
//EAX      fU            iV               fV           iU
//EBX     Aligned Texture Ptr      |      -V-         -U-
//ECX     -------------------------|        COUNTER (Negative)
//EDX     fdU           idV        |     fdV          idU
//ESI
//EDI     -----------     S  C  R  E  E  N     ---------------
//EBP

static int XL_,XR_;
static float UL_,VL_,ZL_,CUL_,CVL_,CZL_;

void TexOuterLoop_Bilinear(IRate *Left,IRate *Right)
{
	int x1,x2;
	float iVal,iV;
	register long ru,rv;

	while (Height_--)
	{
		x1 = XL_>>14; x2 = XR_>>14;
		width = x2-x1; if (width<=0) goto NOINNER;
		WPtr = HPtr + x1;
		iVal = 1.0f/ZL_;
		Fist(&u_,UL_*iVal);
		Fist(&v_,VL_*iVal);

		CUL_ = UL_ + dUdX16;
		CVL_ = VL_ + dVdX16;
		CZL_ = ZL_ + dZdX16;
		iVal = 1.0f/CZL_;
		Fist(&ru,CUL_*iVal);
		Fist(&rv,CVL_*iVal);

		w16 = width>>4;
		while(w16--)
		{
			CUL_ += dUdX16;
			CVL_ += dVdX16;
			CZL_ += dZdX16;

			du_ = ru-u_>>4;
			dv_ = rv-v_>>4;
			iVal = 1.0f/CZL_;
			TexInner_Affine_ASM(WPtr,16,Tex,u_,v_);

			WPtr+=16;
			u_ = ru;
			v_ = rv;
			Fist(&ru,CUL_*iVal);
			Fist(&rv,CVL_*iVal);
		}
		width&=0xF;
		if (!width) goto NOINNER;
		du_ = ru-u_>>4;
		dv_ = rv-v_>>4;
		TexInner_Affine_ASM(WPtr,width,Tex,u_,v_);

		NOINNER:
		XL_ += Left->dXdY; XR_ += Right->dXdY; // interpolate scanline
		UL_ += Left->dUdY; VL_ += Left->dVdY; ZL_ += Left->dZdY; //satallite data
		HPtr += VESA_BPSL;
	}
}

// Quadratic / Perspective Correct Texture Mapper.
// VB must be Y-sorted.
void PTexTriangle(TVertex *VB)
{
	long Y1,Y2,Y3,dY13,dY12,dY23; //Integers.
	long X1,X2,X3,dX13,dX12,dX23; //18:14 Fixed Point.
	IRate d13,d12,d23;
	float iVal;
//	long dXdY13,dXdY12,dXdY23; //18:14 Fixed Point.
//	float dUdY13,dUdY12,dUdY23;
//	float dVdY13,dVdY12,dVdY23;
//	float dZdY13,dZdY12,dZdY23;
	int Merid;
//	int Height;

//	int XL,XR,x1,x2;
//	float UL,UR,VL,VR,ZL,ZR;

	char *Ptr;

	Fist(&Y1,VB[0].Y);
	Fist(&Y2,VB[1].Y);
	Fist(&Y3,VB[2].Y);
/*	if (Y1==Y3) //Y-subpixelled polygon.
	{
		Ptr = PageAddr + Y1*BPSL;
		if (VB[0].X<VB[1].X) TexEdge(Ptr,VB,VB+1); else TexEdge(Ptr,VB+1,VB);
		if (VB[1].X<VB[2].X) TexEdge(Ptr,VB+1,VB+2); else TexEdge(Ptr,VB+2,VB+1);
		if (VB[0].X<VB[2].X) TexEdge(Ptr,VB,VB+2);	else TexEdge(Ptr,VB+2,VB);
		return;
	}*/
	dY12 = Y2-Y1;
	dY23 = Y3-Y2;
	dY13 = Y3-Y1;

	// polygon is not Y-subpixelled
	Fist(&X1, VB[0].X * 16384.0f);
	Fist(&X2, VB[1].X * 16384.0f);
	Fist(&X3, VB[2].X * 16384.0f);
	dX12 = X2-X1;
	dX13 = X3-X1;
	dX23 = X3-X2;

	if (Y1<Y3)
	{
		iVal = InvTbl[dY13];
		Fist(&d13.dXdY,dX13*iVal);
		d13.dUdY = (VB[2].U-VB[0].U)*iVal;
		d13.dVdY = (VB[2].V-VB[0].V)*iVal;
		d13.dZdY = (VB[2].Z-VB[0].Z)*iVal;
	}

	// check meridian(longest scanline) length.
	// negative values indicate that VB[1] is in the left side of the triangle.
	Merid = dX12 - d13.dXdY * dY12;

	if (Y1<Y2)
	{
		iVal = InvTbl[dY12];
		Fist(&d12.dXdY,dX12*iVal);
		d12.dUdY = (VB[1].U-VB[0].U)*iVal;
		d12.dVdY = (VB[1].V-VB[0].V)*iVal;
		d12.dZdY = (VB[1].Z-VB[0].Z)*iVal;

		XL_ = XR_ = X1;
		UL_ = VB[0].U; VL_ = VB[0].V; ZL_ = VB[0].Z;
	} else {
		if (Merid>0)
		{
			//setup for the next section.
			XL_ = X1; XR_ = X2;
			UL_ = VB[0].U;
			VL_ = VB[0].V;
			ZL_ = VB[0].Z;
		} else {
			XL_ = X2; XR_ = X1;
			UL_ = VB[1].U;
			VL_ = VB[1].V;
			ZL_ = VB[1].Z;
		}
	}

	if (Y2<Y3)
	{
		iVal = InvTbl[dY23];
		Fist(&d23.dXdY,dX23*iVal);
		d23.dUdY = (VB[2].U-VB[1].U)*iVal;
		d23.dVdY = (VB[2].V-VB[1].V)*iVal;
		d23.dZdY = (VB[2].Z-VB[1].Z)*iVal;
	}

	HPtr = PageAddr + Y1*VESA_BPSL;

	Height_ = Y2-Y1;
	if (Merid>0)
	{
		TexOuterLoop_Bilinear(&d13,&d12);
		Height_ = Y3-Y2;
		TexOuterLoop_Bilinear(&d13,&d23);
	}	else {
		TexOuterLoop_Bilinear(&d12,&d13);
		Height_ = Y3-Y2;
		TexOuterLoop_Bilinear(&d23,&d13);
	}
}

void PTex8(Vertex *A,Vertex *B,Vertex *C)
{
	TVertex TV[3];
	TV[0].X = A->PX;
	TV[0].Y = A->PY;
	TV[0].U = A->UZ*65536.0;
	TV[0].V = A->VZ*65536.0;
	TV[0].Z = A->RZ;

	TV[1].X = B->PX;
	TV[1].Y = B->PY;
	TV[1].U = B->UZ*65536.0;
	TV[1].V = B->VZ*65536.0;
	TV[1].Z = B->RZ;

	TV[2].X = C->PX;
	TV[2].Y = C->PY;
	TV[2].U = C->UZ*65536.0;
	TV[2].V = C->VZ*65536.0;
	TV[2].Z = C->RZ;

	PageAddr = VPage;
	Tex = DoFace->Txtr->Txtr->Data;
	CalcGrads(TV);
	PTexTriangle(TV);
}