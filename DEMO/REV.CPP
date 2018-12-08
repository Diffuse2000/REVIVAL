#pragma comment(lib, "delayimp")  
#pragma comment(lib, "fmod")  

/******************************************************************************
	Source code for REVIVAL / FLOOD.

	Final version: MOV' 98 (dec' 98)
	
	Revising started at Jan 2002 using FDS v0.76A
	In progress: eval. 70% Complete
 
	FDS version promoted to 0.84 at 22.04.02
	FDS version promoted to 0.85 at 05.07.02
	

Rendering pipeline optimization
-------------------------------
current rendering pipeline uses:
1. Triangle meshes : supported
1.1. arrays of fixed size vertex structures.
1.1.1. Vertex position, normal, U, V, EU, EV
1.1.2. Vertex T.position, RZ, UZ, VZ, EUZ, EVZ
1.1.3. Vertex lighting [float R, G, B]
1.1.4. Vertex visibility Flags
1.2. model / instance seperation : not supported
2. Transform pipeline with bounding spheres and backface culling, floating point 4x3 transform.
3. Lighting pipeline is non-optimized, supports ambient and diffuse lighting.
4. Painter's algorithm: base-256 Radix back-to-front sorting.
5. Frustrum clipper : supported
5.1. stage : immidiately before rasterization
5.2. input : a triangle
5.3. output : a set of triangles
6. Polygon rasterization routines:
6.1. Polygon support : triangles only
6.2. ZBuffering : no support
6.3. Gouraud : supported
6.3.1. perspective correction : not supported
6.3.2. Modulation with screen : supported, uses MMX machine code.
6.4. Texture mapping : supported
6.4.1. perspective correction : piecewise linear per 16 pixels, floating point.
6.4.2. 8bit textures onto 16/32bit framebuffer : not supported
6.4.3. Mipmapping : not supported.
6.4.4. Tiled textures : not supported.
6.4.5. Maginification filter : point
6.4.6. Alpha Blending : constant 50% (aka 'Transparent' texture)
6.4.7. Intrinsics : SMC, uses FPU per 16 pixels.
6.4.8. UV Wrapping : supported

planned rendering pipeline uses:
1. Triangle meshes : supported
1.1. structure containins arrays of vertex components.
1.1.1. Vertex position, normal, U, V, Flags.
1.1.2. It is yet unclear if vertices should be formed before or after clipping,
	   such vertices should contain px,py, 1/z, [dword ARGB].
1.2. model / instance seperation : feasable, but not supported.
2. Transform pipeline with bounding boxes, spheres and backface culling, 
   floating point 4x3 and SSE 4x4 strided structure transform.
3. Optimized lighting pipeline with ambient, diffuse and possibly specular [using MMX modulate 2x].
4 Z-Buffering : base-256 Radix front-to-back, texture-based grouping sort.
				all transparent/addative objects should be rendered last.
5. Frustrum clipper : supported
5.1. stage : immidiately before rasterization OR batched during/after transform pipeline
5.2. input : a triangle
5.2. output : an ngon
6. Polygon rasterization
6.1. Polygon support : simple convex ngon
6.2. ZBuffering : supported
6.2.1. precision : 16bit
6.2.2. interpolation : integer, scaled fixed pt. 1/z.
6.2.3. comparision : integer compare, could configurate with SMC.
6.2.4. comparision impl. : conditional jump across inner loop
6.2.5. clear : either configurate compare, or full Z-buffer clear per frame.
6.3. Gouraud : supported
6.3.1. perspective correction : per scanline, or automatic constant-z polygon fragmentation
6.3.2. Modulation with screen : not supported
6.4. Texture mapping : supported
6.4.1. perspective correction : piecewise linear per 16 pixels, FP or SSE.
	   check possibility for unrolling to utilitize SSE 4 parallel division units
6.4.2. 8bit textures onto 32bit framebuffer : supported
6.4.3. Mipmapping : supported for both 8bit and 32bit, [max width 256 pixels]. 
6.4.3.1. Minification Filter : point, possibily linear.
6.4.3.2. Check if seperate palettes are required.
6.4.4. Tiled textures : supported, configurable tile size. 
		Cannot cooperate with bilinear filtering
6.4.5. Maginification Filter : point. (test speed for bilinear)
6.4.6. Alpha Blending : configurable constant, gouraud alpha, or texture/page alpha channel
6.4.7. Initrinsics : ngon section traversing written in C. 
		uses SSE per 16/32 pixels and MMX within the inner loop.
		Z-Buffer pointer omitted (use SMC and an immidiate delta from framebuffer ptr)
		Texture/Mipmap palette pointer omitted (use constant delta from texture ptr)

Revised Pipeline design
1.Data storage
1.1. SCENE contains an array of Objects<-->Trimeshes, and an array of Models.
Renderer object operates over scene, and contains an index list, and an allocated
SOA vertex buffer large enough to contain all vertices in scene + clipping extra.
VB contains:
transformed X, Y, Z, RGB, U, V
screen X, screen Y, 1/Z, RGB, U/Z, V/Z


1.2. Models 
1.2.1. contain an SOA vertex buffer with the following data types
position X,Y,Z. normal X,Y,Z. U, V.
1.2.2. contain an SOA face information
index list
material global ID or ptr

1.3. Trimeshes have a pointer to a model, position and rotation status.

	Coding log since 11 Feb 2002 (earlier entries may be out of order of occurance or missing)
	==========================================================================================

	[late DEC 2001-early JAN 2002] 
	REVIVAL Project reopened
	
	- Removed JPegLibD.lib and MIDAS.lib from the VC6 build.
	- Added matching __asm blocks for all #pragma aux. to remove dependancy in wcc
	- Added support for extrnal initialization of logical video unit in FDS
	- Removed unconditional usage of DPMI routines in FDS
	- Changed MMX detection to use CPUID instead of INT 6h (invalid opcode trap)
	- Fixed a bug in the FLD reader which caused it to incorrectly count #faces.
	- Fixed a bug in the splines derivate calculation code which caused out of array bounds write.
	- Changed external assembly functions naming convension to support both wcc and vc6.
	- Fixed bug caused by assuming char type to be unsigned.
	- Added support for the win32 platform in the demo.
	- Added support for GDI-based display.
	- Added support for windows multimedia timer (winmm.lib).
	- Fixed optimized VC6 builds by wrapping assembly function calls with pushad/popad.
	- Fixed a memory leak in image manipulation code.
	- Removed the automatic application of the Reset_XForm function.
	- Fixed texture coordinate wrapping support
	- Removed a read from null (address 0) in the FOUNTAIN demo part.
	- Fixed calculation of the FOV spline from ZoomFactor envelope in FLD reader.
	- Added support for IJLD, Intel's JPeg decoding library.
	- Added support for the FMod sound system.
	- Added padding to fix an otherwise unstable read in the Run_Distort, CITY demo part.
	- Added full raster support in arbitrary resolution.
	- Added Wobbler(Grid renderer) support in arbitrary resolution.
	- Added automatic Mipmap generation for textures
	- Changed sprite scaling to be proportional to the display resolution.
	- Changed Flare generation code to be invarient of target image resolution.
	- Improved Flare generation parameters.
	- Fixed Far Z frustrum clipping.
	- Changed application of fog to the pre-filler stage.
	- Added a fogged version for transparent textured surfaces. (two-pass prefiller)
	- Changed automatic flare image generation to globally used single flare
	- Changed Distortion parameters for CITY demo part.
	- Added 32x32 MMX-based point filtering shaded-addative image scaler.
	- Added Skycube generation code to FDS.
	- Added support for particle global and omnilight interpolated size in scaler (prefiller)
	- Added support for particle color before calling the 32x32 shaded scaler.
	- Increased Far Z clipping plane in CITY demo part from 2500 to 5000.
	- Added temporal antialiasing for particle generation in FOUNTAIN demo part.
	- Increased particle velocity, spawn rate and maximal amount in FOUNTAIN demo part.
	- Added support for arbitrary resolution in GLAT demo part.
	- Fixed cross-fade rate overflow bug in GLAT demo part.
	- Optimized grid calculations in GLAT demo part.
	- Optimized framebuffer modulation (Alphablending) code.
	- Added fill rate (pixels/sec) measuring code.
	- Improved Lighting model.
	- Fixed a bug with backface culling for scaled objects
	- Fixed a bug with lighting for scaled objects.
	- Added hardcoded fixup for point light range in GREETS demo part.

	11/02/02
	- Coding log created
	- Recorded all known entries dated before 11/02/02

	12/02/02
	- Added DirectDraw 6 windowed/fullscreen support

	13/02/02
	- Fixed Framerate dependancy in GLAT demo part.
	- Seperated initialization sequence in the GLAT demo part.
	- Fixed a bug in the page modulation routine.
	- Adjusted outer particle generation and rendering parameters in FOUNTAIN demo part.
	- Added Motion blur to FOUNTAIN demo part.

	14/02/02
	- Added support in per-particle imagesize.
	- Corrections to the lighting parameters are now made in GREETS demo part, 
	  removed hardcoded fixup from lighting module.
	- Fixed 'align to path' type animation.
	- Fixed an allocation bug in the FDS messaging system.

	15/02/02
	- Added Direct3D 8 windowed/fullscreen support [inoperative]
	- Attempted to correct DirectX memory allocation errors. Those also occur in
	  MSR_Proj and when using DX8, and dont appear to cause anything to malfunction. 
	  It seems to be associated with usage of two seperate threads within the program's 
	  scope, or the order by which the system is shut down.

	18/02/02
	- Added RGB color support for flat shader.
	- Optimized flat shader innerloop using MOVNTQ

	8/03/02
	- Added rain FX prototype
	- Fixed FP control word bug when using DDraw fullscreen.

	29/03/02
	- Fixed mistakenly assumed unsigned char type in mipmap generation

	30/03/02
	- Added C texture mapper prototype (1)

	31/03/02
	- Fixed bounding sphere / frustrum intersection tests.
	- Fixed Vertex visibility flag calculation across rendering pipeline
	- Added convex polygon clipper.
	- Added modifyable near-Z clipping plane support.

	01/04/02
	- Fixed ZBuffer interpolation errors
	- Written MMX assembly code for IX mapper inner-inner loop 
	- CITY demo part : 512x384 14 FPS...
	- Normalized UV texture coordinates
	- Added mipmapping/block tiling support
	- zbuffer imprecision within subinnerloop routine should be fixed
	- Added 'luminosity' support
	- Fixed sorting for ZBuffered scenes
	- Added Transparent/Fogged versions of new Texture/Gouraud mapper
	- Added Arbitrary Texture resolution support in IX mapper (should code texture read (actually, the entire innerloop) in assembly)

	02/04/02
	- Added ZBuffer support for sprite scalers
	- FOUNTAIN demo part has a marginal framerate decrease when using Z-Buffering mappers.
	- profiled FOUNTAIN: almost 100% rasterization runtime. image scaler rountine needs to be
	  optimized. 

    06/04/02
	- Added Polygon/mipmap clipper. This has a poor implementation, and is therefore executed
	  only on polygons that occupy 10% or more of the screen surface.

	08/04/02
	- Fixed Polygon/mipmap clipper, log/exp tables
	- Added FF/Rew/Pause to all demo parts
	- Fixed Reflection transform, Reflected backface culling
	- Reverted to standard lighting pipeline in CITY demo part.
	- NOTE: ZBuffered flat/gouraud [with Trans/Fog support] only should be coded soon.
	+ CITY demo part related issues
		- NOTE: Monorail/tracks are not properly attached to sea level (h + 86 units)
		- NOTE: Sea level in original scene file decreases from 20 to 0 throughout runtime.
				right now sea level was changed to be constantly 0.

	13/04/02
	- Added IX polygon mappers with Z-buffering and transparency support for texture only and
	  gouraud only.
	- Polygon/mipmap clipper is more efficient and can be used on polygons that cover 2%
	  or more screen area with minimal performance loss
	- Fixed a minor sorting bug with transparent objects
	- Mounted CHASE scene, in principle it is operative with the availability of Z-buffering
	- Fixed a bug with flat-shaded polygons, which could have 'Noshading' flag set.	

	14/04/02
	- Added generic MMX register emulation class. 

    18/04/02
	- Rewritten lighting model to conform to LW3D.
	- GREETS demo part : 512x384 21 FPS...
	
	19/04/02
	- Added support for single (central) Z-compare for particles and flares.
	- Changed Face structure to allow convinient storage of flare related information
	- Fixed bug in GLATO demo part when YRes isn't a multiple of 8.
	- Added new C functions for wobbler f/x used in GLATO demo part.

	20/04/02
	- Added precalculation of static lighting over static (stationary) objects
	- GREETS demo part : 512x384 22 FPS...

	21/04/02
	- Added tile-based rendering for particle sprites.
	- FOUNTAIN demo part : minimal 512x384 FPS went from 8.9 to 10.2.

	22/04/02
	- Fixed an out of array bounds read bug in ConvertBPP, but the fixup is
	  hardcoded for 24->32bit conversions.

    23/04/02
	- Added Rain prototype for CITY scene (2)

	??/04/02
	- Fixed Rain positioning so each drop has nonzero screen time.

	- LAZYNESS STRIKE 

	27/06/02
	- Added IXTGZM mapper, integrated with FDS. currently draws flatpoly spans. (3)

	02/07/02
	- Improved IXTGZM mapper, draws gouraud shaded polygons with partial zbuffering support

	03/07/02
	- Fixed IXTGMZ mapper, renders correct zBuffered gouraud.

	04/07/02
	- Added full texture coordinate interpolation in IXTGZM mapper.
	- IXTGZM : Gouraud only, 20M pixels/second, 40% of the maximal temporal bus write.

	22/07/02
	- Fixed texture coordinate interpolation bug
	- Added SMC support for arbitrarily sized textures up to 256x256.	
	- IXTGZM : Texture/Gouraud, 15M pixels/second, 30% of the maximal temporal bus write.

    26/07/02
	- Added 4x4 block-tiled texturing support (improves cache utilization)
	- Added a transparent version of IXTGZM
	- ITGZM : Texture/Gouraud, 17M pixels/second
	+ GREETS demo part : 512x384 25FPS...
	+ FOUNTAIN demo part : 512x384 12FPS minimum
		- NOTE: Modified vortex texture generation to create a 4x4 block-tiled texture

	27/07/02
	+ Added a profiler prototype within scene code
		- NOTE: frustrum/mapper now take only 50-60% of the total running time.
			CITY,GREETS: xform+lighting take 25-30% together
			flip takes over 15%

	01/08/02
	- Added bounding sphere elimination test to the lighting model
	+ SSE'ed the lighting model code
		- flip was marked as the next target :)
	- Added 1 bit of Zbuffer interpolation precision by rounding instead of truncation
	- Added a convinient aligned memory allocator, aligned texture addrs to 64

	- HUGE LAZYNESS STRIKE 

	07/02/03
	- Removed all memory frees because program appears to be damaging the heap. The
	malfunctioning code is yet to be found.

	- HUGE LAZYNESS STRIKE 
	02/07/03
	- Imported project to VC7
	- Corrected DirectDraw 6 fullscreen display code

	- HUGE LAZYNESS STRIKE 
	10/01/04
	- Project Resumed
	- Added configuration DB.

	
	Remarks
	(1): prototype C mapper has the following specs.
	Scan-conversion type: Counterclockwise convex polygon
	Perspective correction: Bilinear upto 16/32 pixel subspans
	Limited tiling support
	Limited arbitrary texture size support (powers of 2 only)
	Z-Buffering supported
	Z-Buffer:
		std. (1/z) W-buffer, fp. upto subspans, 16bit Z-Bu,ffer precision
	Gouraud:
		Edges: 8:16 Fixed point, Affine interpolation.
		Spans: 8:8 Fixed point, Affine interpolation.
		d/dX: per scanline
	Textures: 
		Supported Pixelformats: 32bit RGB color
		Scribbled blocks: unsupported
	32bit Texture/Gouraud rendering
	Inner loop:
		MMX assembly code.
	Mods:
		Transparency, Z-shading (fog).
	Code generation:
		MMX/SSE register emulation classes: in progress
	Performance analysis:
	GREETS:
	Runs about 30% slower than the old Texture/Gouraud assembly code.
	with Z-Buffering: Almost exactly the same speed, althrough without complete z-buffer
	support in all mappers this is somewhat a speculation.
	CITY:
	with Z-Buffering: Runs FASTER than equivalent Texture/Gouraud assembly code. Kickass.	

	- full engine front-end support in mipmapping should be added to measure performance gains.
	- new ngon frustrum clipper should be written along with a routine to break up polygons 
	  into pieces each with its own constant mipmapping level.

	(2): looks more or less like a piece of crap. and it's pretty slow, too. unless the
	alpha blending version gonna be da bomb, and a significantly higher % of the raindrops
	are to be presented, this is gonna be taken out of the demo

	(3): related coding logged within IX.ASM.

	TODO list.

	18.04.02
		Seperate omni's Flare size (Size track) from Intensity. FLD only has an Intensity
		track, Flare size should be either set manually or based on a new track read from
		LW3D.
	20.04.02
		general engineering upgrades to FDS - Viewports, renderer, naming conventions,
		and framework to allow hardware rendering. 3d object class, remove memory leaks.
		(impl: ongoing low priority)

		FlatPoly with efficient Z testing

		Fast (2 pixels per r/w) spriter

	24.04.02
		Try to predict if raindrops are out of frustrum for their entire lifespan and
		replace them until they have a viewable position. This effort should be made because
		less than 10% of the raindrop particles are visible at a time.

		Render rain particles using alpha blending

	03.07.02
		Use better methods of tile-based rendering to further improve the performance
		of v.high overdraw sprite rendering. 

    27.07.02
		try using a seperate doublepixel (320x240) surface to draw all flares on, and
		then use 'bilinear' addative x2 image enlargement to write all contents 
		to framebuffer. this means spriters are going to support variable zBuffer ptrs.

		SSE the transform pipeline.
		seperate clipping flags calc from xforming

    02.08.02
	    write nonaligned bounding boxes
		use pages AGP memory and DDraw's Blt

	02.07.03
		Lighting takes too much time (20%) on pentium 4, in CITY and GREETS scenes.

	10.01.04
		Add Configurator program; run configurator before demo.
		Add support for objects with multiple instances. This can be supported because we now employ a zBuffer.

	12.01.04
	  Add rendering pipeline for single object instances. FDS now supports meshes 
	  with multiple instances.

	+ more stuff
	 - omni 'size' spline should be called intensity, flare size should be a different parameter
	 - tmapper in assembly
	 
	+ stuff
	 - process mutex (only 1 running demo process at once)
	 - remove all writes to disk (runtime log, video unit log etc)
	 - fix frustrum clipper. [right/bottom]
	 - correct right/bottom edges in bilinear scaler
	 - initialization progress indicator
	 - sleep while window inactive (or not... music sync)
	+ 3D engine upgrades
	 + redesign/recode pipeline with strided arrays, early 3d-clipping (?), and z-buffer optimized sorting.
		- deemed ineffective, this can be mostly ignored
	 - Z-buffering, mipmapping, [tiled?] [palettized?] texture/gouraud single pass mapper based on SSE assembly

	+ demo visuals
	 - GFX improve for lightning and vortex
	 - light projection toward vortex in FOUNTAIN
	 - fountain spawned particles lose/reverse gravity and spin when vortex opens
	 - rain over CITY
	 - skycube over CITY
	 - volumetric (fire, metaballs, ?) in central room GREETS.
	 - shadowing in GREETS
	 - additional 'traffic' animations for CITY

******************************************************************************/
#define POINTER_64
#include "Rev.H"
#include "Fillertest.h"
#include "ImageCompression.h"
#include "PhotonTracer.h"
#include <float.h>
#include "Config.h"
//#include "FModWrapper.h"

