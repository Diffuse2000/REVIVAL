#ifndef _WIN32DISPLAY_H_INCLUDED
#define _WIN32DISPLAY_H_INCLUDED
#include "..\fds\fds_decs.h"
#define POINTER_64
#include <Windows.H>
#include <WinUser.H>
#include <Process.H>
#include <MMSystem.H>

// display related definitions file.
extern long g_demoXRes;
extern long g_demoYRes;
extern long g_fullScreenMode;

enum eDisplayInitFlags
{
	DISP_FULLSCREEN = 1 << 0,
};

struct DisplayInitParams
{
	HWND hWnd;
	HINSTANCE hInst;
	dword x, y;
	dword bpp;
	dword flags;
};
extern DisplayInitParams g_DIP;
#endif