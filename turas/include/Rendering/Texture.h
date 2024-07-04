//
// Created by liam_ on 7/4/2024.
//

#pragma once
namespace turas {

    class Texture {
    public:
        enum class MapType
        {
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
            Metalness,
            Roughness,
            AO,
            PBR_Sheen,
            PBR_Clearcoat,
            PBR_Transmission,
            General,
        };
    };
}