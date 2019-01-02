#include "DDRAW.h"
#include <float.h>
#include <stdio.h>
#include <Base/FDS_VARS.H>
#include <Base/FDS_DECS.H>

//#define fprintf //

static DWORD FullScreen_;

static HINSTANCE G_hInst;
static HWND G_hWnd;
static LPDIRECTDRAW4 DDObj = NULL;
static LPDIRECTDRAWPALETTE DDPal = NULL;
static LPDIRECTDRAWSURFACE4 PrimSurf = NULL;
static LPDIRECTDRAWSURFACE4 ScndSurf = NULL;
static DWORD SurfAtVMem = 0;
static DWORD SurfLocked = 0;
static LPPALETTEENTRY VPal; 
static DWORD XRes_,YRes_,BPP_;
static FILE *Logfile = NULL;
static VESA_Surface Secondary;
static VESA_Surface DD_MainSurf;
static RECT ScRect;
static DWORD RA_Flag = 0;

void mem3(void *dst, void *src, int nbytes)
{
	_asm { 
        mov esi, src 
        mov edi, dst 
        mov ecx, nbytes 
        shr ecx, 7 // 128 bytes per iteration 

loop1: 
        prefetchnta 128[ESI] // Prefetch next loop, non-temporal 
        prefetchnta 256[ESI] 

        movntdqa xmm1,  0[ESI] // Read in source data 
		movntdqa xmm2,  16[ESI]
        movntdqa xmm3, 32[ESI] 
        movntdqa xmm4, 48[ESI] 
        movntdqa xmm5, 64[ESI] 
        movntdqa xmm6, 80[ESI] 
        movntdqa xmm7, 96[ESI] 
        movntdqa xmm0, 112[ESI] 
		   
        movntdq  0[EDI], xmm1 // Non-temporal stores 
        movntdq 16[EDI], xmm2 
        movntdq 32[EDI], xmm3 
        movntdq 48[EDI], xmm4 
        movntdq 64[EDI], xmm5 
        movntdq 80[EDI], xmm6 
        movntdq 96[EDI], xmm7 
        movntdq 112[EDI], xmm0 

        add esi, 128
        add edi, 128
        dec ecx 
        jnz loop1 

;		emms 
	} 
}


// n - number of qwords to copy
static void FastMemcopy(void *dst, void *src, dword n)
{
	__asm
	{
		mov edi, dst
		mov esi, src
		mov ecx, n

		lea edi, [edi+ecx*8]
		lea esi, [esi+ecx*8]
		neg ecx
Inner:
		movq MM0, [esi+ecx*8]
		movq [edi+ecx*8], MM0

/*		movq MM0, [esi+ecx*8]
		movq MM1, [esi+ecx*8 + 8]
		movq MM2, [esi+ecx*8 + 16]
		movq MM3, [esi+ecx*8 + 24]
		movq MM4, [esi+ecx*8 + 32]
		movq MM5, [esi+ecx*8 + 40]
		movq MM6, [esi+ecx*8 + 48]
		movq MM7, [esi+ecx*8 + 56]
		;movntq [edi+ecx*8], MM0
		_emit 0x0F
		_emit 0xE7
		_emit 0x04
		_emit 0xCF
		;movntq [edi+ecx*8 + 8], MM1
		_emit 0x0F
		_emit 0xE7
		_emit 0x4C
		_emit 0xCF
		_emit 0x8
		;movntq [edi+ecx*8 + 16], MM2
		_emit 0x0F
		_emit 0xE7
		_emit 0x54
		_emit 0xCF
		_emit 0x10
		;movntq [edi+ecx*8 + 24], MM3
		_emit 0x0F
		_emit 0xE7
		_emit 0x5C
		_emit 0xCF
		_emit 0x18
		;movntq [edi+ecx*8 + 32], MM4
		_emit 0x0F
		_emit 0xE7
		_emit 0x64
		_emit 0xCF
		_emit 0x20
		;movntq [edi+ecx*8 + 40], MM5
		_emit 0x0F
		_emit 0xE7
		_emit 0x6C
		_emit 0xCF
		_emit 0x28
		;movntq [edi+ecx*8 + 48], MM6
		_emit 0x0F
		_emit 0xE7
		_emit 0x74
		_emit 0xCF
		_emit 0x30
		;movntq [edi+ecx*8 + 56], MM7
		_emit 0x0F
		_emit 0xE7
		_emit 0x7C
		_emit 0xCF
		_emit 0x38*/

		inc ecx
		jnz Inner
		emms
	}
}

