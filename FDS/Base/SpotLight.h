#ifndef REVIVAL_SPOTLIGHT_H
#define REVIVAL_SPOTLIGHT_H

#pragma pack(push, 1)

#include "BaseDefs.h"
#include "Spline.h"
#include "Vector.h"
#include "SpotLight.h"

// [68 Bytes]
struct SpotLight
{
	Spline           Source;
	Spline           Target;
	Vector           ISource;
	Vector           ITarget;
	DWord            Flags;
	SpotLight      * Next;
	SpotLight      * Prev;
};

#pragma pack(pop)

#endif //REVIVAL_SPOTLIGHT_H