// global configuration db
static ConfigurationDB *g_cfg;

// global status flags
dword g_playMusic;
dword g_profilerActive;
long g_demoXRes;
long g_demoYRes;
long g_fullScreenMode;
long g_displayType;


char *ModuleBuf;
dword g_RevModuleHandle;
//FModWrapper	module;
VESA_Surface *g_FlipRequestPage;

void Destroy_Scene(Scene *Sc)
{
	return;
	TriMesh *Tri;
	long total = 0,f;

//	printf("Shootin' scene...\n");
	for(Tri=Sc->TriMeshHead;Tri;Tri=Tri->Next)
	{
		if (Tri->VIndex>5||Tri->FIndex>5)
		{
			delete Tri->Verts;
			delete Tri->Faces;
			f = sizeof(Vertex)*Tri->VIndex+sizeof(Face)*Tri->FIndex;
			total += f;
//			printf("Freed %d Bytes from mesh killing\n",f);
		}
	}
	for(Material *Mat = MatLib;Mat;Mat=Mat->Next)
	{
		if (Mat->RelScene == Sc)
			if (Mat->Txtr&&Mat->Txtr->Data)
			{
				 freeAlignedBlock(Mat->Txtr->Data);
				 Mat->Txtr->Data = NULL;
				 total += 262144;
//				 printf("Freed 262144 Bytes from Texture killing\n");
			}
	}
//	printf("%d Frags\n",total);
	freeAlignedBlock(Sc);
}

