#ifndef REVIVAL_TEXTURE_H
#define REVIVAL_TEXTURE_H

#pragma pack(push, 1)

#include "Palette.h"

// Scene variables and structures
// [16 Bytes]
// Total: [16 Bytes+64K per Charperpixel,+1Kb if Palettized]
struct Texture
{
    Palette *Pal;   // Texture's Local Palette. Relevant only for 8BPP Textures
    byte *Data;     // Texture's Storage space.
    DWord BPP;      // Texture's BPP. Adjusts format in memory.
    // 0 used when Texture is invalid.
    // DWord size for Aligning, Should be treated as Byte.
    char *FileName; // Texture's Originating File-name.

    // OptClass - Will be used by Assign_Fillers to determine Raster device
    DWord OptClass; // 0 = 256x256 ; 1 = square 2^n ; 2 = arbitrary size.
    int32_t SizeX,SizeY;
	int32_t LSizeX, LSizeY; // log2(SizeX), log2(SizeY).

    // Mipmapping/blocktiling support.
	int32_t blockSizeX, blockSizeY; // zero to disable, 1<<blockSize measures length in pixels of each block
    byte *Mipmap[16]; // pointers to up to 16 levels of mipmaps
    dword numMipmaps; // number of mipmap levels.

    dword ID;
    dword Flags;
};

#pragma pack(pop)

#endif //REVIVAL_TEXTURE_H
