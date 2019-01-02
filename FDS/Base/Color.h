#ifndef REVIVAL_COLOR_H
#define REVIVAL_COLOR_H

#include "BaseDefs.h"

#pragma pack(push, 1)

// [12 Bytes]
struct Color
{
	float B, G, R, A;
};

// [4 Bytes]
union QColor
{
    dword BGRA;
    struct
    {
        byte B,G,R,A; //Red,Green,Blue,Alpha.
    };
};

#pragma pack(pop)

#endif //REVIVAL_COLOR_H
