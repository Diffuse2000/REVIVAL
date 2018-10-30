#include "Rev.H"
#include "Nova.H"

#include <vector>

#define NUM_COMETS 1000
#define TRAIL_PCLS 110
#define SPWN_INTERVAL 1.05

static Scene *NovaSc;
static long CHPartTime = 40.0f*100;
using namespace std;

struct Comet
{
	Color c;
	Vector pos, vel;
	Quaternion spin;

	float spawnTick;
};

static Vector TetraVerts[4];
static Vector TetraColors[4];
static mword _klevel;
static Comet *Comets;

static float _grav = 0.4 * (100.0*100.0);

static Material *PclT;
static DWord NPMats;

static void InitializeComets(Scene *Sc)
{
	long I,J;
	Image Img;

	Sc->NumOfParticles = NUM_COMETS * TRAIL_PCLS;
	Sc->Pcl = new Particle[Sc->NumOfParticles];
	memset(Sc->Pcl,0,sizeof(Particle)*Sc->NumOfParticles);


	PclT = (Material *)getAlignedBlock(sizeof(Material), 16);
	PclT->Txtr = new Texture;
	memset(PclT->Txtr, 0, sizeof(Texture));
	PclT->Txtr->BPP = 32;

	// override textures with 32x32
	New_Image(&Img, 32, 32);

	dword x,y;
	float fx,fy,dx = 2.0/32.0,dy =-2.0/32.0;
	for(x = 0,fx=-1.0; x<32; x++,fx+=dx)
	{
		for(y = 0,fy=1.0; y<32; y++,fy+=dy)
		{
			//Img.Data[x+32*y] = (x<<3)+(y<<11);
			if (fx*fx+fy*fy>=1.0)
				Img.Data[x+32*y] = 0;
			else
				Img.Data[x+32*y] = 0x010101 * (dword)((1.0-(fx*fx+fy*fy))*255.0);
		}
	}
	PclT->Txtr->Data = (char *)Img.Data;

	for(I=0;I<Sc->NumOfParticles;I++)
	{
		// Physical Quantities.
		Sc->Pcl[I].Grav = 0;
		Sc->Pcl[I].Mass = 0;
		Sc->Pcl[I].Charge = 0;
		Sc->Pcl[I].Radius = 0;
		// Particle Position.
		Sc->Pcl[I].F.FlareSize = 0.3f; // indicates this isn't an omni flare polygon.
		Sc->Pcl[I].F.A=Sc->Pcl[I].F.B=&Sc->Pcl[I].V;
		Sc->Pcl[I].F.Filler = &The_MMX_Scalar_32;
		Sc->Pcl[I].F.Txtr = PclT;

		Sc->Pcl[I].Vel.x=0;
		Sc->Pcl[I].Vel.y=0;
		Sc->Pcl[I].Vel.z=0;
	}
}

static void GenerateScene(Scene *Sc)
{
	TriMesh *T = (TriMesh *)getAlignedBlock(sizeof(TriMesh), 16);
	Object *O = new Object;
	Material *M = (Material *)getAlignedBlock(sizeof(Material), 16);
	memset(T, 0, sizeof(TriMesh));
	memset(O, 0, sizeof(Object));
	memset(M, 0, sizeof(Material));
	
	// M->AAAAAAAAAAAAAAAAAAAAAAAAAAAAAhhhhhhhhhhhhhhhhhhhh!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// T->AAAAAAAAAAAAAAAAAAAAAAAAAAAAAhhhhhhhhhhhhhhhhhhhh!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// M,T->DON'T PANIC
	M->Flags = Mat_RGBInterp | Mat_TwoSided;
	M->RelScene = Sc;
	Material *Mat;
	if (MatLib)
	{
		for(Mat=MatLib;Mat->Next;Mat=Mat->Next);
		Mat->Next = M;
	}
	M->Diffuse = 1.0;
	M->Luminosity = 1.0;
	

	// Apply Material
//	Material *M = new Material;
//	Insert_TriMesh2Scene(Sc, T);

	memset(O, 0, sizeof(Object));
	memset(T, 0, sizeof(TriMesh));
	T->Flags = HTrack_Visible;

	O->Prev = NULL;
	O->Next = NULL;
	O->Data = T;
	O->Type = Obj_TriMesh;

	T->Prev = NULL;
	T->Next = NULL;

	// set rotation/position ptrs
	O->Rot =&T->RotMat;
	O->Pos =&T->IPos;
	Vector_Form(&O->Pivot, 0, 0, 0);

	Sc->TriMeshHead = T;
	Sc->ObjectHead = O;

	mword ttt = Timer;
//	GenerateKoch3D(T, 1);
	dword timing = Timer - ttt;
	FILE *F = fopen("timing.dat","wt");
	fprintf(F, "%dcs\n", timing);
	fclose(F);

	for(mword i=0; i<T->FIndex; i++)
	{
		T->Faces[i].Txtr = M;
	}

	T->Pos.CurKey = 0;
	T->Pos.NumKeys = 1;
	T->Pos.Keys = new SplineKey[1];
	memset(T->Pos.Keys, 0, sizeof(SplineKey));
	Quaternion_Form(&T->Pos.Keys->Pos,0.0f,0.0f,0.0f,0.0f);
	T->Scale.CurKey = 0;
	T->Scale.NumKeys = 1;
	T->Scale.Keys = new SplineKey[1];
	Quaternion_Form(&T->Scale.Keys->Pos,70.0f,70.0f,70.0f,0.0f);
	T->Rotate.CurKey = 0;
	T->Rotate.NumKeys = 1;
	T->Rotate.Keys = new SplineKey[1];
	Quaternion_Form(&T->Rotate.Keys->Pos,0.0f,0.0f,0.0f,1.0f);
	Vector_Form(&T->BSphereCtr,0.0f,0.0f,0.0f);
	T->BSphereRad = 10000.0f;

	InitializeComets(NovaSc);
}

