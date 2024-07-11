//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "lvk/Mesh.h"
#include "Core/Types.h"
#include "STL/Vector.h"

namespace turas {

    struct VertexLayoutData
    {
        VkVertexInputBindingDescription             m_BindingDescription;
        Vector<VkVertexInputAttributeDescription>   m_AttributeDescriptions;
        u64                                         m_Hash;

        TURAS_IMPL_ALLOC(VertexLayoutData)
    };

    class VertexLayoutDataBuilder
    {
    public:

        struct Attribute
        {
            u16         m_Binding;
            u16         m_Location;
            VkFormat    m_Format;
            u32         m_Size;
            u32         m_Offset;
        };

        VertexLayoutDataBuilder() = default;

        void AddAttribute(VkFormat format, uint32_t attributeSize);
        VertexLayoutData Build();

        Vector<Attribute>   m_Attributes;
        uint32_t            m_LocationCount = 0;

        TURAS_IMPL_ALLOC(VertexLayoutDataBuilder)

    };

}