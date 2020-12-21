#include "REV.H"
#include "Koch.h"

#include <vector>
#include <FILLERS/FILLERS.H>

#define NUM_COMETS 0
#define TRAIL_PCLS 110
#define SPWN_INTERVAL 0.05

static Scene *KochSc;
static long CHPartTime = 40.0f*100;
using namespace std;

struct Triangle
{
	Vector Verts[3];
	Vector Colors[3];
};

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

void KochExpand(vector<Triangle> &src, vector<Triangle> &dst)
{
	Vector u, v, n;
	dst.resize(src.size()*6);
	for(mword i = 0; i < src.size(); i++)
	{
		Triangle &t = src[i];
		Vector newVerts[4];
		Vector newColors[4];

		Vector_LComb(&t.Verts[0], &t.Verts[1], 0.5, 0.5, &newVerts[0]);
		Vector_LComb(&t.Verts[1], &t.Verts[2], 0.5, 0.5, &newVerts[1]);
		Vector_LComb(&t.Verts[2], &t.Verts[0], 0.5, 0.5, &newVerts[2]);
		Vector_LComb(&t.Colors[0], &t.Colors[1], 0.5, 0.5, &newColors[0]);
		Vector_LComb(&t.Colors[1], &t.Colors[2], 0.5, 0.5, &newColors[1]);
		Vector_LComb(&t.Colors[2], &t.Colors[0], 0.5, 0.5, &newColors[2]);


		Vector_Sub(&t.Verts[1], &t.Verts[0], &u);
		Vector_Sub(&t.Verts[2], &t.Verts[1], &v);
		Cross_Product(&u, &v, &n);
		Vector_Norm(&n);

		// calculate position on top of extruding pyramid
		u.x = (t.Verts[0].x + t.Verts[1].x + t.Verts[2].x)/3.0;
		u.y = (t.Verts[0].y + t.Verts[1].y + t.Verts[2].y)/3.0;
		u.z = (t.Verts[0].z + t.Verts[1].z + t.Verts[2].z)/3.0;
								   
		Vector_LERP(&newVerts[0], &u, 0.4, &newVerts[0]);
		Vector_LERP(&newVerts[1], &u, 0.4, &newVerts[1]);
		Vector_LERP(&newVerts[2], &u, 0.4, &newVerts[2]);

		//float h = sqrt(6.0)/(float)(3.0* 2.5/2.0 *pow(2.0,_klevel));
		float h = sqrt(6.0)/(float)(3.0* pow(2.5f,(float)(_klevel)));
		Vector_LComb(&newVerts[0], &n, 1, -h*0.0, &newVerts[0]);
		Vector_LComb(&newVerts[1], &n, 1, -h*0.0, &newVerts[1]);
		Vector_LComb(&newVerts[2], &n, 1, -h*0.0, &newVerts[2]);
		Vector_LComb(&u, &n, 1, h*1.0, &newVerts[3]);

		newColors[3].x = ((t.Colors[0].x + t.Colors[1].x + t.Colors[2].x)/3.0);// + ((rand() * 40) >> 15) - 20;
		newColors[3].y = ((t.Colors[0].y + t.Colors[1].y + t.Colors[2].y)/3.0);// + ((rand() * 40) >> 15) - 20;
		newColors[3].z = ((t.Colors[0].z + t.Colors[1].z + t.Colors[2].z)/3.0);// + ((rand() * 40) >> 15) - 20;
		Vector_LERP(&newColors[0], &newColors[3], 0.4, &newColors[0]);
		Vector_LERP(&newColors[1], &newColors[3], 0.4, &newColors[1]);
		Vector_LERP(&newColors[2], &newColors[3], 0.4, &newColors[2]);

		// generate new triangle batch
		dst[i*6+0].Verts[0] = newVerts[2];
		dst[i*6+0].Verts[1] =  t.Verts[0];
		dst[i*6+0].Verts[2] = newVerts[0];

		dst[i*6+1].Verts[0] = newVerts[0];
		dst[i*6+1].Verts[1] =  t.Verts[1];
		dst[i*6+1].Verts[2] = newVerts[1];

		dst[i*6+2].Verts[0] = newVerts[1];
		dst[i*6+2].Verts[1] =  t.Verts[2];
		dst[i*6+2].Verts[2] = newVerts[2];

		dst[i*6+3].Verts[0] = newVerts[0];
		dst[i*6+3].Verts[1] = newVerts[1];
		dst[i*6+3].Verts[2] = newVerts[3];

		dst[i*6+4].Verts[0] = newVerts[1];
		dst[i*6+4].Verts[1] = newVerts[2];
		dst[i*6+4].Verts[2] = newVerts[3];

		dst[i*6+5].Verts[0] = newVerts[2];
		dst[i*6+5].Verts[1] = newVerts[0];
		dst[i*6+5].Verts[2] = newVerts[3];


		dst[i*6+0].Colors[0] = newColors[2];
		dst[i*6+0].Colors[1] =  t.Colors[0];
		dst[i*6+0].Colors[2] = newColors[0];

		dst[i*6+1].Colors[0] = newColors[0];
		dst[i*6+1].Colors[1] =  t.Colors[1];
		dst[i*6+1].Colors[2] = newColors[1];

		dst[i*6+2].Colors[0] = newColors[1];
		dst[i*6+2].Colors[1] =  t.Colors[2];
		dst[i*6+2].Colors[2] = newColors[2];

		dst[i*6+3].Colors[0] = newColors[0];
		dst[i*6+3].Colors[1] = newColors[1];
		dst[i*6+3].Colors[2] = newColors[3];

		dst[i*6+4].Colors[0] = newColors[1];
		dst[i*6+4].Colors[1] = newColors[2];
		dst[i*6+4].Colors[2] = newColors[3];

		dst[i*6+5].Colors[0] = newColors[2];
		dst[i*6+5].Colors[1] = newColors[0];
		dst[i*6+5].Colors[2] = newColors[3];

	}
}

