#include "Base/Vector.h"
#include "Rev.h"
#include "IMGGENR/IMGGENR.H"
#include "VESA/Vesa.h"

#include <algorithm>

void Cross_Fade(byte *U1,byte *U2,byte *Target,int32_t Perc)
{
	int32_t I;
	for(I=0;I<PageSize;I++)
		Target[I] = (U1[I]*(255-Perc) + U2[I]*Perc)>>8;
}


	// tables, generated and used as an optimization.
#define TRIG_ACC 4096 //trigonometric table accuracy. must be a power of 2.
#define TRIG_MASK (TRIG_ACC-1)
#define TRIG_FACTOR (float(TRIG_ACC)/PI_M2)

#ifdef _C_WATCOM
#define VSurface Screen
#else
#define VSurface MainSurf
#endif


static float *LenTable;
static float *SinTable;
static float *CosTable;
//static NewGridPoint *Plane_GP;
//static GridPoint *Plane_GP;
static GridPointTG *Plane_GP;
static GridPointT *Code_GP;
static GridPointT *Gfx_GP;
static GridPointT *Sfx_GP;
static VESA_Surface Surf1;
static VESA_Surface Surf2;
static VESA_Surface Surf3;
static VESA_Surface Surf4;
static VESA_Surface FinalSurf;
static int32_t numGridPoints;
static byte *Page1;
static byte *Page2;
static byte *Page3;
static byte *Page4;
static byte* FinalPage;
static int32_t TrigOffset;
static Texture *LogoTexture;
static Image *LogoImage;
static Texture *PlaneTexture;
static Image *PlaneImage;
static Texture *CodeTexture;
static Image *CodeImage;
static Texture *GfxTexture;
static Image *GfxImage;
static Texture *SfxTexture;
static Image *SfxImage;

static int32_t InitScreenXRes, InitScreenYRes;

float sdBox(const XMMVector &p, const XMMVector &b)
{
  auto q = p.abs() - b;
  return max(q,0.0).Length() + std::min(std::max(q.x,std::max(q.y,q.z)),0.0f);
}

class RT
{
	int32_t xres = XRes;
	int32_t yres = YRes;
	byte *Page1;
	VESA_Surface Surf1;

public:
	RT()
	{
		Page1 = (byte*)_aligned_malloc(PageSize, 32);
		memset(Page1, 0, PageSize);	
		memcpy(&Surf1,VSurface,sizeof(VESA_Surface));
		Surf1.Data = Page1;
		Surf1.Flags = VSurf_Noalloc;
		Surf1.Targ = NULL;

	}

	std::pair<float, int> map(XMMVector p)
	{
		int mat = 0;
		XMMVector box{}
		sdBox(p, const XMMVector &b)
	}

	void RunFrame()
	{
		XMMVector Intersection, Origin, Direction, U;
		XMMVector CameraPos(0,0,0);
		XMMMatrix CamMat;
		const int MAX_ITERATIONS = 128;	
		

		for (int y=0;y<=yres;y++)
		{
			for (int x=0;x<=xres;x++)
			{
				Direction.x = x - (xres >> 1);
				Direction.y = y - (yres >> 1);
				Direction.z = 1;
				Direction.w = .0f;
				
				Direction = CamMat * Direction;

				Direction.Normalize();



			}
		}

	}

};

