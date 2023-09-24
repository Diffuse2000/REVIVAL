#ifndef DEMO_H_INC
#define DEMO_H_INC
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "FDS_VARS.h"
#include "FDS_DECS.h"
#include "FDS_DEFS.h"


extern void Destroy_Scene(Scene *Sc);

extern void Run_Glato(void);

extern void Initialize_Credits();
extern void Run_Credits();

extern void Initialize_City();
extern void Run_City();

extern void Initialize_Chase();
extern void Run_Chase();

extern void Initialize_Fountain();
extern void Run_Fountain();

extern void Initialize_Greets();
extern void Run_Greets();

extern void Initialize_Crash();
extern void Run_Crash();

#endif