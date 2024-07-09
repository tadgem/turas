#include "Rendering/Mesh.h"
#include "Core/Engine.h"

void turas::Mesh::Free(lvk::VulkanAPI& vk)
{
        // TODO: This isnt nice, probably should set up a free loop
        vkDestroyBuffer(vk.m_LogicalDevice, m_LvkMesh.m_VertexBuffer, nullptr);
        vkDestroyBuffer(vk.m_LogicalDevice, m_LvkMesh.m_IndexBuffer, nullptr);
        vmaFreeMemory(vk.m_Allocator, m_LvkMesh.m_VertexBufferMemory );
        vmaFreeMemory(vk.m_Allocator, m_LvkMesh.m_IndexBufferMemory );

}
