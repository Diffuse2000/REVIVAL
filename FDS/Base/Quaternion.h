#ifndef REVIVAL_QUATERNION_H
#define REVIVAL_QUATERNION_H

#include "Vector.h"

#pragma pack(push, 1)

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

#pragma pack(pop)

#endif //REVIVAL_QUATERNION_H
