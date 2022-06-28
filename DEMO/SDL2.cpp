#include <Base/FDS_VARS.H>
#include <Base/FDS_DECS.H>
#include <SDL.h>


static VESA_Surface DD_MainSurf;

static void V_Flip(VESA_Surface *VS)
{
	SDL_UpdateTexture(static_cast<SDL_Texture*>(VS->Handle), NULL, VS->Data, VS->BPSL);
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


	SDL_Texture * screen_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		VS->X, VS->Y);

	VS->Handle = static_cast<void *>(screen_texture);

	return 0;
}


dword SDL2_InitDisplay()
{
	XRes_ = g_DIP.x;
	YRes_ = g_DIP.y;
	BPP_ = g_DIP.bpp;

	// Creates a Primery Surface in the specified parameters
	if (g_DIP.flags & DISP_FULLSCREEN)
	{
		FullScreen_ = 1;
		// TODO: actually deal with this crap
	} else {
		FullScreen_ = 0;
	}

	// Fill in the secondary surface VSurf structure

	// main system memory
 	DD_MainSurf.X = XRes_;
	DD_MainSurf.Y = YRes_;
	DD_MainSurf.BPP = (BYTE)BPP_;

	V_Create(&DD_MainSurf);

	DD_MainSurf.Flip = V_Flip;

	VESA_VPageExternal(&DD_MainSurf);

	V_Flip(MainSurf);

	int fpcw = _control87(0, 0);
	_control87(_PC_24 | _RC_UP, _MCW_PC | _MCW_RC);
	return 0;
}

