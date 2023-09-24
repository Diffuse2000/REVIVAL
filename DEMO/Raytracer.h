#ifndef _DEMO_RAYTRACER_H_INCLUDED
#define _DEMO_RAYTRACER_H_INCLUDED

#include "Base/FDS_DECS.h"
#include "Base/TriMesh.h"
#include "Base/Face.h"
#include "Base/Vector.h"

struct rtIntersection
{
	TriMesh *T;
	Face *F;
	float u, v;
	float t;
};

struct rtLocalRay
{
	Vector position;
	Vector direction;
};

void GlowRaytracer(Scene *Sc, Camera *Viewer);

#endif