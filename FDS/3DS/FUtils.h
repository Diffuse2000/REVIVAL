#ifndef _FUTILS_H_
#define _FUTILS_H_

#include "..\FDS_Vars.h"
#include "..\FDS_Decs.H"

void ReadASCIIZ(char **s);
void Read(void *Ptr,DWord Size);
void SwapYZ(Vector *Vec);
void SwapYZ(Quaternion *Q);
void SwapYZ(Matrix Mat);
Object *FindObject(short Number);
Object *FindObject(char *Name);

#endif