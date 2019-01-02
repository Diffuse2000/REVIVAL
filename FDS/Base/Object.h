#ifndef REVIVAL_OBJECT_H
#define REVIVAL_OBJECT_H

#include "BaseDefs.h"
#include "Vector.h"
#include "Matrix.h"

#pragma pack(push, 1)

// [16 Bytes per Status Key]
struct ObjectStatus
{
    float          Frame;
    DWord          Stat;
    ObjectStatus * Next;
    ObjectStatus * Prev;
};


// [36 Bytes]
struct  Object
{
	void           * Data;
	DWord            Type;
	DWord            Number;
	Vector         * Pos;
	Matrix         * Rot;
	Vector           Pivot;
	Object		   * Next;
	Object		   * Prev;
	Object		   * Parent;
	signed short	 ParentID;
	char           * Name;
};

#pragma pack(pop)

#endif //REVIVAL_OBJECT_H