HRESULT Restore_All()
{
	RA_Flag = 0;
	HRESULT ErrBits = 0;
	if ((ErrBits |= PrimSurf->IsLost())) 
	{
		ErrBits |= PrimSurf->Restore();
		if (!ErrBits) {
//			fprintf(Logfile,"Successfully restored Primary\n");
		}
	}
	/*for (S_List *S = SurfList ; S ; S=S->Next)
		if (ErrBits |= ((LPDIRECTDRAWSURFACE4)(S->VS->Handle))->IsLost())
		{
			if (!ErrBits) fprintf(Logfile,"Successfully restored Surface %d\n",S);
			ErrBits |= ((LPDIRECTDRAWSURFACE4)(S->VS->Handle))->Restore();
		}*/
	return ErrBits;
}


static dword V_Lock(VESA_Surface *VS)
{
//	fprintf(Logfile,"attempting to LOCK a surface.\n");

	HRESULT hr;
	DDSURFACEDESC2      ddsd;
	if ((hr = DDObj->TestCooperativeLevel() != DD_OK))
	{		
//		fprintf(Logfile,"Mismatching Cooperative Level. (Lock)\n");
		return 1;
	}

	LPDIRECTDRAWSURFACE4 Surf = ScndSurf;//(LPDIRECTDRAWSURFACE4)VS->Handle;
	if (!Surf) {
//		fprintf(Logfile,"Invalid surface handle (Lock)\n");
		return 1;
	}
	memset(&ddsd,0,sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
//Again:
	if ((hr = Surf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL)) == DD_OK)
	{
		Secondary.Data = (byte *)ddsd.lpSurface;
		Secondary.Flags |= VSurf_Locked;
		//fprintf(Logfile,"Surface (at %xH) Locked.\n",(DWORD)VS->FB);
		return 0;
	}

	switch (hr)
	{
		case DDERR_INVALIDOBJECT   : break;
		case DDERR_INVALIDPARAMS   : break;
		case DDERR_SURFACELOST     : 
									 //if (Restore_All() == DD_OK) 	
									 if (Surf->Restore() == DD_OK) 
									 {
//										 fprintf(Logfile,"Surface at %xH was lost and restored.\n",(DWORD)VS->FB);
//										 goto Again; 
										 return V_Lock(VS);
									 } else {
//										fprintf(Logfile,"Lock failed: Surface at %xH was lost and cannot be restored.\n",(DWORD)VS->FB);
										break;
									 }
		case DDERR_WASSTILLDRAWING : break;
		case DDERR_SURFACEBUSY     : break;
	}

//	fprintf(Logfile, "Wahhhhhh!\n");
	return 1;
}

static dword V_Unlock(VESA_Surface *VS)
{
	HRESULT hRet;
	if ((hRet = DDObj->TestCooperativeLevel() != DD_OK)) 
	{
		RA_Flag = 1;
//		fprintf(Logfile,"Mismatching Cooperative Level. (Unlock)\n");
		return 1;
	}
	LPDIRECTDRAWSURFACE4 Surf = ScndSurf;//(LPDIRECTDRAWSURFACE4)VS->Handle;
	if (!Surf) return 1;
	if (!(VS->Flags&VSurf_Locked)) 
	{
//		fprintf(Logfile,"<Warning>: unlock used on unlocked surface at %xH.\n",(DWORD)VS->Data);
		return 1;
	}
	VS->Flags ^= VSurf_Locked;
	if (Surf->Unlock(NULL) == DD_OK)
	{
		//fprintf(Logfile,"Surface (at %xH) Unlocked.\n",(DWORD)VS->FB);
		return 0;
	} else {
//		fprintf(Logfile,"Failed to unlock Surface at %xH.\n",(DWORD)VS->Data);
		return 1;
	}
//	fprintf(Logfile, "Wahhhhhh!\n");
}


