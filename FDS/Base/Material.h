#ifndef REVIVAL_MATERIAL_H
#define REVIVAL_MATERIAL_H

#include "Color.h"
#include "Texture.h"
#include "Vector.h"

#pragma pack(push, 1)

struct Scene;

struct Material
{
    // 16 byte alignment variables
    Color                 BaseCol; //Base Color for the material

    Scene               * RelScene; //Related Scene  // This should be nuked from orbit. Just keep a scene id instead of a pointer
    Texture             * Txtr;
    DWord                 Flags;
    DWord                 TFlags; //Texture Flags

    dword				  ID;

    float                 Luminosity;
    float                 Diffuse; //Diffuse reflection
    float                 Specular; //Specular reflection
    float                 Reflection; //Rebounded light Reflection
    float                 Transparency; //Transparency ratio
    unsigned short        Glossiness; //unknown parameter
    unsigned short        ReflectionMode; //unknown parameter
    char                * ReflectionImage; //Reflection detail
    float                 ReflectionSeamAngle; //unknown
    float                 RefractiveIndex; //Optic Factor
    float                 EdgeTransparency; //unknown factor
    float                 MaxSmoothingAngle; //?
    char                * ColorTexture; //Color Map Textutre Filename
    char                * DiffuseTexture; //Diffuse Map Texture Filename
    char                * SpecularTexture; //Specular Map Texture Filename
    char                * ReflectionTexture; //Reflection Map Texture Filename
    char                * TransparencyTexture; //Transparency Map Texture Filename
    char                * BumpTexture; //Bump Map Texture Filename
    char                * TextureImage; //Texture Map Filename
    unsigned short        TextureFlags; //Should be replaced by dword TFlags
    Vector                TextureSize; //Texture Proj. Size in spatial coordinates
    Vector                TextureCenter; //Texture Origin in space
    Vector                TextureFallOff; //??
    Vector                TextureVelocity; //U/V animation speed
    char                * TextureAlpha; //Texture Map for Alpha Channel
    unsigned short        NoiseFrequencies; //??
    unsigned short        TextureWrapX; //Wrap flag for U
    unsigned short        TextureWrapY; //Wrap flag for V
    float                 AAStrength; //Antialiasing Strength
    float                 Opacity; //?? gotta learn some physics
    Texture							* EnvTexture;
    float									TFP0;
    float									TFP1;
    Material            * Next, * Prev;
    char                * Name;
};

#pragma pack(pop)

#endif //REVIVAL_MATERIAL_H
