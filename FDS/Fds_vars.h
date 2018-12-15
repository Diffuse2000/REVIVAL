#ifndef FDS_VARS_H_INCLUDED
#define FDS_VARS_H_INCLUDED
#include "FDS_DEFS.H"
// FDS - Variables and structures
typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned long DWord;
typedef unsigned char byte;
typedef unsigned short word;
typedef   signed short sword;
typedef unsigned long dword;
typedef   signed long sdword;
typedef unsigned long mword;
typedef   signed long smword;

#ifdef _C_VC6
typedef unsigned __int64 uint64;
typedef   signed __int64  int64;
#endif

// _data must be allocated 
class mmreg
{
public:
	union
	{
		byte _d8[8];
		word _d16[4];
		sword _sd16[4];
		dword _d32[2];
		uint64 _d64;
	};

	static void clearstate();

	// arithmetic operations
	// x.op(r) means x += r
	// x.op(r1,r2) means x = r1+r2


	// addition
	inline void addb(mmreg &r);
	inline void addb(mmreg &r1, mmreg &r2);	
	inline void addw(mmreg &r);
	inline void addw(mmreg &r1, mmreg &r2);
	inline void addd(mmreg &r);
	inline void addd(mmreg &r1, mmreg &r2);
	inline void addq(mmreg &r);
	inline void addq(mmreg &r1, mmreg &r2);

	// subtraction
	inline void subb(mmreg &r);
	inline void subb(mmreg &r1, mmreg &r2);	
	inline void subw(mmreg &r);
	inline void subw(mmreg &r1, mmreg &r2);
	inline void subd(mmreg &r);
	inline void subd(mmreg &r1, mmreg &r2);
	inline void subq(mmreg &r);
	inline void subq(mmreg &r1, mmreg &r2);

	// cxx commands are used for convinient packing/unpacking
	inline void cbw();
	inline void cwb();
	inline void packbw();
	inline void packwb();

	// multiplication
	inline void mulh(mmreg &r1);
	inline void mulh(mmreg &r1, mmreg &r2);
	inline void shrb(uint64 c);
	inline void shrw(uint64 c);
	inline void shrd(uint64 c);
	inline void shrq(uint64 c);
	inline void sarb(uint64 c);
	inline void sarw(uint64 c);
	inline void sard(uint64 c);
	inline void sarq(uint64 c);
	inline void shlb(uint64 c);
	inline void shlw(uint64 c);
	inline void shld(uint64 c);
	inline void shlq(uint64 c);
};
// implementation using inline functions
#include "MISC/mmreg.inl"

// [12 Bytes]
typedef struct
{
	float B, G, R, A;
} Color;

#pragma pack(1)
// [4 Bytes]
union QColor
{
	dword BGRA;
	struct
	{
		byte B,G,R,A; //Red,Green,Blue,Alpha.
	};
};

// [1 KB]
typedef struct
{
	QColor C[256];
} Palette;

// Math variables and structures
// [4 Bytes]
struct Four_Chars
{
	unsigned char A,B,C,D;
};
// [4 Bytes]
union Radix256
{
	unsigned long DW; //the whole DWord
	Four_Chars CH; // A = Highest byte,D = Lowest Byte.
};

// [4 Bytes]
union FDW
{
	float F;
	DWord DW;
};

// [12 Bytes]
struct Vector
{
	float x, y, z;

	Vector() {}
	Vector(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}
	~Vector() {}

/////////////////////////////////////////////////////////
	inline static void add(Vector &u, Vector &v, Vector &w)
	{
		w.x = u.x + v.x;
		w.y = u.y + v.y;
		w.z = u.z + v.z;
	}
	
	// this = u + v;
	inline void add(Vector &u, Vector &v)
	{
		x = u.x + v.x;
		y = u.y + v.y;
		z = u.z + v.z;
	}

	inline Vector operator + (const Vector &u) const
	{
		return Vector(x + u.x, y + u.y, z + u.z);
	}

	inline Vector &operator += (const Vector &u)
	{
		x += u.x;
		y += u.y;
		z += u.z;
		return *this;
	}

/////////////////////////////////////////////////////////
	inline static void sub(Vector &u, Vector &v, Vector &w)
	{
		w.x = u.x - v.x;
		w.y = u.y - v.y;
		w.z = u.z - v.z;
	}
	
	// this = u - v;
	inline void sub(Vector &u, Vector &v)
	{
		x = u.x - v.x;
		y = u.y - v.y;
		z = u.z - v.z;
	}

	inline Vector operator - (const Vector &u) const
	{
		return Vector(x - u.x, y - u.y, z - u.z);
	}

	inline Vector &operator -= (const Vector &u)
	{
		x -= u.x;
		y -= u.y;
		z -= u.z;
		return *this;
	}

/////////////////////////////////////////////////////////
	inline static void cross(Vector &u, Vector &v, Vector &w)
	{
		w.x = u.y * v.z - u.z * v.y;
		w.y = u.z * v.x - u.x * v.z;
		w.z = u.x * v.y - u.y * v.x;
	}

	inline void cross(Vector &u, Vector &v)
	{
		x = u.y * v.z - u.z * v.y;
		y = u.z * v.x - u.x * v.z;
		z = u.x * v.y - u.y * v.x;
	}