static void V_Flip(VESA_Surface *VS)
{
//	fprintf(Logfile, "Wahhhhhh!\n");
//	fflush(Logfile);

	HRESULT hRet;
	DDSURFACEDESC2 ddsd;

	if (V_Lock(&Secondary)) 
	{
		return;
	}

	// copy system page to secondary framebuffer
	dword y;
	char *dst = (char *)Secondary.Data;
	char *src = (char *)VS->Data;

//	fprintf(Logfile, "dst = %xh, src = %xh\n", (dword)dst, (dword)src);

	for(y=0; y<VS->Y; y++)
	{
		//FastMemcopy(dst, src, VS->X * VS->CPP >> 3);
		memcpy(dst, src, VS->X * VS->CPP); 
		dst += Secondary.BPSL;
		src += VS->BPSL;
	}
	__asm emms

	if (V_Unlock(&Secondary))
	{
		OutputDebugString("blah\n");
		return;
	}


	if (FullScreen_)
	{
		hRet = PrimSurf->Flip(NULL, DDFLIP_WAIT);
		if (hRet != DD_OK) OutputDebugString("Erorr in Flipper\n");
//		fprintf(Logfile,"Performing a Page flipping (Source: %xH).\n",(DWORD)VS->Data);
	}															  
	else
	{
		RECT TRect;
		TRect.top = 0;
		TRect.left = 0;
		TRect.right = VS->X;
		TRect.bottom = VS->Y;

		GetClientRect( G_hWnd, &ScRect );
		ClientToScreen( G_hWnd, (POINT*)&ScRect.left );
		ClientToScreen( G_hWnd, (POINT*)&ScRect.right );
		
		hRet = PrimSurf->Blt(&ScRect,ScndSurf,&TRect,DDBLT_WAIT,NULL);

//		fprintf(Logfile,"Performing a Page BlT (Source: %xH).",(DWORD)VS->Data);
	}
}

dword DDRAW_RemoveDisplay()
{
	HRESULT hr;
    if (DDObj != NULL)
    {
		if (!FullScreen_)
		{
			if (ScndSurf != NULL)
			{
				if (Secondary.Flags & VSurf_Locked)
					V_Unlock(&Secondary);
				hr = ScndSurf->Release();
				ScndSurf = NULL;
			}
		} 
	    if (PrimSurf != NULL)
		{
	        hr = PrimSurf->Release(); //should also release attached ScndSurf for Fullscreen mode.
			PrimSurf = NULL;
		}
		
        hr = DDObj->Release();
        DDObj = NULL;
    }
	fclose(Logfile);

	_aligned_free(DD_MainSurf.Data);

	return 0;
}

//#include "dxerr9.h"
static dword V_Create(VESA_Surface *VS)
{
	VS->CPP = (VS->BPP+1)>>3;
	VS->BPSL = VS->CPP * VS->X;
	VS->PageSize = VS->BPSL * VS->Y;

	dword ZBufferSize = sizeof(word) * VS->X * VS->Y;
	if (!(VS->Data = (byte *)malloc(VS->PageSize + ZBufferSize))) return 1;
	memset(VS->Data,0,VS->PageSize + ZBufferSize);

//	VS->Flags = VSurf_Exists;

	VS->Handle = 0;

	return 0;
}


