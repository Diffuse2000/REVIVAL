#ifndef REVIVAL_OMNI_H
#define REVIVAL_OMNI_H

#include "Color.h"
#include "Vertex.h"
#include "Face.h"
#include "Spline.h"

#pragma pack(push, 1)

// [256 Bytes]
struct Omni
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
	Omni           * Next;
	Omni           * Prev;
	dword			 dummy1,dummy2;
};

#pragma pack(pop)

#endif //REVIVAL_OMNI_H