	inline Vector operator ^ (const Vector &v) const
	{

		return Vector(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

	inline Vector &operator ^= (const Vector &v)
	{
		Vector	t(x, y, z);
		x = t.y * v.z - t.z * v.y;
		y = t.z * v.x - t.x * v.z;
		z = t.x * v.y - t.y * v.x;
		return *this;
	}
/////////////////////////////////////////////////////////	
	inline static float dot(Vector &u, Vector &v) 
	{
		return (u.x * v.x + u.y * v.y + u.z * v.z);
	}

	inline float operator * (const Vector &u) const
	{
		return (u.x * x + u.y * y + u.z * z);
	}


/////////////////////////////////////////////////////////
	Vector &operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector &operator /= (float s)
	{
		float r = 1/s;
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}
	Vector operator * (float s) const
	{
		return Vector(x*s, y*s, z*s);
	}

	Vector operator / (float s) const
	{
		float r = 1/s;
		return Vector(x*r, y*r, z*r);
	}

	friend Vector operator * (float s, Vector &v);
};

inline Vector operator * (float s, Vector &v)
{
	return Vector(v.x * s, v.y * s, v.z * s);
}

// [36 Bytes]
typedef float Matrix[3][3];
typedef float Matrix4[4][4]; //stuff

// [8 Bytes]
struct Complex
{
	float x, y;
};

// [16 Bytes]
// 08.04.02 - added support for [s, v] notation.
struct Quaternion
{
	union
	{
		float W;
		float s;
	};

	float x, y, z;

	Vector im()
	{
		return Vector(x, y, z);
	}
};

// [16 Bytes]
struct Plane
{
	Vector N;
	float O;
};

struct Line
{
	Vector O,D;
};

// [88 Bytes]
struct SplineKey
{
	float Frame;                   // Key Frame Number
	
	Quaternion Pos;                // Normative Quaternion Rotation value
	Quaternion AA;                 // Angle-Axis Quaternion Rotation value
	
	// 5 Spline Attributive keys.
	float  Tens,                   // Key tension value
		Cont,                   // Key continuity value
		Bias,                   // Key bias value
		EaseTo,                 // Key ease to value
		EaseFrom;               // Key ease from value
	
	Quaternion DS, DD;             // Key Quaternionic Derivatives
};

// [16 Bytes]
// Total: [16 Bytes+88 Bytes per Key]
struct Spline
{
	DWord NumKeys,CurKey;
	SplineKey *Keys;
	DWord Flags;
};

struct Vector4
{
	float x, y, z, T;
};

extern float SinTab[1440];
extern float CosTab[1440];
extern Vector ZeroVector;
extern Matrix Mat_ID;

// Scene variables and structures
// [16 Bytes]
// Total: [16 Bytes+64K per Charperpixel,+1Kb if Palettized]
typedef struct
{
	Palette *Pal;   // Texture's Local Palette. Relevant only for 8BPP Textures
	char *Data;     // Texture's Storage space.
	DWord BPP;      // Texture's BPP. Adjusts format in memory.
	// 0 used when Texture is invalid.
	// DWord size for Aligning, Should be treated as Byte.
	char *FileName; // Texture's Originating File-name.
	
	// OptClass - Will be used by Assign_Fillers to determine Raster device
	DWord OptClass; // 0 = 256x256 ; 1 = square 2^n ; 2 = arbitrary size.
	long SizeX,SizeY;
	long LSizeX, LSizeY; // log2(SizeX), log2(SizeY).	

	// Mipmapping/blocktiling support.
	long blockSizeX, blockSizeY; // zero to disable, 1<<blockSize measures length in pixels of each block
	char *Mipmap[16]; // pointers to up to 16 levels of mipmaps
	dword numMipmaps; // number of mipmap levels.

	dword ID; 
	dword Flags;
} Texture;

typedef struct
{
	DWord         * STex;  // Static Texture Map (64x64)
	DWord         * LMap;  // Light Map (16x16)
	DWord         * DTex;  // Dynamic Texture Map (64x64)
	DWord						TCacheEntry; //last frame surface was inserted to cache
	long            TCacheOffset; //-1 if surface is cached ; Cache pointer index otherwise.
} Surface;


// Digital Image (32bit) with variable size.
typedef struct
{
	long x, y;
	DWord *Data;
	char *FileName;
} Image;

typedef struct
{
	long U,V;   // 24:8 Fixed point mapping coordinates.
//	long RGB;   // Lighting/Spectral components.	
//	long PARAGRAPH_ALIGN;
	long R,G,B;
} GridPoint;

typedef struct
{
	long U,V;   // 24:8 Fixed point mapping coordinates.
	union
	{
		__int64 RGBA;   // Lighting/Spectral components.
		struct
		{
			word R, G, B, A;
		};
	};
//	long PARAGRAPH_ALIGN;
//	long R,G,B;
} NewGridPoint;

struct GridPointTG
{
	long u, v;	
	mmreg BGRA;
};

struct GridPointT
{
	long u, v;	
};

typedef struct
{
	float x ,y;  // Position on screen
	long RGB;   // Lighting/Spectral components
	long PARAGRAPH_ALIGN;
} AGridPoint;

typedef struct
{
	int x, y;
	float z, U, V;
} Bi_Triangle;

typedef struct
{
	int x, y;
	float z, U, V;
	float R,G,B;
} Bi_GTriangle;

typedef struct
{
	float x, y;
	float z,U,V;
} Bi_Floating;

// [32 Bytes]
// Total: [48 Bytes+64K per Charperpixel,+1Kb if Palettized]

struct Scene; // Temporary declaration for use with the Material structure.

struct Material
{
	// 16 byte alignment variables
	Color                 BaseCol; //Base Color for the material

	Scene               * RelScene; //Related Scene
	Texture             * Txtr;
	DWord                 Flags;
	DWord                 TFlags; //Texture Flags

	dword				  ID;
	
	float                 Luminosity;
	float                 Diffuse; //Diffuse reflection
	float                 Specular; //Specular reflection
	float                 Reflection; //Rebounded light Reflection
	float                 Transparency; //Transparency ratio
	unsigned short        Glossiness; //unknown parameter
	unsigned short        ReflectionMode; //unknown parameter
	char                * ReflectionImage; //Reflection detail
	float                 ReflectionSeamAngle; //unknown
	float                 RefractiveIndex; //Optic Factor
	float                 EdgeTransparency; //unknown factor
	float                 MaxSmoothingAngle; //?
	char                * ColorTexture; //Color Map Textutre Filename
	char                * DiffuseTexture; //Diffuse Map Texture Filename
	char                * SpecularTexture; //Specular Map Texture Filename
	char                * ReflectionTexture; //Reflection Map Texture Filename
	char                * TransparencyTexture; //Transparency Map Texture Filename
	char                * BumpTexture; //Bump Map Texture Filename
	char                * TextureImage; //Texture Map Filename
	unsigned short        TextureFlags; //Should be replaced by dword TFlags
	Vector                TextureSize; //Texture Proj. Size in spatial coordinates
	Vector                TextureCenter; //Texture Origin in space
	Vector                TextureFallOff; //??
	Vector                TextureVelocity; //U/V animation speed
	char                * TextureAlpha; //Texture Map for Alpha Channel
	unsigned short        NoiseFrequencies; //??
	unsigned short        TextureWrapX; //Wrap flag for U
	unsigned short        TextureWrapY; //Wrap flag for V
	float                 AAStrength; //Antialiasing Strength
	float                 Opacity; //?? gotta learn some physics
	Texture							* EnvTexture;
	float									TFP0;
	float									TFP1;
	Material            * Next,
		* Prev;
	char                * Name;
};

// 72 bytes mod 16 = 8
// 4+20+12+12+12+8+4
typedef struct
{
	// BGRA 128bit construct, fits a single XMM register
	//float			LB, LG, LR, LA; // Light intensity for R/G/B components.
	union
	{
		dword			BGRA;		// 
		struct
		{
			byte LB, LG, LR, LA;
		};
	};
		
	float			PX,PY;      // Projected X and Y

	// 17.04.02 consider replacing PX, PY and TPos with x, y, and z.
	float			UZ, VZ, RZ; // U/Z, V/Z and 1/Z. (should be called UZ, VZ, RZ)
	Vector			Pos,TPos;   // Position and transformed position
	Vector			N;          // PseudoNormal
//	float			EU,EV;      // Environment mapping coordinates
//	float			REU,REV;    // EU/Z, EV/Z.
	float			U,V;        // Original mapping coordinates
	DWord			Flags;

//	dword			align16[3]; // this structure requires 16-byte alignment
	//  Word           Faces,FRem; // Faces = How many faces share that perticular
	// vertex, FRem = remaining visible faces.
	// when FRem reaches 0,the vertex will not be
	// transformed. (RULEZ)
} Vertex;

// [72 Bytes]
// [48 w/o U/V shit]
// [+12 for L.maps]
typedef void (*RasterFunc)(Vertex **V, dword numVerts);

typedef struct
{
	Vertex        * A;
	// flares need only one vertex pointer, so B,C can be overwritten with flare-related information
	Vertex        * B;
	union
	{
		Vertex        * C;
		float			FlareSize;
	};
	
	dword           Flags;
	float           NormProd;
	Vector          N;
	FDW             SortZ;
	
	float           U1,V1,U2,V2,U3,V3; //(U,V) coordinates on face
	RasterFunc		Filler;
	Material      * Txtr;
	//	Surface       * Surf; // For T-Caching.
} Face;

// [16 Bytes per Status Key]
typedef struct _Status_
{
	float          Frame;
	DWord          Stat;
	_Status_     * Next;
	_Status_     * Prev;
} ObjectStatus;

// [36 Bytes]
typedef struct _Object_
{
	void           * Data;
	DWord            Type;
	DWord            Number;
	Vector         * Pos;
	Matrix         * Rot;
	Vector           Pivot;
	_Object_       * Next;
	_Object_       * Prev;
	_Object_       * Parent;
	char           * Name;
} Object;

// Edge on a RT.Scene Trimesh representation (Useful for Elastic Models,
// Collision detection, and some other neat stuff like that)
struct Edge
{
	Vertex *A,*B;
	Face *P,*Q;
	float Length;
	DWord Flags;
};

// [172 Bytes]
// Total: [220 Bytes + 84 Bytes per Vertex + 56 Bytes per Face +
//         16 Bytes * (Spline Keys + Hide Keys)]
typedef struct _TriMesh_
{
	// strided array containing precalculated lighting. 
	// uses packed dword BGRA to reduce memory read bandwidth usage
	// was optimized out for sse benefit
	Color			*SL;
	DWord            VIndex;     //Amount of Vertices on Mesh
	DWord            FIndex;     //Amount of Faces on Mesh
	DWord            EIndex;     //Amount of Edges on Mesh
	Vertex         * Verts;      //Vertex List
	Face           * Faces;      //Face List
	Edge           * Edges;      //Edge List
	Spline           Pos;        //Position Track Spline
	Spline           Scale;      //Scale Track Spline
	Spline           Rotate;     //Orientation Track Spline
	ObjectStatus   * Status;     //Hide Track (crap)
	ObjectStatus   * CurStat;    //Current Hide status
	Vector           IPos;       //Interpolated Position
	Vector           IScale;     //Interpolated Scale
	Quaternion       IRot;       //Interpolated Orientation (Quaternion)
	Matrix           RotMat;     //Interpolated Orientation (Matrix)
	Vector		     Heading;    // Generated by Position Track spline for meshes with the Align to path flag.
	Vector           BSphereCtr; // Bounding Sphere Center.
	float			 BSphereRadius; // Bounding Sphere Radius.
	float            BSphereRad; // Bounding Sphere Radius, squared. kept for backward compatibility until pipeline code is updated
	DWord            Flags;

	// this is a temporary hack and will be removed in the future
	dword			 SortPriorityBias; // '1' value causes object to always be rendered first

	_TriMesh_      * Next;
	_TriMesh_      * Prev;
} TriMesh;

// [152 Bytes]
// Total: [216 Bytes + 16 Bytes * Spline Keys]
typedef struct _Camera_
{
	Spline           Roll;
	Spline           FOV;
	Spline           Source;
	Spline           Target;
	Vector           ISource;
	Vector           ITarget;
	Matrix           Mat;
	float            IRoll;
	float            IFOV;
	float            PerspX;
	float            PerspY;
	DWord            Flags;
	_Camera_       * Next;
	_Camera_       * Prev;
} Camera;

// [256 Bytes]
typedef struct _Omni_
{
	Color            L;
	Vertex           V;
	Face             F;
	Spline           Pos;
	Spline					 Size;
	Spline					 Range;
	//  Spline           Col; // Not Supported - Light Color Track.
	Vector           IPos;
	DWord            Flags;
	float            FallOff;
	float            HotSpot;
	float						 ISize;
	float						 IRange,rRange;
	_Omni_         * Next;
	_Omni_         * Prev;
	dword			 dummy1,dummy2;
} Omni;

// [68 Bytes]
typedef struct _SpotLight_
{
	Spline           Source;
	Spline           Target;
	Vector           ISource;
	Vector           ITarget;
	DWord            Flags;
	_SpotLight_    * Next;
	_SpotLight_    * Prev;
} SpotLight;

// entry for TBR - implements multiple lists of entries over a vector.
struct TBREntry
{	
	Face *F;
	dword next;
};

// [56 Bytes]
// Total: [(56 + 16 * Objects + (220 + 84 * Vertices + 56 * Faces + 16 *
//         (Spline + Hide Keys)) per TriMesh + 216 + (16 * Spline Keys) per
//         Camera + 200 * Omnies + 68 * SpotLights + 192 * Particles) +
//         44 * Materials + (64K * CharsperPixel * Textures) + 1Kb * Textures
//         (only if BPP==8) Bytes] = ALOT.
struct Scene
{
	// 16 Byte Aligned Members
	Color			 Ambient;			// scene's Ambient illumination

	Object         * ObjectHead;
	TriMesh        * TriMeshHead;
	Camera         * CameraHead;
	Omni           * OmniHead;
	SpotLight      * SpotLightHead;
	Surface        * Surfaces;			//<tss> oh yeah, good stuff..
	DWord            NumOfSurf;	
	
	float            StartFrame;		// Scene start frame
	float            EndFrame;			// Scene end frame
	DWord            NumOfObjs;			// Number of objects
	struct Particle *Pcl;				// Particle Dynamic array.
	void          (* PclExec)();		// Particle Sys. Kinematics and Effects.
	DWord            NumOfParticles;	// Number of particles
	Palette        * Pal;				// Collective palette
	DWord            Flags;				// Scene flags, for optimization
	float            FZP;				// Far-Z clipping plane
	float			 NZP;				// Near-Z clipping plane
	float			 PathingMinVelocity;// at this this velocity is required for objects to change heading.

	TBREntry		*SBuffer;
	dword			 SBufferCur;
	dword			 SBufferSize;
	sdword			*SBufferHead;
	dword			 NumTiles;
};

// [16 Bytes]
// The functionability of this structure is still unclear... (Silvatar)
// I added the ID variable for more exact tracking of parent fitting
typedef struct _KeyFrame_
{
	Object        * Object;
	DWord           ID;
	_KeyFrame_    * Prev;
	_KeyFrame_    * Next;
} KeyFrame;

struct CurLight
{
	Color Col;
	Vector Pos; // position in relation to model
	float Range; // light range (before mesh scaling)
	float Range2;
	float rRange;
	dword	dummy1,dummy2;
};

struct MusicModuleInfo
{
	dword ModuleHandle;
	dword Order;
	dword Pattern;
	dword Row;
};

extern "C" int MMXState;

extern char G_String[256]; //General string.

// illumination
extern float Ambient_Factor;
extern float Diffusive_Factor;
extern float Specular_Factor;
extern float Range_Factor;
extern float ImageSize;

// Global variables that hold the current scene info


extern Scene            * CurScene;       // Globally referenced scene.

extern float           CurFrame;       // Current frame
extern Camera           * View;           // Main active camera
extern Camera             FC;             // Free Camera..(Future Crew rulez)
extern Material           Default_Mat;    // Default Material...
extern Material         * MatLib;         // yosi

extern unsigned short int OldFPUCW,FPUCW;
extern dword OldMXCSR, CurMXCSR;
extern long Timer_Freq;

extern char **MsgStr;
extern long *MsgClock;
extern DWord *MsgID;


extern char *TxtrBuf,*TxtrAlign;
// Face List. Allocate before use.
extern Face **FList;
extern Face **SList;
extern float FOVX,FOVY;
extern Face *DoFace; //Currently rendered face. Used for further mapper info.
extern float dTime;
extern float Exec_FPS;
extern long Polys,Frames;
extern long CPolys,COmnies,CPcls,CAll,Polys;
extern long Bytes1[257];
extern long Bytes2[257];
extern long Bytes3[257];
extern long Bytes4[257];
extern Texture Phong_Mapping;
extern float C_rFZP,C_FZP, C_rNZP, C_NZP;
extern dword g_renderedPolys, g_MipLevel;

extern "C" float g_zscale;
extern "C" float g_zscale256;

extern long g_FrameTime;


extern DWord Colors32[256];

#pragma pack(1)
typedef unsigned long DWord;

// Register structures
struct _RMWORDREGS {
	unsigned short ax, bx, cx, dx, si, di, cflag;
};

struct _RMBYTEREGS {
	unsigned char   al, ah, bl, bh, cl, ch, dl, dh;
};

typedef union {
	struct  _RMWORDREGS x;
	struct  _RMBYTEREGS h;
} RMREGS;

typedef struct {
	unsigned short  es;
	unsigned short  cs;
	unsigned short  ss;
	unsigned short  ds;
} RMSREGS;

typedef struct {
	long    edi;
	long    esi;
	long    ebp;
	long    reserved;
	long    ebx;
	long    edx;
	long    ecx;
	long    eax;
	short   flags;
	short   es,ds,fs,gs,ip,cs,sp,ss;
} _RMREGS;

// Module memory block
// Total size [32+Name size] bytes.
struct Module_MB
{
	DWord Address;
	DWord Handle;  //For low blocks, this will be [Selector:Segment].
	DWord Size;
	Module_MB *Next,*Prev,*Father,*Child,*Brother; //Brother I can see the light!
	// This holds Linked list, Hierarchial tree and Binary tree at the same
	// time. Linked list used for [de]Allocation,h.tree for info/delete
	char *Name;
};

struct Aligned_MB
{
	long Pos; //Position in aligned memory space.
	Aligned_MB *Prev,*Next;
};

extern DWord DPMI_InitFlag;
extern void *DPMI_Main;
extern Module_MB *DPMI_Core;
//char *DPMI_Heap; //Internal Heap (for Aligned memory support)
//DWord DPMI_Heap_Handle;

#pragma pack(4)
// VESA variables and structures
typedef char VPalette[768]; // Direct Video 8-BIT Palette

// Quantized point in 2D
/*struct QPoint
{
	long X,Y;
};

struct Rectangle
{
	QPoint *UL,*LR; //Upper left, Lower right
};

// will only be logically supported at this point.
struct Window
{
	Rectangle Loc;   //Location on screen
	QPoint DispOffs; //Display offset
};*/

// [92 Bytes]
struct VESA_Surface
{
	char *Data,*Targ;
	float *ZBuffer;
	long X,Y;
	long BPP,CPP;
	long BPSL;
	long CX,CY,X_1,Y_1; //X/2,Y/2,X-1,Y-1 respectively.
	float FOV;          //In degrees (for convinience)
	float PX,PY;        //Perspective ratios
	long PageSize;      //Size of allocation in Data.
	DWord Flags;        //Attributes for the surface.
	long *YTable;       //Y Offset table
//	Window *Wnd;        //Lousy piece of shit
	char *Pal;          //SHHIIIIT
	void (*Flip)(VESA_Surface *VS); //appropriate Flipping procedure to LFB.
	VESA_Surface *Next,*Prev;
	void *Handle;
};


// [13 Bytes]
// Packed for [24 Bytes]
typedef struct
{
	char *Ch;
	char *Len;
	char X,Y,BPP;
	unsigned short XY;
} Font;

// [11 Bytes]
// Packed for [16 Bytes]
struct VESA_Listinfo
{
	long X,Y;             // X and Y resolution
	unsigned short Mode;  // True mode notation (Displayed in Hex)
	char BPP;             // Bits per Pixel
};

struct VideoMode
{
	long XRes,YRes;
	long BPP;
	long BPSL;
	long Driver_ID;
	long Mode_ID;
	char *VideoMem;
	long PhysMem;
	DWord Flags;
};

extern short CurrentGraphicsMode;
extern void *VESAModule,*VESABuffer;
extern short *VESA_ModesAvail;
extern long VESA_NumModes;
extern long *YOffs;
extern char Palette_Reserved;

// General info
extern char Graphmode;  //set if client is on active g.mode state.
//otherwise it won't be able to determine what
//to do with the surfaces...
extern char Granola;         //GRANOLA=GRANULARITY ;), that is 64Kb/Vesamode->WinGranularity
extern "C" long VESA_BPSL;       //Crazy bytes per scan line crap. not currently supported.
extern "C" long BPP_Shift;
extern "C" long PageSize;
extern char Screenmem_Model; //1=LFB,2=Banks,3=Fake 14bit (not supported)
extern VESA_Surface *Screen; //this is where the LFB surface resides
extern VESA_Surface *MainSurf;   //this is regular surface u flip fastest,aka Page2 aka VPage
extern VESA_Surface *Last_Surface;
extern long VESA_Ver;

extern char Direct_FLIP;

// Global Surface Scalar variables (Fastest access)
extern "C" long XRes,YRes,XRes_1,YRes_1,CntrX,CntrY,BPP,CPP;
extern float CntrEX,CntrEY;
extern long PageSize,PageSizeW,PageSizeDW;

extern VideoMode VMode;
extern char *VGAPtr,*VPage; //Direct access pointers to LFB/Banks and v.Page[2].
extern float *ZBuffer;
extern Font *Active_Font;    //Currently Active font
extern Font *Font1; //a font
extern char *VESA_Pal;

extern float CParticle_ISize;


extern void (*Flip)(VESA_Surface *VS);
extern VESA_Surface Layer2;

extern long Basic_FOffs; //basic font offset

// ISR variables and structures
const long Rate_Div = 1193180;
const unsigned int Basic_Rate = 0xFFFF;
extern volatile long Timer;
extern long Old_Int_Count,Old_Int_Rate;
extern volatile char Keyboard[128];

//extern Material Flare_IR,Flare_IG,Flare_IB;
extern Material Flare;

// Span buffer variables
extern char *Bitmap;

// Frustrum variables
extern float FOVX,FOVY;
extern float FillerPixelcount;

// Filler externals
extern dword zReject, zPass;
extern __int64 precisePixelCount;

typedef int (__stdcall *VPFunc)(
								void *lpAddress,		// address of region of committed pages
								dword dwSize,			// size of the region
								dword flNewProtect,	// desired access protection
								dword *lpflOldProtect // address of variable to get old protection
								);

//extern long ASM_XRes;
extern float *InvTbl;


extern long Kbpos;

// Particle System Variables
// [192 Bytes]
struct Particle
{
	Vertex V;
	Face F;
	Vector Conv,Vel;
	QColor Color;
	float Grav,Charge;
	float Mass;
	void (*GFX)(Particle *P);
	float Radius;
	float PRad;
	DWord Flags;
};





////////////////////////////////////
// Radiosity unit - Global variables
extern DWord SCacheSize;

#pragma pack(1)

//stuff
extern "C" dword WobNumOfHorizontalBlocks;
extern "C" dword WobNumOfVerticalBlocks;
extern "C" dword WobBlockJump;
extern "C" dword WobBPSL;

extern "C" dword MMXWobNumOfHorizontalBlocks;
extern "C" dword MMXWobNumOfVerticalBlocks;
extern "C" dword MMXWobBlockJump;
extern "C" dword MMXWobBPSL;

extern "C" DWord WOBPOINTSHEIGHT;
extern long Grid_X,Grid_Y,Grid_XY;
extern float Plane_Z2U_Conv;
extern float Plane_X2V_Conv;
extern TriMesh *Meatballs; // Target of Meat-Balls tesselation routine
extern float WRFOV;
extern Vector *WPDT;

// for the rend1 scene renderer
extern Particle *ExpPcls;
extern long NumExpPcls;

extern Vertex **ColVtx_Stack,**CVS;
extern Edge **ColEdge_Stack,**CES;

extern FILE *LogFile;

///////////////////////
/// TABLES!!!!!!!!!!!
extern DWord _t_sqrttab[];
extern DWord _t_rsqrttab[];
extern DWord _t_exptab[];
extern DWord _t_rexptab[];
extern float _t_fxfl;


////////////////////////////////////////////////////////////////////
///////////// ASSEMBLER / AUXILERY PRAGMAS SECTION /////////////////
////////////////////////////////////////////////////////////////////
#ifdef _C_WATCOM
void MatrixXVector(Matrix M,Vector *U,Vector *V);
#pragma aux MatrixXVector=\
	"FLD   DWORD PTR [ESI]"\
	"FMUL  DWORD PTR [EDX]"\
	"FLD   DWORD PTR [ESI]"\
	"FMUL  DWORD PTR [EDX+12]"\
	"FLD   DWORD PTR [ESI]"\
	"FMUL  DWORD PTR [EDX+24]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FMUL  DWORD PTR [EDX+4]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FMUL  DWORD PTR [EDX+16]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FMUL  DWORD PTR [EDX+28]"\
	"FXCH  ST(2)"\
	"FADDP ST(5),ST(0)"\
	"FADDP ST(3),ST(0)"\
	"FADDP ST(1),ST(0)"\
	"FLD   DWORD PTR [ESI+8]"\
	"FMUL  DWORD PTR [EDX+8]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FMUL  DWORD PTR [EDX+20]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FMUL  DWORD PTR [EDX+32]"\
	"FXCH  ST(2)"\
	"FADDP ST(5),ST(0)"\
	"FADDP ST(3),ST(0)"\
	"FADDP ST(1),ST(0)"\
	"FXCH  ST(1)"\
	"FXCH  ST(2)"\
	"FSTP  DWORD PTR [EDI]"\
	"FSTP  DWORD PTR [EDI+8]"\
	"FSTP  DWORD PTR [EDI+4]"\
	parm [EDX] [ESI] [EDI]\
modify exact [8087];


void Vector_Add(Vector *U,Vector *V,Vector *W);
#pragma aux Vector_Add=\
	"FLD   DWORD PTR [ESI]"\
	"FADD  DWORD PTR [EDX]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FADD  DWORD PTR [EDX+4]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FADD  DWORD PTR [EDX+8]"\
	"FXCH  ST(2)"\
	"FSTP  DWORD PTR [EDI]"\
	"FSTP  DWORD PTR [EDI+4]"\
	"FSTP  DWORD PTR [EDI+8]"\
	parm [ESI] [EDX] [EDI]\
modify exact [8087];

void Vector_Sub(Vector *U,Vector *V,Vector *W);
#pragma aux Vector_Sub=\
	"FLD   DWORD PTR [ESI]"\
	"FSUB  DWORD PTR [EDX]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FSUB  DWORD PTR [EDX+4]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FSUB  DWORD PTR [EDX+8]"\
	"FXCH  ST(2)"\
	"FSTP  DWORD PTR [EDI]"\
	"FSTP  DWORD PTR [EDI+4]"\
	"FSTP  DWORD PTR [EDI+8]"\
	parm [ESI] [EDX] [EDI]\
modify exact [8087];

/*void Vector_Scale(Vector *U,float &Scale,Vector *W);
#pragma aux Vector_Scale=\
"FLD   DWORD PTR [ESI]"\
"FMUL  DWORD PTR [EDX]"\
"FLD   DWORD PTR [ESI+4]"\
"FMUL  DWORD PTR [EDX]"\
"FLD   DWORD PTR [ESI+8]"\
"FMUL  DWORD PTR [EDX]"\
"FXCH  ST(2)"\
"FSTP  DWORD PTR [EDI]"\
"FSTP  DWORD PTR [EDI+4]"\
"FSTP  DWORD PTR [EDI+8]"\
parm [ESI] [EDX] [EDI]\
modify exact [8087];
*/

void FPU_LPrecision(void);
#pragma aux FPU_LPrecision=\
	"   FNStCW  [OldFPUCW]            " \
	"   Mov     ax,[OldFPUCW]         " \
	"   And     eax,1111101111111111b " \
	"   Or      eax,0000100000000000b " \
	"   And     eax,64767             " \
	"   Mov     [FPUCW],ax        " \
	"   FLdCW   [FPUCW]           " \
modify exact [eax 8087];

void FPU_NormalState(void);
#pragma aux FPU_NormalState=\
	"   FLdCW   [OldFPUCW]"\
modify exact [8087];

void RoundToInt(long *dest, float val);
#pragma aux RoundToInt=\
	"fistp DWORD [eax]"\
	parm nomemory [eax] [8087]\
modify exact [eax 8087];

void RoundToDW(DWord *dest, float val);
#pragma aux RoundToDW=\
	"fistp DWORD [eax]"\
	parm nomemory [eax] [8087]\
modify exact [eax 8087];


float SQRT(float Value);
#pragma aux SQRT =                               \
	"     mov   ecx,eax"                          \
	"     and   eax,0xffffff"                     \
	"     shr   ecx,23"                           \
	"     xor   eax,0x800000"                     \
	"     shr   eax,12"                           \
	"     mov   edx,[eax*4+_t_sqrttab]"           \
	"     mov   ecx,[ecx*4+_t_exptab]"            \
	"     or    ecx,edx"                          \
	"     mov   _t_fxfl,ecx"                      \
	"     fld   _t_fxfl"                          \
    parm caller [eax]                            \
modify [eax ecx edx];

/*北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北*/
// 1.0 / sqrt - fast reciprocal square root (to within 5 digits)

float RSQRT(float Value);

#pragma aux RSQRT =                           \
	"     mov   ecx,eax"                           \
	"     and   eax,0xffffff"                      \
	"     shr   ecx,23"                            \
	"     xor   eax,0x800000"                      \
	"     shr   eax,12"                            \
	"     mov   edx,[eax*4+_t_rsqrttab]"           \
	"     mov   ecx,[ecx*4+_t_rexptab]"            \
	"     test  edx,edx"                           \
	"     jz    rsqrt_psqr"                        \
	"     or    ecx,edx"                           \
	"     mov   _t_fxfl,ecx"                       \
	"     fld   _t_fxfl"                           \
	"     jmp   rsqrt_exit"                        \
	"rsqrt_psqr:"                                  \
	"     add   ecx,0x800000"                      \
	"     or    ecx,edx"                           \
	"     mov   _t_fxfl,ecx"                       \
	"     fld   _t_fxfl"                           \
	"rsqrt_exit:"                                  \
    parm caller [eax]                             \
modify [eax ecx edx];

#endif

#ifdef _C_VC6
inline void MatrixXVector(Matrix M,Vector *U,Vector *V)
{
	__asm {
		mov	EDX, [M]
			mov ESI, [U]
			mov EDI, [V]
			FLD   DWORD PTR [ESI]
			FMUL  DWORD PTR [EDX]
			FLD   DWORD PTR [ESI]
			FMUL  DWORD PTR [EDX+12]
			FLD   DWORD PTR [ESI]
			FMUL  DWORD PTR [EDX+24]
			FLD   DWORD PTR [ESI+4]
			FMUL  DWORD PTR [EDX+4]
			FLD   DWORD PTR [ESI+4]
			FMUL  DWORD PTR [EDX+16]
			FLD   DWORD PTR [ESI+4]
			FMUL  DWORD PTR [EDX+28]
			FXCH  ST(2)
			FADDP ST(5),ST(0)
			FADDP ST(3),ST(0)
			FADDP ST(1),ST(0)
			FLD   DWORD PTR [ESI+8]
			FMUL  DWORD PTR [EDX+8]
			FLD   DWORD PTR [ESI+8]
			FMUL  DWORD PTR [EDX+20]
			FLD   DWORD PTR [ESI+8]
			FMUL  DWORD PTR [EDX+32]
			FXCH  ST(2)
			FADDP ST(5),ST(0)
			FADDP ST(3),ST(0)
			FADDP ST(1),ST(0)
			FXCH  ST(1)
			FXCH  ST(2)
			FSTP  DWORD PTR [EDI]
			FSTP  DWORD PTR [EDI+8]
			FSTP  DWORD PTR [EDI+4]
	}
}

inline void Vector_Add(Vector *U,Vector *V,Vector *W)
{
	__asm {
		mov esi, [U]
			mov edx, [V]
			mov edi, [W]
			FLD   DWORD PTR [ESI]
			FADD  DWORD PTR [EDX]
			FLD   DWORD PTR [ESI+4]
			FADD  DWORD PTR [EDX+4]
			FLD   DWORD PTR [ESI+8]
			FADD  DWORD PTR [EDX+8]
			FXCH  ST(2)
			FSTP  DWORD PTR [EDI]
			FSTP  DWORD PTR [EDI+4]
			FSTP  DWORD PTR [EDI+8]
	}
}

inline void Vector_Sub(Vector *U,Vector *V,Vector *W)
{
	__asm {
		mov esi, [U]
			mov edx, [V]
			mov edi, [W]
			FLD   DWORD PTR [ESI]
			FSUB  DWORD PTR [EDX]
			FLD   DWORD PTR [ESI+4]
			FSUB  DWORD PTR [EDX+4]
			FLD   DWORD PTR [ESI+8]
			FSUB  DWORD PTR [EDX+8]
			FXCH  ST(2)
			FSTP  DWORD PTR [EDI]
			FSTP  DWORD PTR [EDI+4]
			FSTP  DWORD PTR [EDI+8]
	}
}

inline void FPU_LPrecision()
{
	__asm {
		// FPU control word
		FNStCW  [OldFPUCW]
		Mov     ax,[OldFPUCW]
		And     eax,1111101111111111b
		Or      eax,0000100000000000b
		And     eax,1111110011111111b            
		Mov     [FPUCW],ax        
		FLdCW   [FPUCW]

		// SSE control word
		STMXCSR	 [OldMXCSR];      RCMSKBIT EXPBIT
		mov eax, [OldMXCSR];  8765432109876543210
		and eax, 11111111111111110000000001000000b
		or  eax, 00000000000000000101111110000000b
		mov [CurMXCSR], eax
		LDMXCSR  [CurMXCSR]
	}
}


inline void FPU_NormalState()
{
	__asm {
		FLdCW   [OldFPUCW]
	}
}

inline void RoundToInt(long *dest, float val)
{
	__asm {
		fld dword ptr [val]
			mov eax, [dest]
			fistp [eax]
	}
}

inline void RoundToDW(DWord *dest, float val)
{
	__asm {
		fld dword ptr [val]
			mov eax, [dest]
			fistp [eax]
	}
}


inline float SQRT(float Value)
{
	__asm {
		mov eax, dword ptr [Value]
			mov   ecx,eax
			and   eax,0xffffff
			shr   ecx,23
			xor   eax,0x800000
			shr   eax,12
			mov   edx,[eax*4+_t_sqrttab]
			mov   ecx,[ecx*4+_t_exptab] 
			or    ecx,edx
			mov   _t_fxfl,ecx
	}
	return _t_fxfl;
}

/*北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北*/
// 1.0 / sqrt - fast reciprocal square root (to within 5 digits)

inline float RSQRT(float Value)
{
	__asm {
		mov eax, dword ptr [Value]
			mov   ecx,eax
			and   eax,0xffffff
			shr   ecx,23
			xor   eax,0x800000
			shr   eax,12
			mov   edx,[eax*4+_t_rsqrttab]
			mov   ecx,[ecx*4+_t_rexptab]
			test  edx,edx
			jz    rsqrt_psqr
			or    ecx,edx
			mov   _t_fxfl,ecx
			jmp   rsqrt_exit
rsqrt_psqr:
        add   ecx,0x800000
			or    ecx,edx
			mov   _t_fxfl,ecx        
rsqrt_exit:
	}
	return _t_fxfl;
}

inline void Fist(word &r, float x)
{	
	__asm 
	{
		mov edi, [r]
		FLD dword ptr x
		FISTP word ptr [edi]
	}
}

inline void Fist(sword &r, float x)
{	
	__asm 
	{
		mov edi, [r]
		FLD dword ptr x
		FISTP word ptr [edi]
	}
}

inline void Fist(dword &r, float x)
{	
	__asm 
	{
		mov edi, [r]
		FLD dword ptr x
		FISTP dword ptr [edi]
	}
}

inline dword Fist(float x)
{
	static dword ceiled;
	__asm 
	{
		FLD   dword ptr [x]		
		FISTP dword ptr [ceiled]
		;mov eax, [ceiled]
	}
	return ceiled;
}

// stores 'data' n times starting from address p
// (p[0...n-1] = data)
inline void FastWrite(void *p, dword data, dword n)
{
	__asm
	{
		mov edi, [p]
		mov eax, data
		mov ecx, n

		cmp ecx, 16
		jge optver
		rep stosd
		jmp afterfill
		; setup MMX register
optver:
		movd MM0, data
		movq MM1, MM0
		punpckldq MM0, MM1

		lea edi, [edi+ecx*4]
		neg ecx

		; align write
		test edi, 4
		jz afterprior

		mov [edi+ecx*4], eax
		inc ecx

afterprior:
		mov edx, ecx

		sar ecx, 1 ; # qwords
		and edx, 1 ; remainder dwords
		
quads:
		;movntq [edi+ecx*8], MM0
		__asm _emit 0x0F
		__asm _emit 0xE7
		__asm _emit 0x04
		__asm _emit 0xCF
		inc ecx
		jnz quads

		and edx,edx
		jz afterpost
		mov [edi-4], eax
afterpost:
		emms
afterfill:
	}
}

inline void rcpss(float *s)
{
	__asm
	{
		mov edi, [s]
		rcpss xmm0, [edi]
		movss [edi], xmm0
	}
}


#endif

#endif //FDS_VARS_H_INCLUDED