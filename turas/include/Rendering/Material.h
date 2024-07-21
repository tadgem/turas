//
// Created by liam_ on 7/14/2024.
//

#pragma once

#include "Core/Types.h"
#include "STL/HashMap.h"
#include "Rendering/Texture.h"
#include "STL/String.h"
#include "lvk/Material.h"

namespace turas {
    // Store these in renderer, returned from pipeline?
    // represents a shader a drawable in the scene can use,
    // and associated set of map types to uniform names in this shader
    class MaterialCandidate {
    public:

        String m_ShaderName;
        HashMap<Texture::MapType, String> m_AvailableMaterialMaps;
    };

    struct MaterialComponent
    {
        u64 m_ShaderHash;
    };
}