#ifdef _C_WATCOM

void Initialize_System()
{
	LogFile = fopen("Runtime.LOG","wt");

	if (!FDS_Init(512,384,32))    // Initialize FDS
	{
		printf("Initializing failed! Crashing to OS\n");
		exit(1);
	}

	if (g_playMusic)
	{
		ModuleBuf = LoadARJ("Music\\Music.ARJ","Revival.xM");
		Initialize_Module(ModuleBuf);
	}
	Generate_RGBFlares();
	if (BPP==8) Materials_Remap_Aligned();
//  InitMouse();
	// all Hardwired Initializations complete
}

void Scan_MaterialLib()
{
	printf("Scanning MatLib\n");
	for(Material *M=MatLib;M;M=M->Next)
		printf("Material %s at Scene %xH\n",M->Name,(DWord)M->RelScene);
}
/*
void main(int argc,char *argv[])
{
	Initialize_System();

//	printf("MEM = %d\n",DPMI_Free_Memory());
//	Initialize_Credits();
//	printf("[Credits]MEM = %d\n",DPMI_Free_Memory());
//	printf("[City]MEM = %d\n",DPMI_Free_Memory());
//
//	printf("[Chase]MEM = %d\n",DPMI_Free_Memory());
//	printf("[Fountain]MEM = %d\n",DPMI_Free_Memory());

	setbuf(stdout,NULL);
	printf("Initializing, please wait...");
	SetGraphicsMode();

//	Initialize_City(); //9.3
	Initialize_Fountain(); //8.3
//	Run_Glato();
//	while (Timer<4500) continue;
	Timer = 1;
//	Run_City();

	Timer = 1;
	Run_Fountain();

	Initialize_Greets(); //5.6
	Timer = 1;
	Run_Greets();




//	Initialize_Crash();
	Timer = 1;

//	Run_Crash();
#if Play_Music
	ShutDown();
#endif
	FDS_End();

//	printf("MEM = %d\n",DPMI_Free_Memory());
}
*/
void SysSleep(dword ticks)
{
}

