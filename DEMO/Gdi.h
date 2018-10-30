#ifndef _GDI_H_INCLUDED
#define _GDI_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <Windows.H>
#include <WinUser.H>
#include <Process.H>
#include <MMSystem.H>

#include "Win32Display.h"

dword GDI_InitDisplay();
dword V_Create(VESA_Surface *VS);
void V_Flip(VESA_Surface *VS);

#endif