void GenerateKoch3D(TriMesh *T, dword level)
{
	// initialize data structure
	vector<Triangle> Facets[2];	
	Facets[0].resize(4);

	float x = 0.5;
	float y = sqrt(3.0)/6.0;
	float z = sqrt(6.0)/12.0;

	TetraVerts[0] = Vector_Make(-x, -y, -z);
	TetraVerts[1] = Vector_Make( x, -y, -z);
	TetraVerts[2] = Vector_Make( 0,2*y, -z);
	TetraVerts[3] = Vector_Make( 0,  0,3*z);

	// da bomb
	TetraColors[0] = Vector_Make(253, 2, 2);
	TetraColors[1] = Vector_Make(2, 253, 2);
	TetraColors[2] = Vector_Make(2, 2, 253);
	TetraColors[3] = Vector_Make(212, 212, 80);

	Facets[0][0].Verts[0] = TetraVerts[0];
	Facets[0][0].Verts[1] = TetraVerts[1];
	Facets[0][0].Verts[2] = TetraVerts[3];
	Facets[0][0].Colors[0] = TetraColors[0];
	Facets[0][0].Colors[1] = TetraColors[1];
	Facets[0][0].Colors[2] = TetraColors[3];

	Facets[0][1].Verts[0] = TetraVerts[1];
	Facets[0][1].Verts[1] = TetraVerts[0];
	Facets[0][1].Verts[2] = TetraVerts[2];
	Facets[0][1].Colors[0] = TetraColors[1];
	Facets[0][1].Colors[1] = TetraColors[0];
	Facets[0][1].Colors[2] = TetraColors[2];

	Facets[0][2].Verts[0] = TetraVerts[1];
	Facets[0][2].Verts[1] = TetraVerts[2];
	Facets[0][2].Verts[2] = TetraVerts[3];
	Facets[0][2].Colors[0] = TetraColors[1];
	Facets[0][2].Colors[1] = TetraColors[2];
	Facets[0][2].Colors[2] = TetraColors[3];

	Facets[0][3].Verts[0] = TetraVerts[0];
	Facets[0][3].Verts[1] = TetraVerts[3];
	Facets[0][3].Verts[2] = TetraVerts[2];
	Facets[0][3].Colors[0] = TetraColors[0];
	Facets[0][3].Colors[1] = TetraColors[3];
	Facets[0][3].Colors[2] = TetraColors[2];

	mword i;
	for(i=1; i<=level; i++)
	{
		_klevel = i;
		KochExpand(Facets[0], Facets[1]);
		Facets[0] = Facets[1];
		Facets[1].clear();
	}

	T->VIndex = Facets[0].size()*3;
	T->Verts = new Vertex [T->VIndex];

	T->FIndex = Facets[0].size();
	T->Faces = new Face [T->FIndex];

	memset(T->Verts, 0, sizeof(Vertex)*T->VIndex);
	memset(T->Faces, 0, sizeof(Face) * T->FIndex);
	
	for(i=0; i<Facets[0].size(); i++)
	{
		T->Verts[i*3+0].Pos = Facets[0][i].Verts[0];
		T->Verts[i*3+1].Pos = Facets[0][i].Verts[1];
		T->Verts[i*3+2].Pos = Facets[0][i].Verts[2];
		T->Verts[i*3+0].LR = Facets[0][i].Colors[0].x;
		T->Verts[i*3+0].LG = Facets[0][i].Colors[0].y;
		T->Verts[i*3+0].LB = Facets[0][i].Colors[0].z;
		T->Verts[i*3+1].LR = Facets[0][i].Colors[1].x;
		T->Verts[i*3+1].LG = Facets[0][i].Colors[1].y;
		T->Verts[i*3+1].LB = Facets[0][i].Colors[1].z;
		T->Verts[i*3+2].LR = Facets[0][i].Colors[2].x;
		T->Verts[i*3+2].LG = Facets[0][i].Colors[2].y;
		T->Verts[i*3+2].LB = Facets[0][i].Colors[2].z;
		T->Faces[i].A = &T->Verts[i*3+0];
		T->Faces[i].C = &T->Verts[i*3+1];
		T->Faces[i].B = &T->Verts[i*3+2];				
	}
}