#endif


#ifdef _C_VC6
#include "GDI.h"
#include "DDRAW.h"
#include "D3D8.h"

FILE *Stub_Log;
DWORD Stub_Disable;

static MMRESULT TimerID = 0; //MM Timer Handle
static long Timer__NA = -1;
static volatile long *TimerPtr = &Timer;

static HINSTANCE G_hInst;
static HWND G_hWnd;
static char G_TitleStr[MAX_GSTRING] = "Revival / Flood";
static char G_WindowClassStr[MAX_GSTRING] = "Revival";
static char G_AppTitle[MAX_GSTRING];

static HACCEL hAccelTable;
static DWORD MID,STID;

static DWORD StubI = 0;

// cheap substitute for a semaphore...
static DWORD Thread_Interlock_Mutex = 0;

static dword S_InitRet;
static void *Signmem;
DisplayInitParams g_DIP;

void FlipRequest(VESA_Surface *VS)
{	
	Thread_Interlock_Mutex = 1;
	g_FlipRequestPage = VS;
	PostMessage(G_hWnd,WM_USER+3,0,0); //DEVICE INIT SIGNAL  
	while (Thread_Interlock_Mutex)
	{
		Sleep(1);
	}
}

// Timer code.
static void CALLBACK TimerProc(UINT wTimerID, UINT msg, 
    DWORD dwUser, DWORD dw1, DWORD dw2) 
{ 
    (*TimerPtr)++;
} 

int TimerInit(long Freq)
{
	Timer_Freq = Freq;
	DWORD msInterval = 1000/Timer_Freq; //millisecond accuracy.
	DWORD wTimerRes = 0;

	TimerID = timeSetEvent(msInterval,wTimerRes,TimerProc,NULL,TIME_PERIODIC );

	if (!TimerID)
	{
		return 0;
	}

	*TimerPtr = 0;
	return 1;
}

void TimerRemove(void)
{
	if (TimerID)
	{
		timeKillEvent(TimerID);
		TimerID = 0;
	}
} 
//Timer code ends.

static dword InitDisplayDevice()
{
	long rv = -1;
	char *displayAPI = g_cfg->extractString("DisplayAPI");
	if (!strcmp(displayAPI, "D3D8"))
	{
		// initialize Direct3D 8.0 based display
		g_displayType = 0;
		rv = D3D8_InitDisplay();
	} else if (!strcmp(displayAPI, "DDRAW"))
	{
		// initialize DDRAW based display.
		g_displayType = 1;
		rv = DDRAW_InitDisplay();
	} else if (!strcmp(displayAPI, "GDI"))
	{
		// initialize GDI based display
		g_displayType = 2;
		rv = GDI_InitDisplay();
	}
	delete [] displayAPI;
	return rv;
}



DWORD StubInit__(void)
{
	StubI = 1;


	//hAccelTable = LoadAccelerators(G_hInst, (LPCTSTR)IDC_W32STUB);

	SetForegroundWindow(G_hWnd);

	//SetTimer(G_hWnd,1,1000/TimerFreq,NULL);
	int TimerFreq = 100;
	TimerInit(TimerFreq);
	
	return 0;
}

/*void Run_RotationTest()
{
	Image Im;
	New_Image(&Im, 256, 256);
//	Complex C,R1,R2;
//	Complex_Form(&C, 0.5, 0.3);
//	Complex_Form(&R1, -2.0,-2.0);
//	Complex_Form(&R2,  2.0, 2.0);
//	Generate_Julia_Fractal(&Im, &C, &R1, &R2);
	long i,j;
	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
			Im.Data[i+j*256] = (  ((i>>2) ^ (j>>2)) <<3) * 0x010101;

	Image IRot;
	New_Image(&IRot, 640, 480);

	dword *vpage = (dword *)VPage;
	while (Timer < 3000)
	{
		// clear target.
		memset(IRot.Data, 0, 640*480*sizeof(dword));
		RotateImage(IRot,Im, 320, 240, (float)Timer*0.003, 1.0);

		dword *p = IRot.Data;
		for(j=0; j<480; j++)
			for(i=0; i<640; i++)
				vpage[i+j*XRes] = *p++;

		Flip(MainSurf);

		if (Keyboard[ScESC]) break;
	}
	while (Keyboard[ScESC]) continue;
	if (Timer > 3000)
		Timer -= 3000;
	else
		Timer = 0;
}*/