static void randomDir(Vector &v)
{
	float x, y, z;
	do
	{
		x = ((rand() - 16384.0)/16384.0);
		y = ((rand() - 16384.0)/16384.0);
		z = ((rand() - 16384.0)/16384.0);
	} while (x*x + y*y + z*z >= 1.0);

	v.x = x;
	v.y = y;
	v.z = z;
	Vector_Norm(&v);
}

static void GenerateComets()
{
	mword i;
	Vector u, v;
	Comets = new Comet [NUM_COMETS];
	for(i=0; i<NUM_COMETS; i++)
	{
		Comets[i].c.R = 64;
		Comets[i].c.G = 128;
		Comets[i].c.B = 255;
		Quaternion_Form(&Comets[i].spin, 1.0, 0.0, 0.0, 0.0);
		do
		{
			randomDir(u);
			randomDir(v);

			if (fabs(Dot_Product(&u, &v)) > 0.3) continue;

			Vector_Scale(&u, 10.0 + rand() * 0.0 / 32768.0, &Comets[i].vel);
			Vector_Scale(&v, 30.0 + rand() * 0.0 / 32768.0, &Comets[i].pos);
			
			break;
		} while (1);
		Comets[i].spawnTick = rand() * SPWN_INTERVAL / 32768.0;
	}
}

static void ProcessComets(Scene *Sc)
{	
	float t = Timer * 0.01;
	float dt = dTime * 0.01;
	Vector u, v;
	mword i, j;
	for(i=0; i<NUM_COMETS; i++)
	{
		// kinetics
		float l = Vector_Length(&Comets[i].pos);
		float g = -_grav / (l*l*l);
		Vector_Scale(&Comets[i].pos, g*dt*0.5, &u);
		Vector_SelfAdd(&Comets[i].vel, &u);
		Vector_Scale(&Comets[i].vel, dt, &v);
		Vector_SelfAdd(&Comets[i].vel, &u);
		Vector_SelfAdd(&Comets[i].pos, &v);
	}

	Particle *PS, *P, *PE;

	// generate trail
	for(i=0; i<NUM_COMETS; i++)
	{
		PS =&Sc->Pcl[i*TRAIL_PCLS];
		PE = PS + TRAIL_PCLS;
		
		while (Comets[i].spawnTick < t)
		{
			for(P=PS; P<PE; P++)
			{
				if (P->Flags & Particle_Active) continue;

				randomDir(v);
				Vector_LComb(&Comets[i].vel, &v, 0.0, 1.0, &P->Vel);

				// calc position
				float latency = t-Comets[i].spawnTick;
				Vector_LComb(&Comets[i].pos, &P->Vel, 1.0, latency, &P->V.Pos);
				
				P->Charge = 2.5 - latency;
				P->Color.R = 63;
				P->Color.G = 127;
				P->Color.B = 255;
				P->Flags |= Particle_Active;
				break;
			}
			Comets[i].spawnTick += SPWN_INTERVAL;
		}
	}

	// kinetics/transform
	for(P=Sc->Pcl, PE = P + Sc->NumOfParticles; P<PE; P++)
	{
		if (!(P->Flags&Particle_Active)) continue;

		Vector_Scale(&P->Vel, dt, &v);
		Vector_SelfAdd(&P->V.Pos, &v);

		Vector_Sub(&P->V.Pos,&View->ISource,&v);
		MatrixXVector(View->Mat,&v,&P->V.TPos);

		if (P->V.TPos.z>=1.0)
		{
			P->V.RZ = 1.0/P->V.TPos.z;
			P->V.PX = CntrX+FOVX*P->V.TPos.x*P->V.RZ;
			P->V.PY = CntrY-FOVY*P->V.TPos.y*P->V.RZ;
//      P->PRad = 0;//FOVX*P->Radius*RZ;
			P->V.Flags = 0;
		} else P->V.Flags |= Vtx_VisNear;

		// dummy illumination model
		P->V.LB = P->Color.B * P->Charge / 5.0;
		P->V.LG = P->Color.G * P->Charge / 5.0;
		P->V.LR = P->Color.R * P->Charge / 5.0;
		P->Charge -= dt;
		if (P->Charge < 0.0) P->Flags &=~Particle_Active;
	}


}

