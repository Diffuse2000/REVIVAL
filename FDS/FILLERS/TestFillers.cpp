/***********************************************\

  Filler testing routines
  
\***********************************************/

#include "Base/FDS_DECS.H"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
const	float	AlmostHalf = 0.499999f;

dword fceil (float f)
{
	static dword ceiled;
	__asm 
	{
		FLD   dword ptr [f]
		FADD  dword ptr [AlmostHalf]
		FISTP dword ptr [ceiled]
		mov eax, [ceiled]
	}
}

/* Structures */

const float fSPANSIZE = 32.0f;
const int	SPANSIZE = 32.0f;

struct _Vertex
{
	float X, Y, RZ, UZ, VZ;
	word R, G, B, A;
};

struct LeftSection
{
	float  X,  RZ,  UZ,  VZ;
	float dX, dRZ, dUZ, dVZ;
	
	word    R,  G,  B, A;	// Fixed point!
	word   dR, dG, dB, dA;
	
	float Height; 	// Section height
	int   ScanLines; // Scan-lines left to draw
	int   Index;
};


struct RightSection
{
	float  X;
	float dX;
	
	float Height;
	int   ScanLines;
	int   Index;
};

/* Global variables */

RightSection Right;
LeftSection  Left;

float	dUdx;
float	dVdx;
float	dZdx;
		
float	dUdx32;
float	dVdx32;
float	dZdx32;

word	dRdx;
word	dGdx;
word	dBdx;
word	dAdx;

void CalcRightSection (_Vertex *V1, _Vertex *V2)
{
	
	// Calculate number of scanlines
	Right.ScanLines = fceil (V2->Y) - fceil (V1->Y);
	
	if (Right.ScanLines == 0)
	{
		return;
	}
	
	// Calculate delta
	
	Right.Height = (V2->Y - V1->Y);
	
	Right.X = V1->X;
	
	Right.dX = (V2->X - V1->X) / Right.Height;
	
	// Sub pixeling
	Right.X += Right.dX * ((float)fceil(V1->Y) - V1->Y);
	
}


void CalcLeftSection (_Vertex *V1, _Vertex *V2)
{
	float RevHeight;
	
	// Calculate number of scanlines
	Left.ScanLines = fceil (V2->Y) - fceil (V1->Y);
	
	if (Left.ScanLines == 0)
	{
		return;
	}
	
	Left.Height = (V2->Y - V1->Y);
	RevHeight = 1.0f / Left.Height;
	
	Left.X = V1->X;
	Left.UZ = V1->UZ;
	Left.VZ = V1->VZ;
	Left.RZ = V1->RZ;
	
	
	// Calculate deltas for texture coordinates
	Left.dX = (V2->X - V1->X) * RevHeight;
	Left.dUZ = (V2->UZ - V1->UZ) * RevHeight;
	Left.dVZ = (V2->VZ - V1->VZ) * RevHeight;
	Left.dRZ = (V2->RZ - V1->RZ) * RevHeight;
	
	RevHeight *= 256.0f;
	
	Left.R = fceil (V1->R * 256.0f);
	Left.G = fceil (V1->G * 256.0f);
	Left.B = fceil (V1->B * 256.0f);
	Left.A = fceil (V1->A * 256.0f);
	
	// Calculate deltas for Gouraud values
	Left.dR = fceil ((V2->R - V1->R) * RevHeight);
	Left.dG = fceil ((V2->G - V1->G) * RevHeight);
	Left.dB = fceil ((V2->B - V1->B) * RevHeight);
	Left.dA = fceil ((V2->A - V1->A) * RevHeight);
	
	
	float prestep = ((float)fceil(V1->Y) - V1->Y);
	Left.X += Left.dX * prestep;
	Left.UZ += Left.dUZ * prestep;
	Left.VZ += Left.dVZ * prestep;
	Left.RZ += Left.dRZ * prestep;
	
}