struct Raindrop
{
	Vector pos;
};

dword numDrops;
Raindrop *RD;
static float dTime;

inline void fist(long &r, float x)
{
	__asm 
	{
		FLD dword ptr [x]
		mov edi, dword ptr [r]
		FISTP dword ptr [edi]
	}
}

static void rainmapper(float x1, float y1, float x2, float y2)
{
	if (y1<0 || y2 >= YRes || x1<2 || x2<2 || x1 >= XRes-3 || x2 >= XRes-3) return;	

	float rdy = 1.0 / (y2-y1);
	float dxdy = (x2-x1) * rdy;

	long iy1,iy2, ix;
	fist(iy1, y1);
	fist(iy2, y2);
	float x = x1 + (iy1-y1) * dxdy;

	dword *scanline = (dword *)VPage;
	dword j = XRes;
	scanline += iy1 * j;

	float lt = (iy1-y1) * rdy;
	dword R, G, B, a, A, pixel;

	for(long y=iy1; y<iy2; y++)
	{
		fist(ix, x);

		if (lt < 0.7)
		{
//			R = lt * 0xCF / 0.8;
//			G = lt * 0xCF / 0.8;
//			B = lt * 0xFF / 0.8;
			A = lt * 0xFF / 0.7;
		} else {
//			R = (1.0-lt) * 0xCF / 0.2;
//			G = (1.0-lt) * 0xCF / 0.2;
//			B = (1.0-lt) * 0xFF / 0.2;
			A = (1.0-lt) * 0xFF / 0.3;
		}

		//dword P2 = ((R>>2)<<16) + ((G>>2)<<8) + (B>>2);
		//dword P1 = ((R>>1)<<16) + ((G>>1)<<8) + (B>>1);
		//dword P0 = ((R   )<<16) + ((G   )<<8) + (B);
		
		// modulate(scanline[ix-2], 0x7F7F7F, A, 255-A);
		
/*		for (int dx=-2; dx<=2; dx++)
		{
			pixel = scanline[ix+dx];

			R = (pixel>>16);
			G = ((pixel & 0xff00)>>8);
			B = (pixel & 0xff);

			a = A >> abs(dx);
			R = (R * (255 - a) + a * 0x7f) >> 8;
			G = (G * (255 - a) + a * 0x7f) >> 8;
			B = (B * (255 - a) + a * 0x7f) >> 8;
			scanline[ix+dx] = ((R   )<<16) + ((G   )<<8) + (B);
		}*/

		pixel = scanline[ix-2];

		R = ((pixel & 0xff0000)>>16);
		G = ((pixel & 0xff00)>>8);
		B = (pixel & 0xff);

		a = A >> 2;
		R = (R * (255 - a) + a * 0x7f) >> 8;
		G = (G * (255 - a) + a * 0x8f) >> 8;
		B = (B * (255 - a) + a * 0xaf) >> 8;
		scanline[ix-2] = ((R   )<<16) + ((G   )<<8) + (B);

		pixel = scanline[ix-1];

		R = ((pixel & 0xff0000)>>16);
		G = ((pixel & 0xff00)>>8);
		B = (pixel & 0xff);

		a = A*3 >> 2;
		R = (R * (255 - a) + a * 0x7f) >> 8;
		G = (G * (255 - a) + a * 0x8f) >> 8;
		B = (B * (255 - a) + a * 0xaf) >> 8;
		scanline[ix-1] = ((R   )<<16) + ((G   )<<8) + (B);

		pixel = scanline[ix];

		R = ((pixel & 0xff0000)>>16);
		G = ((pixel & 0xff00)>>8);
		B = (pixel & 0xff);

		a = A;
		R = (R * (255 - a) + a * 0x7f) >> 8;
		G = (G * (255 - a) + a * 0x8f) >> 8;
		B = (B * (255 - a) + a * 0xaf) >> 8;
		scanline[ix] = ((R   )<<16) + ((G   )<<8) + (B);

		pixel = scanline[ix+1];

		R = ((pixel & 0xff0000)>>16);
		G = ((pixel & 0xff00)>>8);
		B = (pixel & 0xff);

		a = A*3 >> 2;
		R = (R * (255 - a) + a * 0x7f) >> 8;
		G = (G * (255 - a) + a * 0x8f) >> 8;
		B = (B * (255 - a) + a * 0xaf) >> 8;
		scanline[ix+1] = ((R   )<<16) + ((G   )<<8) + (B);

		pixel = scanline[ix+2];

		R = ((pixel & 0xff0000)>>16);
		G = ((pixel & 0xff00)>>8);
		B = (pixel & 0xff);

		a = A >> 2;
		R = (R * (255 - a) + a * 0x7f) >> 8;
		G = (G * (255 - a) + a * 0x8f) >> 8;
		B = (B * (255 - a) + a * 0xaf) >> 8;
		scanline[ix+2] = ((R   )<<16) + ((G   )<<8) + (B);

		//scanline[ix-2] = 0x7f7f7f;
		//scanline[ix-1] = 0x7f7f7f;
		//scanline[ix  ] = 0x7f7f7f;
		//scanline[ix+1] = 0x7f7f7f;
		//scanline[ix+2] = 0x7f7f7f;
		
		lt += rdy;
		x += dxdy;
		scanline += j;
	}
}

static void initrain()
{
	numDrops = 5000;
	RD = new Raindrop [numDrops];

	for(dword i=0; i<numDrops; i++)
	{
		RD[i].pos.x = ((rand()-16384) * 10.0) / 16384.0;
		RD[i].pos.y = ((rand()-16384) * 10.0) / 16384.0;
		RD[i].pos.z = ((rand()-16834) * 10.0) / 16384.0;
	}
}

static void calc_DomDirection(Vector &RainDir, Vector &DomDir)
{
	float dom = 0.0;
	if (RainDir.x > dom)
	{
		dom = RainDir.x;
		DomDir.x = 1.0;
		DomDir.y = 0.0;
		DomDir.z = 0.0;
	}
	if (-RainDir.x > dom)
	{
		dom =-RainDir.x;
		DomDir.x =-1.0;
		DomDir.y = 0.0;
		DomDir.z = 0.0;
	}
	if (RainDir.y > dom)
	{
		dom = RainDir.y;
		DomDir.x = 0.0;
		DomDir.y = 1.0;
		DomDir.z = 0.0;
	}
	if (-RainDir.y > dom)
	{
		dom =-RainDir.y;
		DomDir.x = 0.0;
		DomDir.y =-1.0;
		DomDir.z = 0.0;
	}
	if (RainDir.z > dom)
	{
		dom = RainDir.z;
		DomDir.x = 0.0;
		DomDir.y = 0.0;
		DomDir.z = 1.0;
	}
	if (-RainDir.z > dom)
	{
		dom =-RainDir.z;
		DomDir.x = 0.0;
		DomDir.y = 0.0;
		DomDir.z =-1.0;
	}
}