void Initialize_Nova()
{
	NovaSc = (Scene *)getAlignedBlock(sizeof(Scene), 16);
	memset(NovaSc,0,sizeof(Scene));
//	LoadFLD(GreetSc,"Scenes\\Greets.FLD");
	NovaSc->StartFrame = 0.0;
	NovaSc->EndFrame = 400*30.0;

	GenerateScene(NovaSc);

	// 5 seconds ending time
	CHPartTime = 100.0f*(NovaSc->EndFrame-NovaSc->StartFrame)/30.0f;

//	printf("FLD-loaded MEM = %d\n",DPMI_Free_Memory());
//	printf("Scene-Proc MEM = %d\n",DPMI_Free_Memory());
	NovaSc->NZP = 1.0f;
	NovaSc->FZP = 1500.0f;
	NovaSc->Flags |= Scn_ZBuffer;

	Preprocess_Scene(NovaSc);
	// also make the appropriate Layer 2 fillers,

	NovaSc->Ambient.R = 255;
	NovaSc->Ambient.G = 255;
	NovaSc->Ambient.B = 255;

	//NovaSc->PathingMinVelocity = 0.01;

	// generate Comets
	GenerateComets();
}

static void RenderComets()
{
	Vector u, v;
	for(mword i=0; i<NUM_COMETS; i++)
	{
		Vector_Sub(&Comets[i].pos, &View->ISource, &u);
		MatrixXVector(View->Mat, &u, &v);

		if (v.z < CurScene->NZP) continue;

		float rz = 1.0 / v.z;
		long x = CntrX + FOVX * v.x * rz;
		long y = CntrY - FOVY * v.y * rz;

		if (x<0||y<0||x>=XRes||y>=YRes) continue;
		((dword *)VPage)[x + y*XRes] = 0xFFFFFF;
	}
}