inline void InnerLoop (int Width, dword *ScanLinePtr, dword *Texture, float prestep)
{
	int i = 0;
	float RZ = Left.RZ + prestep * dZdx;
	float UZ = Left.UZ + prestep * dUdx;
	float VZ = Left.VZ + prestep * dVdx;
	int R = Left.R;
	int G = Left.G;
	int B = Left.B;
	
	for (i = 0; i < Width; i++)
	{
		int   _u, _v;
		float _z;
		_z = 256.0f / RZ;
		_u = fceil(UZ * _z);
		_v = fceil(VZ * _z);
		
		*ScanLinePtr = Texture[(_u >> 8) + (_v & 0xFF00)];
		//*ScanLinePtr = (B >> 8) + (G &0xFF00) + ((R & 0xFF00) << 8);		
		UZ += dUdx;
		VZ += dVdx;
		RZ += dZdx;
		
		ScanLinePtr++;
	}
}

inline void SubInnerLoop (int bWidth, dword *ScanLinePtr, dword *Texture, float prestep)
{
	int i = 0;
	int   _u0, _v0, _z0;
	int   _u1, _v1, _z1;
	int   _u2, _v2, _z2;
	int   _u3, _v3, _z3;
	int   Width = bWidth;

	float RZ = Left.RZ + prestep * dZdx;
	float UZ = Left.UZ + prestep * dUdx;
	float VZ = Left.VZ + prestep * dVdx;
	word R = Left.R;
	word G = Left.G;
	word B = Left.B;
	int SpanWidth = Width; // %32

	if (Width > SPANSIZE) SpanWidth = SPANSIZE;

	_z0 = 256.0f / RZ;
	RZ += dZdx32;
	_z1 = 256.0f / RZ;
	RZ += dZdx32;
	_z2 = 256.0f / RZ;
	RZ += dZdx32;
	_z3 = 256.0f / RZ;
	RZ += dZdx32;

		
	_u0 = fceil(UZ * _z0);
	_v0 = fceil(VZ * _z0);
	UZ += dUdx32;
	VZ += dVdx32;


	
	for(;;)
	{
		int   _du, _dv;

		_u1 = fceil(UZ * _z1);
		_v1 = fceil(VZ * _z1);

		_du = _u1 - _u0 >> 5;
		_dv = _v1 - _v0 >> 5;
		
		while (SpanWidth--)
		{
			dword r,g,b, tex;
			tex = Texture[(_u0>> 8) + (_v0 & 0xFF00)];
			r = (tex & 0xff0000) >> 8;
			g = (tex & 0x00ff00);
			b = (tex & 0x0000ff) << 8;

			r *= R;
			g *= G;
			b *= B;


			r = (r & 0xff000000) >> 8;               
			g = (g >> 16) & 0x00ff00;
			b = (b >> 24) & 0x0000ff;      
	//		g = b = 0;

			*ScanLinePtr = r + g + b; 
	//		*ScanLinePtr = (B >> 8) + (G &0xFF00) + ((R & 0xFF00) << 8);
			ScanLinePtr++;
			_u0 += _du;
			_v0 += _dv;
			R += dRdx;
			G += dGdx;
			B += dBdx;
		}

		Width -= 32;
		if (Width <= 0) return;

		SpanWidth = Width;

		if (Width > 32)
		{
			SpanWidth = 32;
		}
		
		_u0 = _u1;
		_v0 = _v1;

		_z1 = _z2;
		_z2 = _z3;
		_z3 = 256.0f / RZ;
		RZ += dZdx32;
		UZ += dUdx32;
		VZ += dVdx32;
	}
		
}



