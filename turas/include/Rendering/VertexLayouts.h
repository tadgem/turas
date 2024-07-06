//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "lvk/Mesh.h"
#include "Core/Types.h"

namespace turas {

    struct VertexLayoutData
    {
        VkVertexInputBindingDescription             m_BindingDescription;
        Vector<VkVertexInputAttributeDescription>   m_AttributeDescriptions;
        u64                                         m_Hash;
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

        VertexLayoutDataBuilder(lvk::VulkanAPI& vk);

        void AddAttribute(VkFormat format, uint32_t attributeSize);
        VertexLayoutData Build();

        Vector<Attribute>   m_Attributes;
        uint32_t            m_LocationCount = 0;

    protected:
        lvk::VulkanAPI& p_VK;

    };

}