void Run_Nova()
{
	char MSGStr[128];
	long Polys = 0, TTrd;
	TriMesh *T;
	Omni *O;

	long i, timerStack[20], timerIndex = 0;
	for(i=0; i<20; i++)
		timerStack[i] = Timer;

	SetCurrentScene(NovaSc);	
	for(T = NovaSc->TriMeshHead;T;T=T->Next)
		Polys+=T->FIndex;
	for(O = NovaSc->OmniHead;O;O=O->Next)
		Polys++;
	Polys += NovaSc->NumOfParticles;
	FList = new Face * [Polys];
	SList = new Face * [Polys];

	View = NovaSc->CameraHead;

	Ambient_Factor = 0.25;
	Diffusive_Factor = 1.0;
	Specular_Factor = 0.0;
	Range_Factor = 1.0;
	ImageSize = 2.5;

	dword RenderTimeCounter = 0;
	dword numFrames = 0;

	FillerPixelcount = 0;
	TTrd = Timer;


	// position for flood logo generation
	View = &FC;	
	FC.IFOV = 75;
	FC.ISource.x = 100*TetraVerts[3].x;
	FC.ISource.y = 100*TetraVerts[3].y;
	FC.ISource.z = 100*TetraVerts[3].z;
	FC.ITarget.x = 0.0;
	FC.ITarget.y = 0.0;
	FC.ITarget.z = 0.0;
	Kick_Camera(&FC.ISource, &FC.ITarget, 180.0, FC.Mat);

	char *BPage = new char[PageSize];

	VESA_Surface Blur;
	memcpy(&Blur,MainSurf,sizeof(VESA_Surface));
	Blur.Data = BPage;
	Blur.Flags = VSurf_Noalloc;
	Blur.Targ = VGAPtr;


	while ((!Keyboard[ScESC])&&Timer<CHPartTime)
	{
		numFrames++;

		if (Keyboard[ScTab])
		{
			if (View == &FC)
				View = NovaSc->CameraHead;
			else
				View = &FC;
		}


		dTime = Timer-TTrd;
		// fast forward/rewind
		if (Keyboard[ScF2])
		{
			Timer += dTime * 8;
		}
		if (Keyboard[ScF1])
		{
			if (dTime * 8 > Timer)
				Timer = 0;
			else
				Timer -= dTime * 8;
		}
		static bool pause_mode = false;
		if (Keyboard[ScP])
			pause_mode = true;
		if (Keyboard[ScU])
			pause_mode = false;
		if (pause_mode)
		{
			Timer = TTrd;
		}
		g_FrameTime = TTrd = Timer;

		
		// Clear Framebuffer and ZBuffer
		memset(VPage,0,PageSize + XRes*YRes*sizeof(word));

		if (Timer < CHPartTime)
			CurFrame = NovaSc->StartFrame + (NovaSc->EndFrame-NovaSc->StartFrame) * (float)g_FrameTime / (float)(CHPartTime);
		else
			CurFrame = NovaSc->EndFrame;

		Dynamic_Camera();
		if (Keyboard[ScC]) {FC.ISource = View->ISource; Matrix_Copy(FC.Mat,View->Mat); FC.IFOV=View->IFOV;}

		Animate_Objects(NovaSc);

		ProcessComets(NovaSc);
		Transform_Objects(NovaSc);
//		Lighting(NovaSc);

		if (CAll)
		{
			Radix_SortingASM(FList,SList,CAll);
			RenderTimeCounter -= Timer;
			// reset per-frame zbuffer statistics
			Render();
			RenderTimeCounter += Timer;
		}
		RenderComets();
		// FPS printer
		timerStack[timerIndex++] = Timer;
		if (timerIndex==20) 
		{
			timerIndex = 0;
			sprintf(MSGStr,"%f FPS", 2000.0/(float)(timerStack[19]-timerStack[timerIndex]) );
		} else {
			sprintf(MSGStr,"%f FPS", 2000.0/(float)(timerStack[timerIndex-1]-timerStack[timerIndex]) );
		}
//		OutTextXY(VPage,0,0,MSGStr,255);
//		sprintf(MSGStr, "%dK pixels/frame" , (long)(FillerPixelcount/(1000.0*numFrames)));
//		OutTextXY(VPage,0,15,MSGStr,255);
//		sprintf(MSGStr, "%dK pixels/second" , (long)(FillerPixelcount/1000.0 * 100.0 / RenderTimeCounter));
//		OutTextXY(VPage,0,30,MSGStr,255);
		// display messages.
/*		long I, Y = -15;
		for(I=0;I<10;I++)
		if (MsgStr[I])
		{
			Y = OutTextXY(VPage,0,Y+15,MsgStr[I],255);
			if (Timer>MsgClock[I]+150)
			{
				delete MsgStr[I];
				MsgStr[I]=NULL;
			}
		}*/

/*		if (Keyboard[ScR])
		{
			mword EntryTimer = Timer;
			while (Keyboard[ScR]);
//			memset(VPage,0,PageSize + XRes*YRes*sizeof(word));
			GlowRaytracer(NovaSc, View);
			Flip(MainSurf);
			while (!Keyboard[ScR]);
			while (Keyboard[ScR]);
			Flip(MainSurf);
			while (!Keyboard[ScR]);
			while (Keyboard[ScR]);
			Timer = EntryTimer;
		}*/


#ifdef _C_WATCOM
		Flip(Screen);
#else
		Flip(MainSurf);
#endif
//		Modulate(MainSurf,&Blur,0x707070,0x808080);
//		Flip(&Blur);
		
	} Timer-=CHPartTime;
//	if (Keyboard[ScESC])
//	{
//		#ifdef Play_Music
//		ShutDown();
//		#endif
//		FDS_End();
//		exit(-1);
//	}
	while (Keyboard[ScESC]) continue;
	if (Timer<0) Timer = 0;

	delete [] FList;
	delete [] SList;
	Destroy_Scene(NovaSc);
}