#include "Base/FDS_DECS.H"
#include "FMod.h"

//===============================================================================================
// SIMPLEST.EXE
// Copyright (c), Firelight Multimedia, 1999,2000.
//
// This is the simplest way to play a song through FMOD.  It is basically Init, Load, Play!
//===============================================================================================

#include <windows.h>
#include <conio.h>

#include "api/inc/fmod.h"
#include "api/inc/fmod_errors.h"	// optional

FMOD_SYSTEM* system;

dword FModInit()
{
	FMOD_System_Create(&system);

	unsigned int ver;
	FMOD_System_GetVersion(system, &ver);
	if (ver < FMOD_VERSION)
	{
		printf("Error : You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);
		return 0;
	}
	
   
	if (!FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS))
	{
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
		return 0;
	}

	return 1;
}

dword FModLoadModule(const char *ModuleFilename)
{
	FMUSIC_MODULE *mod;

	mod = FMUSIC_LoadSong(ModuleFilename);
	if (!mod)
	{
		//printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
		return NULL;
	}

	return (dword)mod;
}

/*dword FModLoadModule(void *moduleBuffer, dword moduleLength)
{
	FMUSIC_MODULE *mod;

	mod = FMUSIC_LoadSongMemory(moduleBuffer, moduleLength);
	if (!mod)
	{
		//printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
		return NULL;
	}

	return (dword)mod;
}*/


void FModPlayModule(dword handle)
{
	FMUSIC_MODULE *mod = (FMUSIC_MODULE *)handle;
	FMUSIC_PlaySong(mod);
}

void FModFreeModule(dword handle)
{
	FMUSIC_MODULE *mod = (FMUSIC_MODULE *)handle;
	FMUSIC_FreeSong(mod);
}

void FModClose()
{
	FSOUND_Close();
}

void FModGetModuleInfo(MusicModuleInfo &mmi)
{
	if (!mmi.ModuleHandle) return;
	FMUSIC_MODULE *mod = (FMUSIC_MODULE *)mmi.ModuleHandle;
	mmi.Order = FMUSIC_GetOrder(mod);
	mmi.Pattern = FMUSIC_GetPattern(mod);
	mmi.Row = FMUSIC_GetRow(mod);
}

void FModSetModuleInfo(MusicModuleInfo &mmi)
{
	if (!mmi.ModuleHandle) return;
	FMUSIC_MODULE *mod = (FMUSIC_MODULE *)mmi.ModuleHandle;

	// set module order
	FMUSIC_SetOrder(mod, mmi.Order);

}