// Counter clock-wise vertices!
void BaseMapper (_Vertex **Verts,  int VertNum,
				 char *DestSurf, dword *TexMap, int TexLogSize,
				 unsigned int BPSL)
{
	int 	Width;		// Scan-line width
	float 	RevLongest;
	char	*SurfPtr;
	int		EqualY;		// Determines if (at least) the first two vertices are
						// eqaul in their Y component
	
	SurfPtr = (char *)(DestSurf + fceil(Verts[0]->Y) * BPSL);
	
	// Calculate starting sections
	Left.Index = 1;
	Right.Index  = VertNum - 1;
	
	EqualY = 0;
	
	CalcLeftSection (Verts[0], Verts[Left.Index]);
	while (Left.ScanLines == 0)
	{
		EqualY = 1;
		CalcLeftSection(Verts[Left.Index], Verts[Left.Index + 1]);
		Left.Index++;
		if (Left.Index >= VertNum)
		{
			return;
		}
	}
	
	CalcRightSection (Verts[0], Verts[Right.Index]);	
	while (Right.ScanLines == 0)
	{
		EqualY = 1;
		CalcRightSection (Verts[Right.Index], Verts[Right.Index - 1]);
		Right.Index--;
	}
	
	if (EqualY == 0)	// First two Y are different
	{
		// Calculate constant deltas for line drawings
		RevLongest = 1.0f / (Verts[Right.Index]->X -
			(Left.X + Right.Height * Left.dX));
		
		dUdx = (Verts[Right.Index]->UZ - (Left.UZ + Right.Height * Left.dUZ)) *
			RevLongest;
		dVdx = (Verts[Right.Index]->VZ - (Left.VZ + Right.Height * Left.dVZ)) *
			RevLongest;
		dZdx = (Verts[Right.Index]->RZ - (Left.RZ + Right.Height * Left.dRZ)) *
			RevLongest;
		dRdx = ((Verts[Right.Index]->R << 8) - (Left.R + Right.Height * Left.dR)) *
			RevLongest;
		dGdx = ((Verts[Right.Index]->G << 8) - (Left.G + Right.Height * Left.dG)) *
			RevLongest;
		dBdx = ((Verts[Right.Index]->B << 8) - (Left.B + Right.Height * Left.dB)) *
			RevLongest;
		dAdx = ((Verts[Right.Index]->A << 8) - (Left.A + Right.Height * Left.dA)) *
			RevLongest;
		
	} else	// First two vertices share the same Y
	{
		// TODO: Maybe calculate constant deltas outside of the mapper
		int _t = VertNum - 1;
		RevLongest = 1.0f / (Verts[0]->X - Verts[_t]->X);
		dUdx = (Verts[0]->UZ - Verts[_t]->UZ) * RevLongest;
		dVdx = (Verts[0]->VZ - Verts[_t]->VZ) * RevLongest;
		dZdx = (Verts[0]->RZ - Verts[_t]->RZ) * RevLongest;
		dRdx = ((Verts[0]->R) - (Verts[_t]->R)) * RevLongest;
		dGdx = ((Verts[0]->G) - (Verts[_t]->G)) * RevLongest;
		dBdx = ((Verts[0]->B) - (Verts[_t]->B)) * RevLongest;
		dAdx = ((Verts[0]->A) - (Verts[_t]->A)) * RevLongest;
	}

	dUdx32 = dUdx * fSPANSIZE;
	dVdx32 = dVdx * fSPANSIZE;
	dZdx32 = dZdx * fSPANSIZE;
	
	// Iterate over sections
	while (1)
	{
		// *** Draw scan line *** //
		char	*ScanLinePtr = SurfPtr + (fceil(Left.X) << 2);
		int		lx;
		// Calculate scan-line width
		
		lx = fceil(Left.X);
		Width = fceil (Right.X) - lx;
		
		if (Width == 0)
		{
			goto AfterLineDrawing;
		}
		
		// Iterate over scan-line
		SubInnerLoop(Width, (dword *)ScanLinePtr, TexMap, (float)lx - Left.X);
		
		// *** End of scan line drawing //
AfterLineDrawing:
		
		SurfPtr += BPSL;
		
		// Reduce sections heights and check if they are done
		Left.ScanLines --;
		if (Left.ScanLines == 0)
		{
			if (Left.Index == Right.Index)
			{
				break; // End mapper!
			}
			
			CalcLeftSection (Verts[Left.Index], Verts[Left.Index + 1]);
			Left.Index ++;
		}
		
		Right.ScanLines --;
		if (Right.ScanLines == 0)
		{
			if (Right.Index == Left.Index)
			{
				break; // End mapper! (shouldn't happen!!!)
			}
			
			CalcRightSection (Verts[Right.Index], Verts[Right.Index - 1]);
			Right.Index --;
		}
		
		// Advance section components
		Left.X  += Left.dX;
		Left.RZ += Left.dRZ;
		Left.UZ += Left.dUZ;
		Left.VZ += Left.dVZ;
		Left.R  += Left.dR;
		Left.G  += Left.dG;
		Left.B  += Left.dB;
		Left.A  += Left.dA;
		
		Right.X += Right.dX;
		
	}  // End of outer scan-line loop
}
void PrepareVerts (_Vertex **Verts, dword NumVerts, _Vertex **Out)
{
	dword	FirstVert;	// First vertex in NGon
	dword	i;
	
	// Find vertex with lowest Y and set it to be the first vertex
	FirstVert = 0;
	for (i = 1; i < NumVerts; i++) 
	{
		if (Verts[i]->Y < Verts[FirstVert]->Y) 
		{
			FirstVert = i;
		}
	}
	
	// Copy vertices into new array
	memcpy (Out,
		Verts + FirstVert/* sizeof(Vertex *)*/,
		sizeof(_Vertex *) * (NumVerts - FirstVert));
	
	if (FirstVert == 0)
	{
		return;
	}
	
	memcpy (Out + (NumVerts - FirstVert) /* * sizeof(Vertex *) */,
		Verts,
		sizeof(_Vertex *) * (FirstVert));
}

