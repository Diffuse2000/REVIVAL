#define POINTER_64
#include "GDI.H"
#include "Base/FDS_DECS.H"

/*
	GDI VIDEO DRIVER 
	Part of the MSR Project by Silvatar/Flood.

	LOG: 22.3.99: Initiated.
		 22.3.99: V1.0 Complete, Support for the standard
				  [W32] Init, Flip and Remove API.

		 24.10.99: Upgrade to BITMAPV4HEADER
*/



static char *DIBMem;
static BITMAPINFO *DIBMap;

// these are updated externally via the W32-Preinit.
static HINSTANCE G_hInst;
static HWND G_hWnd;
static DWORD XRes_,YRes_,BPP_;

static VESA_Surface Secondary;

dword GDI_InitDisplay()
{
	G_hInst = g_DIP.hInst;
	G_hWnd = g_DIP.hWnd;
	dword x = g_DIP.x;
	dword y = g_DIP.y;
	dword bpp = g_DIP.bpp;

	int32_t MXR,MYR;
	RECT rectum;

	XRes_ = x;
	YRes_ = y;
	BPP_ = bpp;

	MXR=GetSystemMetrics(SM_CXSCREEN);
	MYR=GetSystemMetrics(SM_CYSCREEN);

	int window_x = x;//, x*2
	int window_y = y; //y, y*2
	rectum.left = (MXR-window_x)>>1;
	rectum.right = (MXR+window_x)>>1;
	rectum.top = (MYR-window_y)>>1;
	rectum.bottom = (MYR+window_y)>>1;
	if (!AdjustWindowRectEx(  &rectum, WS_OVERLAPPEDWINDOW,FALSE,WS_EX_APPWINDOW))
		return 1;

//	SetWindowPos(G_hWnd,HWND_TOP,rectum.left, rectum.top, rectum.right-rectum.left, rectum.bottom-rectum.top,SWP_SHOWWINDOW);
//	SetWindowLong(G_hWnd,GWL_STYLE,WS_OVERLAPPED);

	SetWindowLong(G_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	SetWindowPos(G_hWnd,HWND_NOTOPMOST,rectum.left, rectum.top, rectum.right-rectum.left, rectum.bottom-rectum.top,0);

    if (!G_hWnd)
    {
		return 1;
	}

	// no way to get pitch, so we just use the usual metrics
/*	MainVSurf->CPP = (MainVSurf->BPP+1)>>3;
	MainVSurf->Pitch = MainVSurf->CPP * MainVSurf->X;
	MainVSurf->PageSize = MainVSurf->Pitch * MainVSurf->Y;
	MainVSurf->FB = malloc(MainVSurf->PageSize);
	memset(MainVSurf->FB,0,MainVSurf->PageSize);*/
	Secondary.X = XRes_;
	Secondary.Y = YRes_;
	Secondary.BPP = (BYTE)BPP_;
	V_Create(&Secondary);
	//Secondary.Flags |= VSurf_Secondary;

	Secondary.Flip = V_Flip;
	VESA_VPageExternal(&Secondary);

	ShowWindow(G_hWnd,SW_SHOW);
	UpdateWindow(G_hWnd);
//	ActiveVSurf = NULL;
	return 0;
}


static dword V_Create(VESA_Surface *VS)
{
	VS->CPP = (VS->BPP+1)>>3;
	VS->BPSL = VS->CPP * VS->X;
	VS->PageSize = VS->BPSL * VS->Y;
	dword ZBufferSize = sizeof(word) * VS->X * VS->Y;
	if (!(VS->Data = (byte *)malloc(VS->PageSize + ZBufferSize))) return 1;
	memset(VS->Data,0,VS->PageSize + ZBufferSize);
//	VS->Flags = VSurf_Exists;

	// create GDI surface descriptor
	BITMAPINFOHEADER *BIH;
	switch (VS->BPP)
	{
		case 8:
			DIBMem = new char [sizeof(BITMAPINFOHEADER)+sizeof(QColor)*256];
			DIBMap = (BITMAPINFO *)DIBMem;
			BIH = &DIBMap->bmiHeader;
			BIH->biSize = sizeof(BITMAPINFOHEADER);
			BIH->biWidth = VS->X;
			BIH->biHeight = -(signed)VS->Y;
			BIH->biPlanes = 1;
			BIH->biBitCount = 8;
			BIH->biCompression = BI_RGB;
			BIH->biSizeImage = 0;
			BIH->biXPelsPerMeter = 0;
			BIH->biYPelsPerMeter = 0;
			BIH->biClrUsed = 0;
			BIH->biClrImportant = 0;
			memset(DIBMap->bmiColors,0,sizeof(QColor)*256);
			//VS->Pal = (VPalette)DIBMap->bmiColors;
		break;
		case 16:
			DIBMem = new char[sizeof(BITMAPINFOHEADER)+12];
			DIBMap = (BITMAPINFO *)DIBMem;
			BIH = &DIBMap->bmiHeader;
	
			// create an internal 16bit color DIB
			BIH->biSize = sizeof(BITMAPINFOHEADER);
			BIH->biWidth = VS->X;
			BIH->biHeight = -(signed)VS->Y;
			BIH->biPlanes = 1;
			BIH->biBitCount = 16;
			BIH->biCompression = BI_BITFIELDS;
			BIH->biSizeImage = 0;
			BIH->biXPelsPerMeter = 0;
			BIH->biYPelsPerMeter = 0;
			BIH->biClrUsed = 0;
			BIH->biClrImportant = 0;

			// 16bit color
			((uint32_t*)DIBMap->bmiColors)[0]=0xf800;
			((uint32_t*)DIBMap->bmiColors)[1]=0x07e0;
			((uint32_t*)DIBMap->bmiColors)[2]=0x001f;
		break;
		case 32:
			DIBMem = new char[sizeof(BITMAPINFOHEADER)+12];
			DIBMap = (BITMAPINFO *)DIBMem;
			BIH = &DIBMap->bmiHeader;
	
			// create an internal 16bit color DIB
			BIH->biSize = sizeof(BITMAPINFOHEADER);
			BIH->biWidth = VS->X;
			BIH->biHeight = -(signed)VS->Y;
			BIH->biPlanes = 1;
			BIH->biBitCount = 32;
			BIH->biCompression = BI_BITFIELDS;
			BIH->biSizeImage = 0;
			BIH->biXPelsPerMeter = 0;
			BIH->biYPelsPerMeter = 0;
			BIH->biClrUsed = 0;
			BIH->biClrImportant = 0;

			// 16bit color
			((uint32_t*)DIBMap->bmiColors)[0]=0xff0000;
			((uint32_t*)DIBMap->bmiColors)[1]=0x00ff00;
			((uint32_t*)DIBMap->bmiColors)[2]=0x0000ff;
		break;
		default: return 1;
	}
	VS->Handle = DIBMem;

	return 0;
}

static void V_Flip(VESA_Surface *VS)
{
	HDC dc;
	int ret;
	if (!(dc = GetDC(G_hWnd))) return;

	RECT rectum;
	GetClientRect( G_hWnd, &rectum);

	ClientToScreen( G_hWnd, (POINT*)&rectum.left );
	ClientToScreen( G_hWnd, (POINT*)&rectum.right );

	
	ret = StretchDIBits(dc, 0, 0, //XRes_,YRes_,
		rectum.right-rectum.left, rectum.bottom-rectum.top,
							0, 0, VS->X, VS->Y,
							VS->Data, (BITMAPINFO *)VS->Handle, DIB_RGB_COLORS, SRCCOPY);
	
	if (ret == GDI_ERROR) return;

	if (!(ret = ReleaseDC(G_hWnd, dc))) return;
}

static void V_Remove()
{
	delete Secondary.Data;
	delete (char *)(Secondary.Handle);
}

