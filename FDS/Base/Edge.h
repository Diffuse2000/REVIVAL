#ifndef REVIVAL_EDGE_H
#define REVIVAL_EDGE_H
#include "Vertex.h"
#include "Face.h"

#pragma pack(push, 1)

// Edge on a RT.Scene Trimesh representation (Useful for Elastic Models,
// Collision detection, and some other neat stuff like that)
struct Edge
{
    Vertex *A,*B;
    Face *P,*Q;
    float Length;
    DWord Flags;
};

#pragma pack(pop)

#endif //REVIVAL_EDGE_H