extern "C" void __cdecl P_Texture_32_(void);
static void P_Texture_32V(Bi_Triangle * BTM, char * Bitmap,unsigned int u)
{
	__asm
	{
		pushad
			mov ebx, [BTM]
			mov eax, [Bitmap]
			mov ecx, [u]
			call	P_Texture_32_
			popad
	}
}

extern "C" void __cdecl NewMapper_(void);

void RunFillerTests()
{
	
	// Test texture mapper
	// TODO: Code a texture mapper which supports variable texture size (powers of 2)
	//		 and supports Z buffering
	
	
	// Create test vertices
	dword *Texture;
	Image	tx;
	int i, j;
	
	Load_Image_JPEG(&tx,"Textures//Logo.JPG");
	Scale_Image(&tx,256,256);
	Texture = tx.Data;
	
	/*	for (i = 0; i < 256; i++) 
	{
	for (j = 0; j < 256; j++)
	{
	Texture [i*256 + j] = i ^ j;//(i ^ j) + ((i & j) << 8) + (((i * j) & 0xFF) << 16);
	}
}*/
	
//	NewMapper_();

	dword NumVerts = 3;
	_Vertex **Verts = new _Vertex * [NumVerts * 2];
	_Vertex **NewVerts = new _Vertex * [NumVerts * 2];
	
	for (int Counter = 0;Counter < 3900; Counter += 7)
	{
		
		int		_t = 0;
		float 	_z = 1.0f;
		
		Verts[_t] = new _Vertex;
		Verts[_t]->X = 0.0f;
		Verts[_t]->Y = 0.0f;//Counter / 10.0f;//0.0f;
		Verts[_t]->UZ = 0.0f / _z;
		Verts[_t]->VZ = 0.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 254;
		Verts[_t]->G = 254;
		Verts[_t]->B = 254;
		
		_t ++;
		_z = 1.0f;
		Verts[_t] = new _Vertex;
		Verts[_t]->X = Counter / 10.0f;
		Verts[_t]->Y = 599.0f;
		Verts[_t]->UZ = 0.0f / _z;
		Verts[_t]->VZ = 255.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 254;
		Verts[_t]->G = 254;
		Verts[_t]->B = 254;
		
		_t ++;
		_z = 1.0f;;
		Verts[_t] = new _Vertex;
		Verts[_t]->X = 399.0f;
		Verts[_t]->Y = 599.0f;
		Verts[_t]->UZ = 255.0f / _z;
		Verts[_t]->VZ = 255.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 254;
		Verts[_t]->G = 254;
		Verts[_t]->B = 254;
		
		//////////////////////////////////////////////////
		_t ++;
		_z = 1.0f;
		Verts[_t] = new _Vertex;
		Verts[_t]->X = 0.0f;
		Verts[_t]->Y = 0.0f;
		Verts[_t]->UZ = 0.0f / _z;
		Verts[_t]->VZ = 0.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 254;
		Verts[_t]->G = 254;
		Verts[_t]->B = 254;
		
		_t ++;
		_z = 1.0f;
		Verts[_t] = new _Vertex;
		Verts[_t]->X = 399.0f;
		Verts[_t]->Y = 599.0f;
		Verts[_t]->UZ = 255.0f / _z;
		Verts[_t]->VZ = 255.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 254;
		Verts[_t]->G = 254;
		Verts[_t]->B = 254;
		
		_t ++;
		_z = 1.0f;
		Verts[_t] = new _Vertex;
		Verts[_t]->X = 399.0f;
		Verts[_t]->Y = 0.0f;
		Verts[_t]->UZ = 255.0f / _z;
		Verts[_t]->VZ = 0.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 254;
		Verts[_t]->G = 254;
		Verts[_t]->B = 254;
		
		
		/*_t ++;
		_z = 1.0f;
		Verts[_t] = new _Vertex;
		Verts[_t]->X = 319.0f;
		Verts[_t]->Y = 0.0f;
		Verts[_t]->UZ = 255.0f / _z;
		Verts[_t]->VZ = 0.0f / _z;
		Verts[_t]->RZ = _z;
		Verts[_t]->R = 200.0f;
		Verts[_t]->G = 220.0f;
		Verts[_t]->B = 240.0f;*/
		
		// Copy vertices into a sorted vertex array
		PrepareVerts (Verts, NumVerts, NewVerts);
		PrepareVerts (Verts + 3, NumVerts, NewVerts + 3);
		// Copy first vertex to last index, for comfort reasons
		//	memcpy (NewVerts[NumVerts], NewVerts[0], sizeof (_Vertex *));

		Bi_Triangle BT[6];
		for (i = 0; i < 6; i ++)
		{
			BT[i].X = NewVerts[i]->X + 400.0f;
			BT[i].Y = NewVerts[i]->Y;
			BT[i].Z = NewVerts[i]->RZ;
			BT[i].U = NewVerts[i]->UZ;
			BT[i].V = NewVerts[i]->VZ;
		}
		
		//#ifdef NEWMAPPER	
		// Draw the NGon
//		BaseMapper (NewVerts, NumVerts, MainSurf->Data, Texture, 8, VESA_BPSL);
		BaseMapper (NewVerts + 3, NumVerts, MainSurf->Data, Texture, 8, VESA_BPSL);	
		
		//#else // !defined NEWMAPPER
		// Convert vertices
		
		P_Texture_32V(BT, (char *)Texture ,(unsigned long)MainSurf->Data);
		P_Texture_32V(BT + 3, (char *)Texture ,(unsigned long)MainSurf->Data);
		//#endif // NEWMAPPER
		
		
		
		//	while (!Keyboard[ScESC]) 
		//	{
		Flip(MainSurf);
		Sleep(20);
		//	}
	}
	for (i = 0; i < NumVerts; i++) delete Verts[i];
	
	delete [] Verts;
}
