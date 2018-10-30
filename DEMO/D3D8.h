#ifndef _REV_D3D8_H_INCLUDED
#define _REV_D3D8_H_INCLUDED
#include "Win32Display.h"

dword D3D8_InitDisplay();
dword D3D8_RemoveDisplay();
void D3D8_Flip(VESA_Surface *VS);

#endif