dword DDRAW_InitDisplay()
{
	LPDIRECTDRAW        DDCom;
	DDSURFACEDESC2      ddsd;
	HRESULT             DDRes;
//		char temp[100];

	XRes_ = g_DIP.x;
	YRes_ = g_DIP.y;
	BPP_ = g_DIP.bpp;
	G_hWnd = g_DIP.hWnd;
	G_hInst = g_DIP.hInst;

	Logfile = fopen("VidUnit.Log","wt+");
//	fprintf(Logfile,"Unit Initialization Attempt with Screen Resolution (%d,%d) at %d BPP\n",XRes_, YRes_, BPP_);

	DDRes = DirectDrawCreate( NULL,&DDCom, NULL );
	if( DDRes != DD_OK ) return 1;

    // Fetch DirectDraw4 interface
    DDRes = DDCom->QueryInterface(IID_IDirectDraw4, (LPVOID *) &DDObj);
    if (DDRes != DD_OK) return 1;

	// Creates a Primery Surface in the specified parameters
	if (g_DIP.flags & DISP_FULLSCREEN)
	{
		ShowCursor(FALSE);
		FullScreen_ = 1;

		// Drawing rectangles: the entire screen
		SetRect( &ScRect, 0, 0, XRes_, YRes_ );

//		SetWindowLong( G_hWnd, GWL_STYLE, WS_VISIBLE);
//		SetForegroundWindow(G_hWnd);

		// Get exclusive mode
		DDRes = DDObj->SetCooperativeLevel( G_hWnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );		

		//sprintf(temp,"COOP = %x\n", DDRes);
		//OutputDebugString(temp);

		if( DDRes != DD_OK ) return 1;
	
		DDRes = DDObj->SetDisplayMode(XRes_, YRes_, BPP_, 0,0);

		//sprintf(temp,"SETDISP = %x\n", DDRes);
		//OutputDebugString(temp);

		if( DDRes != DD_OK ) return 1;

		// Create the Primary surface with and attached Secondary surface
		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof( DDSURFACEDESC2 );
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
		ddsd.dwBackBufferCount = 1;

		//while (ShowCursor(FALSE)>=0);
	} else {
		FullScreen_ = 0;
		//GDI code prepares window
		RECT rectum;
		long MXR,MYR;

		MXR=GetSystemMetrics(SM_CXSCREEN);
		MYR=GetSystemMetrics(SM_CYSCREEN);

		rectum.left = (MXR-XRes_)>>1;
		rectum.right = (MXR+XRes_)>>1;
		rectum.top = (MYR-YRes_)>>1;
		rectum.bottom = (MYR+YRes_)>>1;
		if (!AdjustWindowRectEx(  &rectum, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW))
			return 1;
		SetWindowLong(G_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(G_hWnd,HWND_NOTOPMOST,rectum.left, rectum.top, rectum.right-rectum.left, rectum.bottom-rectum.top,0);
	
		DDRes = DDObj->SetCooperativeLevel( G_hWnd, DDSCL_NORMAL );
		if( DDRes != DD_OK ) return 1;
		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof( DDSURFACEDESC2 );
		ddsd.dwFlags = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		GetClientRect( G_hWnd, &ScRect );
		ClientToScreen( G_hWnd, (POINT*)&ScRect.left );
		ClientToScreen( G_hWnd, (POINT*)&ScRect.right );
	}

	DDRes = DDObj->CreateSurface(&ddsd, &PrimSurf, NULL );

	//sprintf(temp,"CREATE PRIMARY SURFACE %x\n", DDRes);
	//OutputDebugString(temp);

//	const char *str = DXGetErrorString9(DDRes);
	// Primery surface creation must succeed
	if (DDRes != DD_OK) 
	{
//		fprintf(Logfile, "Failed to create primary surface.\n");
		return 1;
	}

	if (FullScreen_)
	{
		// Fetch the secondary Video surface
		DDSCAPS2 ddscaps;
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		if( FAILED( DDRes = PrimSurf->GetAttachedSurface( &ddscaps, &ScndSurf ) ) )	
		{
//			fprintf(Logfile, "Failed to fetch secondary surface.\n");
			return 1;
		}
	} else {
		// Create the secondary Video surface
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth        = XRes_;
		ddsd.dwHeight       = YRes_;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		if( FAILED( DDRes = DDObj->CreateSurface( &ddsd, &ScndSurf, NULL ) ) ) 
		{
//			fprintf(Logfile, "Failed to create secondary surface.\n");
			return 1;
		}

		// Create a clipper object (Unknown purpose)
		LPDIRECTDRAWCLIPPER pcClipper;
		if( FAILED( DDRes = DDObj->CreateClipper( 0, &pcClipper, NULL ) ) )
		{
//			fprintf(Logfile, "Failed to create clipper.\n");
			return 1;
		}

		// Associate the clipper with the window
		pcClipper->SetHWnd( 0, G_hWnd );
		PrimSurf->SetClipper( pcClipper );
		SAFE_RELEASE( pcClipper );
	}

	// Fill in the secondary surface VSurf structure
	DDRes = ScndSurf->GetSurfaceDesc(&ddsd);
	Secondary.X = XRes_;
	Secondary.Y = YRes_;
	//Secondary.BPP = (BYTE)BPP_;
	//get BPP from Surface description
	Secondary.BPP = ddsd.ddpfPixelFormat.dwRGBBitCount;

	Secondary.BPSL = ddsd.lPitch;
	
	Secondary.CPP = (Secondary.BPP+1)>>3;
	if (Secondary.BPSL>Secondary.CPP*Secondary.X)
	{
//		fprintf(Logfile," - Note! BPSL(%d) > Scanline size in Bytes(%d).\n",Secondary.BPSL,Secondary.CPP*Secondary.X);
	}
	Secondary.PageSize = Secondary.BPSL * Secondary.Y;
	Secondary.Flags = 0;

	Secondary.Handle = ScndSurf;


	// main system memory
 	DD_MainSurf.X = XRes_;
	DD_MainSurf.Y = YRes_;
	DD_MainSurf.BPP = (BYTE)BPP_;

	V_Create(&DD_MainSurf);

	DD_MainSurf.Flip = V_Flip;

	VESA_VPageExternal(&DD_MainSurf);

	V_Flip(MainSurf);
	ShowWindow(G_hWnd, SW_SHOW);

	int fpcw = _control87(0, 0);
//	FILE *F = fopen("fpu.log", "wt");
//	fprintf(F, "[init] control word = %xh\n" , fpcw);

	// Reset FPU control word to CEIL / 24bit precision
	_control87(_PC_24 | _RC_UP, _MCW_PC | _MCW_RC);
		
//	fpcw = _control87(0, 0);
//	fprintf(F, "[init] control word = %xh\n" , fpcw);
//	fclose(F);

	
	return 0;
}