static void rain(Image &Im)
{
	float cx = Im.x * 0.5;
	float cy = Im.y * 0.5;

	float kx = Im.x * 0.8;
	float ky = Im.x * 0.8;

	static float alpha = PI*0.9, beta = 0.0;

	if (Keyboard[ScLeft]) alpha -= 0.01;
	if (Keyboard[ScRight]) alpha += 0.01;
	if (Keyboard[ScDown]) beta -= 0.01;
	if (Keyboard[ScUp]) beta += 0.01;

	Vector RainDir;
	RainDir.x = sin(alpha) * cos(beta);
	RainDir.y = cos(alpha) * cos(beta);
	RainDir.z = sin(beta);	

	Vector DomDir;
	calc_DomDirection(RainDir, DomDir);

	Vector delta;
	Vector_Form(&delta, 
		RainDir.x * dTime * 10.0, 
		RainDir.y * dTime * 10.0, 
		RainDir.z * dTime * 10.0);

	Vector rainTrail;
	Vector_Scale(&RainDir, 0.5, &rainTrail);

	for(dword i=0; i<numDrops; i++)
	{
		RD[i].pos.x += delta.x;
		RD[i].pos.y += delta.y;
		RD[i].pos.z += delta.z;

		if (Dot_Product(&RD[i].pos, &DomDir) > 10.0)
		{
			RD[i].pos.x = ((rand()-16384) * 10.0) / 16384.0;
			RD[i].pos.y = ((rand()-16384) * 10.0) / 16384.0;
			RD[i].pos.z = ((rand()-16834) * 10.0) / 16384.0;

			// project on <x,DomDir> = -10.0
			// x := x + a*RainDir.
			// <x+a*RainDir, DomDir> = -10.0.
			// <x,DomDir> + a*<RainDir, DomDir> = -10.0
			// a = - (10.0 + <x, DomDir>) / <RainDir, DomDir>.
			float a = - (10.0 + Dot_Product(&RD[i].pos, &DomDir)) / Dot_Product(&RainDir, &DomDir);
			RD[i].pos.x += a*RainDir.x;
			RD[i].pos.y += a*RainDir.y;
			RD[i].pos.z += a*RainDir.z;
		}

		float ahead_z = RD[i].pos.z + rainTrail.z;

		if (RD[i].pos.z < 1.0 || ahead_z < 1.0) continue;

		float rz = 1.0 / RD[i].pos.z;

		float x = cx + kx * RD[i].pos.x * rz;
		float y = cy - ky * RD[i].pos.y * rz;

		float raz = 1.0 / ahead_z;
		float ahead_x = cx + kx * (RD[i].pos.x + rainTrail.x) * raz;
		float ahead_y = cy - ky * (RD[i].pos.y + rainTrail.y) * raz;

		// fat map
		if (ahead_y > y)			
			rainmapper(x, y, ahead_x, ahead_y);
		else
			rainmapper(ahead_x, ahead_y, x, y);

		// pixel rendering
//		int ix = x;
//		int iy = y;

//		if (ix>=0 && ix<Im.x && iy>=0 && iy<Im.y)
//			((dword *)Im.Data)[ix + iy * Im.x] = 0xFFFFFF;
	}
}


static Vector 
	TexOrigin(-1.0, 1.5, 6.0), 
	TexU(3.0, 0.0,-1.0), 
	TexV(-1.0,-3.0, 0.0);

static Vector TexNorm;
static float TexOffs;
static float TexDxy;
static float MaxVolDepth = 4.0f;

void VectorGetUV(Vector *V, float &u, float &v)
{
	Vector P;
	Vector_LComb(V, &TexNorm, 1.0, -Dot_Product(V,&TexNorm), &P);

	float a, b;

	u = (P.x * TexV.y - P.y * TexV.x) * TexDxy;
	v = (TexU.x * P.y - TexU.y * P.x) * TexDxy;
}

void PointGetUV(Vector *V, float &u, float &v)
{
	Vector P;
	Vector_Sub(V, &TexOrigin, &P);
	VectorGetUV(&P, u, v);
}

dword TexGetColor(float u, float v)
{
	long U = u*16.0;
	long V = v*16.0;
	long U2 = u*256.0;
	long V2 = v*256.0;
	return ((U^V)&1)*0xFF0000 + (U2<<8) + V2;
}

