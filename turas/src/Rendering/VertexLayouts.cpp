#include "Rendering/VertexLayouts.h"
#include "Core/Utils.h"

turas::VertexLayoutData turas::VertexLayoutDataBuilder::Build() {
    VertexLayoutData data {};
    data.m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    data.m_BindingDescription.binding = 0; // TODO: do we need to support more than one binding?
    for(auto& attribute : m_Attributes)
    {
        data.m_BindingDescription.stride += attribute.m_Size;
    }

    u64 hash = 0;

    data.m_AttributeDescriptions.resize(m_Attributes.size());
    for(int i = 0; i < m_Attributes.size(); i++)
    {
        auto& attr = m_Attributes[i];
        auto& vkAttr = data.m_AttributeDescriptions[i];

        vkAttr.binding = attr.m_Binding;
        vkAttr.location = attr.m_Location;
        vkAttr.offset = attr.m_Offset;
        vkAttr.format = attr.m_Format;

        hash ^= attr.m_Binding ^ attr.m_Location ^attr.m_Offset ^attr.m_Size ^ (u32)attr.m_Format;
    }

    data.m_Hash = hash;
    return data;
}

turas::VertexLayoutDataBuilder::VertexLayoutDataBuilder(lvk::VulkanAPI &vk) : p_VK(vk)
{

}

void turas::VertexLayoutDataBuilder::AddAttribute(VkFormat format, u32 attributeSize) {
    Attribute a {};
    a.m_Binding  = 0; // TODO: Need more than one binding?
    a.m_Location = m_LocationCount;
    a.m_Format = format;
    a.m_Size = attributeSize;

    if(m_Attributes.empty())
    {
        a.m_Offset = 0;
        m_Attributes.push_back(a);
    }
    else {
        u32 offset = 0;
        i32 currentIndex = static_cast<i32>(m_LocationCount - 1);
        while (currentIndex >= 0)
        {
            offset += m_Attributes[currentIndex].m_Size;
            currentIndex--;
        }
        a.m_Offset = offset;
        m_Attributes.push_back(a);
    }

    m_LocationCount++;

}
