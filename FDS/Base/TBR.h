#ifndef REVIVAL_TBR_H
#define REVIVAL_TBR_H

#include "BaseDefs.h"
#include "Face.h"

#pragma pack(push, 1)

// entry for TBR - implements multiple lists of entries over a vector.
struct TBREntry
{
    Face *F;
    dword next;
};

#pragma pack(pop)

#endif //REVIVAL_TBR_H