void VolTrace(Image &Im)
{

	mword i, j;
	float x, y;
	float dx = 2.0 / (float)Im.x, dy = 2.0 / (float)Im.y;

	Cross_Product(&TexU, &TexV, &TexNorm);
	Vector_Norm(&TexNorm);

	TexOffs = Dot_Product(&TexOrigin, &TexNorm);

	TexDxy = 1.0 / (TexU.x * TexV.y - TexU.y * TexV.x);

	// initial height over texture plane
	float hOrig =-TexOffs;

	float UOrig, VOrig;
	float UDir, VDir;

	// calculate UOrig, VOrig
	Vector zero(0,0,0);
	PointGetUV(&zero, UOrig, VOrig);


	for(j=0,y=1.0; j<Im.y; j++, y-=dy)
	{
		for(i=0,x=-1.0; i<Im.x; i++, x+=dx)
		{
			Vector V;
			V.x = x;
			V.y = y;
			V.z = 1.0;
			Vector_Norm(&V);

			// (du, dv) along ray
			float rdu, rdv;
			VectorGetUV(&V, rdu, rdv);

			// delta-height over plane along ray
			float rdh = Dot_Product(&V, &TexNorm);			

			float scanres = 1.0/(256*sqrt(rdu*rdu + rdv*rdv)); // scanning resolution
			if (scanres > 1.0/64.0) scanres = 1.0/64.0;

			// interval to clip
			float t, t0, t1;
			t0 = 0.0;
			t1 = 1E+05;

			// vol. Clipping
			float i_rdh = 1.0/rdh;
			if (rdh<0)
			{			
				t = -hOrig*i_rdh;
				if (t1>t) t1 = t;

				t = (MaxVolDepth-hOrig)*i_rdh;
				if (t0<t) t0 = t;
			} else {
				t = (MaxVolDepth-hOrig)*i_rdh;
				if (t1>t) t1 = t;

				// initial height cannot be negative
//				t = -hOrig*i_rdh;
//				if (t0<t) t0 = t;
			}
			if (t0>=t1) continue;
			
			if (y<0)
			{
				int banana = 1;
			}
			float i_rdu = 1.0/rdu;
			if (rdu<0)
			{			
				t = -UOrig*i_rdu;
				if (t1>t) t1 = t;

				t = (1.0-UOrig)*i_rdu;
				if (t0<t) t0 = t;
			} else {
				t = (1.0-UOrig)*i_rdu;
				if (t1>t) t1 = t;

				t = -UOrig*i_rdu;
				if (t0<t) t0 = t;
			}
			if (t0>=t1) continue;
			float i_rdv = 1.0/rdv;
			if (rdv<0)
			{			
				t = -VOrig*i_rdv;
				if (t1>t) t1 = t;

				t = (1.0-VOrig)*i_rdv;
				if (t0<t) t0 = t;
			} else {
				t = (1.0-VOrig)*i_rdv;
				if (t1>t) t1 = t;

				t = -VOrig*i_rdv;
				if (t0<t) t0 = t;
			}
			if (t0>=t1) continue;
			
			float u = UOrig + t0 * rdu;
			float v = VOrig + t0 * rdv;
			float h = hOrig + t0 * rdh;

			float tex_r, tex_g, tex_b;
			float res_r = 0.0, res_g = 0.0, res_b = 0.0;
			dword TexColor;

			/*if (fabs(rdu) > fabs(rdv))
			{
				if (rdu>0.0)
				{					
				} else {
				}
			} else {
				if (rdv>0.0)
				{
				} else {

				}
			}*/
			for(t=t0; t<t1; t+=scanres)
			{
				TexColor = TexGetColor(u, v);

				tex_r = ((TexColor&0xFF0000)>>16);
				tex_g = ((TexColor&0x00FF00)>>8);
				tex_b = (TexColor&0xFF);

				float lumin = 1.0*exp(-h*0.3);
				if (lumin > 1.0) lumin = 1.0;

				float ls = lumin * scanres;
				res_r += lumin * tex_r * ls;
				res_g += lumin * tex_g * ls;
				res_b += lumin * tex_b * ls;

				u += rdu * scanres;
				v += rdv * scanres;
				h += rdh * scanres;
			}
			long res_R = res_r; if (res_R>255) res_R = 255;
			long res_G = res_g; if (res_G>255) res_G = 255;
			long res_B = res_b; if (res_B>255) res_B = 255;
			Im.Data[i+j*Im.x] = (res_R<<16)+(res_G<<8)+res_B;
		}
	}
}

void Run_RainFX()
{
	const long PartTime = 10000;

	Image Im;
	Im.x = XRes;
	Im.y = YRes;
	Im.Data = (dword *)VPage;

	initrain();

	// motion blur trash
	char *BPage = new char[PageSize];
	VESA_Surface Blur;
	memcpy(&Blur,MainSurf,sizeof(VESA_Surface));
	Blur.Data = BPage;
	Blur.Flags = VSurf_Noalloc;
	Blur.Targ = VPage;


	float TT = Timer;
	while (Timer < PartTime)
	{
		dTime = (Timer - TT) / 100.0;
		TT = Timer;

		memset(VPage, 0, PageSize);
		
		// draw scene
		//Generate_Plasma(&Im, 10, 3621);
		
//		VolTrace(Im);
		rain(Im);
//		int fpcw = _control87(0, 0);
//		FILE *F = fopen("fpu.log", "at");
//		fprintf(F, "[exec] control word = %xh\n" , fpcw);
//		fclose(F);

		Flip(MainSurf);
//		Modulate(MainSurf,&Blur,0x707070,0x808080);
//		Flip(&Blur);

		//while (Timer < TT + 50) continue;
		if (Keyboard[ScESC])
		{
			Timer = PartTime;
			break;
		}
	} Timer -= PartTime;

	delete BPage;
}


void CodeEntry(void *var)
{
	ConfigurationDB cfg;
	g_cfg = &cfg;
	if (0)
	{
		// Write ConfigurationDB.
		cfg.setCategory("Primary");	
		cfg.addEntry(new CFGInteger("MusicEnable", 1));
		cfg.addEntry(new CFGInteger("ResolutionX", 640));
		cfg.addEntry(new CFGInteger("ResolutionY", 480));
		cfg.addEntry(new CFGInteger("FullScreenMode", 1));
		cfg.addEntry(new CFGInteger("ProfilerEnable", 1));
		cfg.toFile("rev.cfg");		
	} else
	{
		// Read ConfigurationDB.
		cfg.fromFile("rev.cfg");
	}
	
	g_playMusic = cfg.extractInteger("MusicEnable");
	g_profilerActive = cfg.extractInteger("ProfilerEnable");
	g_demoXRes = cfg.extractInteger("ResolutionX");	
	g_demoYRes = cfg.extractInteger("ResolutionY");

	int yadda = 0;

	int internalXRes = g_demoXRes;
	int internalYRes = g_demoYRes;
	LogFile = fopen("Runtime.LOG","wt");

	if (!FDS_Init(internalXRes, internalYRes,32))    // Initialize FDS
	{
		return;
	}
	Generate_RGBFlares(); // should be in fds_init
	
	if (g_playMusic)
		FModInit();
	InitFillers (VirtualProtect, PAGE_EXECUTE_READWRITE);

	// device init parameters
	g_DIP.hInst = G_hInst;
	g_DIP.hWnd = G_hWnd;
	g_DIP.x = internalXRes;
	g_DIP.y = internalYRes;
	g_DIP.bpp = 32;
	g_DIP.flags = 0;

	g_fullScreenMode = cfg.extractInteger("FullScreenMode");
	if (g_fullScreenMode)
		g_DIP.flags |= DISP_FULLSCREEN;

	// request display initialization
	Thread_Interlock_Mutex = 1;
	PostMessage(G_hWnd,WM_USER+1,0,0); //DEVICE INIT SIGNAL  
	while (Thread_Interlock_Mutex)
	{		
		Sleep(10);
	}
	// direct display initialization
//	InitDisplayDevice(cfg);
	InitPolyStats(200);

	dword sh = 0;
	if (g_playMusic)
	{
		sh = FModLoadModule("Revival.XM");
		//module.Load("Revival.xM");
	}
	g_RevModuleHandle = sh;

	Initialize_Glato(); //9.3
	Initialize_City(); //9.3
	Initialize_Chase();
	Initialize_Fountain(); //8.3
	Initialize_Crash();
	Initialize_Greets(); //5.6
	//Initialize_Nova();
	
//	Initialize_Koch();
//	ImageCompressionTestCode();
//	TestPhotonTracer();

	if (g_playMusic)
	{
		if (sh) FModPlayModule(sh);
		//module.Start();
	}
	Timer = 0;
//	FillerTest();
//	return;

	Run_Glato();
	while (Timer<4000);
	Timer = 0;
	if (Timer<=0) Timer = 1;
	Run_City();
//	SavePolyStats("City.stat");

	if (Timer <= 0) Timer = 1;
	Run_Chase();

	if (Timer<=0) Timer = 1;
	Run_Fountain();

//	SavePolyStats("Fountain.stat");
	if (Timer <= 0) Timer = 1;
	Timer = 1;
	Run_Crash();

	if (Timer<=0) Timer = 1;
	Run_Greets();
//	SavePolyStats("Greets.stat");
//	Run_RainFX();
//	Run_Koch();
//	Run_Nova();

	if (g_playMusic)
	{
		if (sh) FModFreeModule(sh);
		FModClose();
		//module.Stop();
	}
}

