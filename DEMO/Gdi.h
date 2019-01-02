#ifndef _GDI_H_INCLUDED
#define _GDI_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>
#include <process.h>
#include <mmsystem.h>

#include "Win32Display.h"
#include <Base/FDS_VARS.H>

dword GDI_InitDisplay();
dword V_Create(VESA_Surface *VS);
void V_Flip(VESA_Surface *VS);

#endif