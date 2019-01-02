
#ifndef REVIVAL_FACE_H
#define REVIVAL_FACE_H

#include "BaseDefs.h"
#include "Material.h"
#include "Vertex.h"

#pragma pack(push, 1)

// [72 Bytes]
// [48 w/o U/V shit]
// [+12 for L.maps]
typedef void (*RasterFunc)(Vertex **V, dword numVerts);

// [4 Bytes]
union FDW
{
    float F;
    DWord DW;
};

struct Face
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
};

#pragma pack(pop)

#endif //REVIVAL_FACE_H
