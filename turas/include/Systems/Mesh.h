//
// Created by liam_ on 7/4/2024.
//

#ifndef TURAS_ALL_MESH_H
#define TURAS_ALL_MESH_H
#include "STL/Memory.h"
#include "Assets/Asset.h"

namespace turas
{
    struct MeshComponent
    {
        AssetHandle m_Handle;
        u32         m_EntryIndex;
        lvk::Mesh*  m_LvkMesh;

        template<typename Archive>
        void serialize(Archive& ar)
        {
            ZoneScoped;
            ar(m_Handle, m_EntryIndex);
        }

        TURAS_IMPL_ALLOC(MeshComponent)
    };
}
#endif //TURAS_ALL_MESH_H
