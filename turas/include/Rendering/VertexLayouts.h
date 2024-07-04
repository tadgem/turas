//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "lvk/Mesh.h"
namespace turas {
    enum class VertexLayout
    {
        Pos,
        PosUv,
        PosNormalUv
    };

    struct VertexLayoutData
    {
        VkVertexInputBindingDescription             m_BindingDescription;
        Vector<VkVertexInputAttributeDescription>   m_AttributeDescriptions;
    };

    static VertexLayoutData GetVertexDataLayout(const VertexLayout& layout)
    {
        switch (layout) {
            case VertexLayout::Pos:
                return {
                    lvk::VertexDataPos4::GetBindingDescription(),
                    lvk::VertexDataPos4::GetAttributeDescriptions()
                };
                break;
            case VertexLayout::PosUv:
                return {
                        lvk::VertexDataPosUv::GetBindingDescription(),
                        lvk::VertexDataPosUv::GetAttributeDescriptions()
                };
                break;
            case VertexLayout::PosNormalUv:
                return {
                        lvk::VertexDataPosNormalUv::GetBindingDescription(),
                        lvk::VertexDataPosNormalUv::GetAttributeDescriptions()
                };
                break;
        }
    }

}