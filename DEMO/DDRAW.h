#ifndef _REV_DDRAW_H_INCLUDED
#define _REV_DDRAW_H_INCLUDED

#define POINTER_64
#include <DDraw.H>
#include "Win32Display.h"

#define SAFE_RELEASE(x) if (x) x->Release()

dword DDRAW_InitDisplay();
dword DDRAW_RemoveDisplay();

void DDRAW_Flip(VESA_Surface *VS);


#endif