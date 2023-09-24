#ifndef _REV_DDRAW_H_INCLUDED
#define _REV_DDRAW_H_INCLUDED

#define POINTER_64
#include <ddraw.h>
#include "Win32Display.h"
#include "Base/FDS_VARS.h"

#define SAFE_RELEASE(x) if (x) x->Release()

dword DDRAW_InitDisplay();
dword DDRAW_RemoveDisplay();

void DDRAW_Flip(VESA_Surface *VS);


#endif