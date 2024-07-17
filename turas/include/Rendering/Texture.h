//
// Created by liam_ on 7/4/2024.
//

#pragma once
namespace turas {

    class Texture {
    public:
        enum class MapType {
            Invalid = 0,
            Diffuse,
            Specular,
            Ambient,
            Emissive,
            Height,
            Normal,
            Shininess,
            Opacity,
            Displacement,
            Lightmap,
            Reflection,
            PBR_BaseColour,
            PBR_NormalCamera,
            PBR_EmissionColour,
            PBR_Metalness,
            PBR_Roughness,
            PBR_AO,
            Sheen,
            Clearcoat,
            Transmission,
            General,
        };
    };
}