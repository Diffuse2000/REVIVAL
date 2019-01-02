#ifndef REVIVAL_KEYFRAME_H
#define REVIVAL_KEYFRAME_H

#include "BaseDefs.h"
#include "Object.h"

#pragma pack(push, 1)

// [16 Bytes]
// The functionability of this structure is still unclear... (Silvatar)
// I added the ID variable for more exact tracking of parent fitting
struct KeyFrame
{
    Object        * Object;
    DWord           ID;
    KeyFrame    * Prev;
    KeyFrame    * Next;
};

#pragma pack(pop)

#endif //REVIVAL_KEYFRAME_H
