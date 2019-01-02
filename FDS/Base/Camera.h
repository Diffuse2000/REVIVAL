
#ifndef REVIVAL_CAMERA_H
#define REVIVAL_CAMERA_H
#include "Vector.h"
#include "Matrix.h"
#include "BaseDefs.h"
#include "Spline.h"

#pragma pack(push, 1)

// [152 Bytes]
// Total: [216 Bytes + 16 Bytes * Spline Keys]
struct Camera
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
	Camera       * Next;
	Camera       * Prev;
};

#pragma pack(pop)

#endif //REVIVAL_CAMERA_H