static Material *PclT;
static DWord NPMats;

static void InitializeComets(Scene *Sc)
{
	long I,J;
	Image Img;

	Sc->NumOfParticles = NUM_COMETS * TRAIL_PCLS;
	Sc->Pcl = new Particle[Sc->NumOfParticles];
	memset(Sc->Pcl,0,sizeof(Particle)*Sc->NumOfParticles);


	PclT = getAlignedType<Material>(16);//(Material *)getAlignedBlock(sizeof(Material), 16);
	PclT->Txtr = new Texture;
//	memset(PclT->Txtr, 0, sizeof(Texture));
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
	PclT->Txtr->Data = (byte *)Img.Data;

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
	Material *M = (Material *)getAlignedType<Material>(16);
//	memset(T, 0, sizeof(TriMesh));
//	memset(O, 0, sizeof(Object));
//	memset(M, 0, sizeof(Material));
	
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
	GenerateKoch3D(T, 1);
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

	InitializeComets(KochSc);
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

void ProcessComets(Scene *Sc)
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

void Initialize_Koch()
{
	KochSc = (Scene *)getAlignedBlock(sizeof(Scene), 16);
	memset(KochSc,0,sizeof(Scene));
//	LoadFLD(GreetSc,"Scenes\\Greets.FLD");
	KochSc->StartFrame = 0.0;
	KochSc->EndFrame = 400*30.0;

	GenerateScene(KochSc);

	// 5 seconds ending time
	CHPartTime = 100.0f*(KochSc->EndFrame-KochSc->StartFrame)/30.0f;

//	printf("FLD-loaded MEM = %d\n",DPMI_Free_Memory());
//	printf("Scene-Proc MEM = %d\n",DPMI_Free_Memory());
	KochSc->NZP = 1.0f;
	KochSc->FZP = 1500.0f;
	KochSc->Flags |= Scn_ZBuffer;

	Preprocess_Scene(KochSc);
	// also make the appropriate Layer 2 fillers,

	KochSc->Ambient.R = 255;
	KochSc->Ambient.G = 255;
	KochSc->Ambient.B = 255;

	//KochSc->PathingMinVelocity = 0.01;

	// generate Comets
	GenerateComets();
}

void RenderComets()
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



void Run_Koch()
{
	char MSGStr[128];
	long Polys = 0, TTrd;
	TriMesh *T;
	Omni *O;

	long i, timerStack[20], timerIndex = 0;
	for(i=0; i<20; i++)
		timerStack[i] = Timer;

	SetCurrentScene(KochSc);	
	for(T = KochSc->TriMeshHead;T;T=T->Next)
		Polys+=T->FIndex;
	for(O = KochSc->OmniHead;O;O=O->Next)
		Polys++;
	Polys += KochSc->NumOfParticles;
	FList = new Face * [Polys];
	SList = new Face * [Polys];

	View = KochSc->CameraHead;

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

	byte *BPage = new byte[PageSize];

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
				View = KochSc->CameraHead;
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
			CurFrame = KochSc->StartFrame + (KochSc->EndFrame-KochSc->StartFrame) * (float)g_FrameTime / (float)(CHPartTime);
		else
			CurFrame = KochSc->EndFrame;

		Dynamic_Camera();
		if (Keyboard[ScC]) {FC.ISource = View->ISource; Matrix_Copy(FC.Mat,View->Mat); FC.IFOV=View->IFOV;}

		Animate_Objects(KochSc);

		ProcessComets(KochSc);
		Transform_Objects(KochSc);
//		Lighting(KochSc);

		if (CAll)
		{
			Radix_SortingASM(FList,SList,CAll);
			RenderTimeCounter -= Timer;
			// reset per-frame zbuffer statistics
			Render();
			RenderTimeCounter += Timer;
		}
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

		if (Keyboard[ScR])
		{
			mword EntryTimer = Timer;
			while (Keyboard[ScR]);
//			memset(VPage,0,PageSize + XRes*YRes*sizeof(word));
			GlowRaytracer(KochSc, View);
			Flip(MainSurf);
			while (!Keyboard[ScR]);
			while (Keyboard[ScR]);
			Flip(MainSurf);
			while (!Keyboard[ScR]);
			while (Keyboard[ScR]);
			Timer = EntryTimer;
		}


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
	Destroy_Scene(KochSc);
}