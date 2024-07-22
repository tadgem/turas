//
// Created by liam_ on 7/4/2024.
//

#ifndef TURAS_ALL_MATERIAL_H
#define TURAS_ALL_MATERIAL_H

#include "STL/Memory.h"
#include "Core/Types.h"
#include "Rendering/Material.h"

namespace turas {
    struct MaterialComponent {
        TURAS_IMPL_ALLOC(MaterialComponent)
        u64             m_ShaderHash;
        lvk::Material*  m_Material = nullptr;
    };
}

#endif //TURAS_ALL_MATERIAL_H
