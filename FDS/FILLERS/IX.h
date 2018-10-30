#ifndef IX_H_INCLUDED
#define IX_H_INCLUDED

#include "../FDS_DECS.h"
#include "F4Vec.h"

//#define MEASURE_ZSTATS
//#define MEASURE_POLYSTATS

struct IXVertexG
{
	union
	{
		struct
		{
			float B, G, R, z;	 // 16 bytes
		};
		F4Vec BGRZ;
	};	
	float x, RZ;		 // 8 bytes
	float y;			 // 4 bytes
	dword _align16[1];	 // 4 bytes
						 // Sum - 32 Bytes
};

struct IXVertexT
{
	union
	{
		struct
		{
			float x, RZ, UZ, VZ; // 16 bytes
		};
		F4Vec XZUV;
	};
	float y;			 // 4 bytes
	dword _align16[3];	 // 12 bytes
						 // Sum - 48 Bytes
};


struct IXVertexTG
{
	union
	{
		struct
		{
			float RZ, UZ, VZ, x; // 16 bytes
		};
		F4Vec ZUVX;
	};
	union
	{
		struct
		{
			float B, G, R, z;	 // 16 bytes
		};
		F4Vec BGRZ;
	};	
	float y;			 // 4 bytes
	dword _align16[3];	 // 12 bytes
						 // Sum - 48 Bytes
};

struct IXVertexF
{
	float R, G, B, z;	 // 16 bytes
	float x, RZ;		 // 8 bytes
	float y;			 // 4 bytes
	dword _align16[1];	 // 4 bytes
						 // Sum - 32 Bytes
};

extern mword zPass, zReject;
extern __int64 precisePixelCount;

#endif //IX_H_INCLUDED