static dword RemoveDisplayDevice(void)
{
	switch (g_displayType)
	{
	case 0:
		return D3D8_RemoveDisplay();
	case 1:
		return DDRAW_RemoveDisplay();
	case 2:
		// TODO: GDI_RemoveDisplay();
		break;
	}
	return 0;
}

void StubRemove()
{
	if (StubI)
	{
		//RemoveDisplayDevice();
		TimerRemove();
		StubI = 0;
		free(Signmem);
	}

}


static void StubbedThread(void *var)
{
	STID = GetCurrentThreadId();

	StubInit__();

	// randomize
//	SYSTEMTIME st;
//	GetSystemTime(&st);
//	srand(st.wMilliseconds+1000*st.wSecond);

	CodeEntry(var); // if this was a real STUB, it should have
	//supposedly read an OBJ file where this CodeEntry is located.
	//a DOS STUB would do the same jumping from main() into that CodeEntry function.

	//PostThreadMessage(MID,WM_QUIT,0,0);
	PostMessage(G_hWnd,WM_USER+2,0,0); //TERMINATION SIGNAL

	_endthread();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	dword scanCode;
	if (StubI)
	{
		switch (message)
		{
			// Keyboard Array may require conversions to my own standard.
			// (parhaps to the std. scancodes. messing with the interrupt 9h code later on to refit wont be fun.)
			case WM_ACTIVATE:
				WORD fActive,fMinimized;
				fActive = LOWORD(wParam);           // activation flag 
				fMinimized = (BOOL) HIWORD(wParam); // minimized flag 
				Stub_Disable = (fActive==WA_INACTIVE||fMinimized);
			break;
			case WM_KEYDOWN:
				scanCode = MapVirtualKey(wParam, 0);
				//fprintf(Stub_Log,"<%d> Keyboard Key [%d] down\n",*TimerPtr,wParam);
				Keyboard[scanCode] = 1;
			break;
			case WM_KEYUP:
				scanCode = MapVirtualKey(wParam, 0);
				//fprintf(Stub_Log,"<%d> Keyboard Key [%d] up\n",*TimerPtr,wParam);
				Keyboard[scanCode] = 0;
			break;
/*			case WM_MOUSEMOVE:
				CMS->Buttons = 0;
				StubProcessMouseSignal(wParam);

				CMS->X = LOWORD(lParam);  // horizontal position of cursor 
				CMS->Y = HIWORD(lParam);  // vertical position of cursor  
			break;
			case WM_LBUTTONDOWN:
				CMS->Buttons = MouseLDC;
				StubProcessMouseSignal(wParam);

				CMS->SX = LOWORD(lParam);  // horizontal position of cursor 
				CMS->SY = HIWORD(lParam);  // vertical position of cursor  
			break;

			case WM_LBUTTONDBLCLK:
				CMS->Buttons = MouseLDC;
				StubProcessMouseSignal(wParam);

				CMS->SX = LOWORD(lParam);  // horizontal position of cursor 
				CMS->SY = HIWORD(lParam);  // vertical position of cursor  
			break;*/
		}
	}
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
        case WM_SETCURSOR:
			// kill fullscreen cursor, no matter the cost!
			if (g_fullScreenMode)
				while (ShowCursor(FALSE)>=0);
        break;
		case WM_TIMER:
			//(*TimerPtr)++;
		break;
/*
		case WM_USER:
			fprintf(Stub_Log,"<%d> Initialization Signal\n",*TimerPtr);
			S_InitRet = StubInit__(InStS);
			Thread_Interlock_Mutex = 0;
		break;*/

		case WM_USER+1:
			//fprintf(Stub_Log,"<%d> Device Initialization Signal\n",*TimerPtr);
			S_InitRet = InitDisplayDevice();
			StubI = 2;
			Thread_Interlock_Mutex = 0;
		break;
		// Page flip signal
		case WM_USER+3:
			Flip(g_FlipRequestPage);
			Thread_Interlock_Mutex = 0;
		break;

		case WM_USER+2:
			//fprintf(Stub_Log,"<%d> Termination Signal\n",*TimerPtr);
			StubRemove();
			// Try to allow "recovery time" from low res fullscreen.
			//SetWindowLong( G_hWnd, GWL_STYLE, WS_OVERLAPPED );
			//Sleep(1000);
			DestroyWindow(G_hWnd);			
//			PostMessage(G_hWnd,WM_DESTROY,0,0);
			//PostQuitMessage(0);
		break;

		default:
			//fprintf(Stub_Log,"<%d> Unknown Signal\n",*TimerPtr);
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MID = GetCurrentThreadId();
	
	MSG msg;
	G_hInst = hInstance;
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= G_hInst;
	wcex.hIcon			= NULL;//LoadIcon(G_hInst, (LPCTSTR)IDI_W32STUB);
	wcex.hCursor		= LoadCursor(NULL, IDC_HELP);
	wcex.hbrBackground	= (HBRUSH)(COLOR_DESKTOP+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= G_WindowClassStr;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	if (!RegisterClassEx(&wcex)) return 0;

//	HACCEL hac;
//	hac = CreateAcceleratorTable(NULL,0);
//	G_hWnd = CreateWindowEx(WS_EX_APPWINDOW,G_WindowClassStr,G_TitleStr,WS_OVERLAPPEDWINDOW,0,0,0,0, NULL, NULL, G_hInst, NULL);
    G_hWnd = CreateWindowEx(WS_EX_TOPMOST,
                          G_WindowClassStr,
                          G_TitleStr,
                          WS_POPUP,
                          0,
                          0,
                          0,//GetSystemMetrics(SM_CXSCREEN),
                          0,//GetSystemMetrics(SM_CYSCREEN),
                          NULL,
                          NULL,
                          G_hInst,
                          NULL);

    ShowWindow(G_hWnd, nCmdShow);
    UpdateWindow(G_hWnd);
    SetFocus(G_hWnd);

	// Ow shit
	// STUB must initialize all video,raster and render devices.
	//
	_beginthread(StubbedThread,64*1024,NULL);
 
	// do the stuff
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return 0;
}

void SysSleep(dword ticks)
{
	Sleep(ticks);
}

#endif