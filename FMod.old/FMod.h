#ifndef DREKMAN_FMOD_H
#define DREKMAN_FMOD_H


dword FModInit();

dword FModLoadModule(char *ModuleFilename);

void FModPlayModule(dword handle);

void FModFreeModule(dword handle);

void FModClose();

void FModGetModuleInfo(MusicModuleInfo &mmi);

void FModSetModuleInfo(MusicModuleInfo &mmi);

#endif
/*$ preserve end $*/
