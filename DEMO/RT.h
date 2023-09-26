#include "Base/BaseDefs.h"
#include "Base/FDS_DECS.H"
#include "Base/FDS_VARS.H"
#include "Base/Vector.h"
#include "Rev.h"
#include "IMGGENR/IMGGENR.H"
#include "VESA/Vesa.h"

#include <algorithm>

	// tables, generated and used as an optimization.
#define TRIG_ACC 4096 //trigonometric table accuracy. must be a power of 2.
#define TRIG_MASK (TRIG_ACC-1)
#define TRIG_FACTOR (float(TRIG_ACC)/PI_M2)

#ifdef _C_WATCOM
#define VSurface Screen
#else
#define VSurface MainSurf
#endif


static int32_t InitScreenXRes, InitScreenYRes;

inline float sdBox(const XMMVector &p, const XMMVector &b)
{
  auto q = p.abs() - b;
  auto maxq = max(q, 0.0);
  auto maxqlen = maxq.Length();
  auto boxtest = std::min(std::max(q.x,std::max(q.y,q.z)),0.0f);
  return maxqlen + boxtest;
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
		XMMVector box{1.0, 1.0, 1.0};
		return std::make_pair(sdBox(p, box), 1);
	}

	void RunFrame(float t)
	{
		XMMVector Intersection, Origin, Direction, U;
		XMMVector CameraPos(0,0,-10.0);
		XMMMatrix CamMat;
		Matrix_Identity(CamMat.Data);
		const int MAX_ITERATIONS = 128;
		const float epsilon = 0.01;
		
		memset(VPage,0,PageSize + XRes*YRes*sizeof(word));

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

				XMMVector p = CameraPos;

				for (int ii = 0; ii < MAX_ITERATIONS; ++ii)
				{

					auto d = map(p);
					if (d.first < epsilon) // hit 
					{
						*(dword *)(&Surf1.Data[x * Surf1.BPP + Surf1.BPSL * y]) = 0xff00ff;
					}

					p += Direction * d.first;
				}
			}
		}
		Flip(&Surf1);

	}
};

