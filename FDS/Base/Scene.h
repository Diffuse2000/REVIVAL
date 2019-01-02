#ifndef REVIVAL_SCENE_H
#define REVIVAL_SCENE_H

#pragma pack(push, 1)

#include "TriMesh.h"
#include "SpotLight.h"
#include "Surface.h"
#include "TBR.h"
#include "Object.h"
#include "Camera.h"
#include "Omni.h"

// [56 Bytes]
// Total: [(56 + 16 * Objects + (220 + 84 * Vertices + 56 * Faces + 16 *
//         (Spline + Hide Keys)) per TriMesh + 216 + (16 * Spline Keys) per
//         Camera + 200 * Omnies + 68 * SpotLights + 192 * Particles) +
//         44 * Materials + (64K * CharsperPixel * Textures) + 1Kb * Textures
//         (only if BPP==8) Bytes] = ALOT.
struct Scene
{
    // 16 Byte Aligned Members
    Color			 Ambient;			// scene's Ambient illumination

    Object         * ObjectHead;
    TriMesh        * TriMeshHead;
    Camera         * CameraHead;
    Omni           * OmniHead;
    SpotLight      * SpotLightHead;
    Surface        * Surfaces;			//<tss> oh yeah, good stuff..
    DWord            NumOfSurf;

    float            StartFrame;		// Scene start frame
    float            EndFrame;			// Scene end frame
    DWord            NumOfObjs;			// Number of objects
    struct Particle *Pcl;				// Particle Dynamic array.
    void          (* PclExec)();		// Particle Sys. Kinematics and Effects.
    DWord            NumOfParticles;	// Number of particles
    Palette        * Pal;				// Collective palette
    DWord            Flags;				// Scene flags, for optimization
    float            FZP;				// Far-Z clipping plane
    float			 NZP;				// Near-Z clipping plane
    float			 PathingMinVelocity;// at this this velocity is required for objects to change heading.

    TBREntry		*SBuffer;
    dword			 SBufferCur;
    dword			 SBufferSize;
    sdword			*SBufferHead;
    dword			 NumTiles;
};

#pragma pack(pop)

#endif //REVIVAL_SCENE_H
