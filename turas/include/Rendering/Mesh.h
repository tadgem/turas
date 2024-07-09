//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "lvk/Mesh.h"
#include "Rendering/VertexLayouts.h"

namespace turas {

    struct AABB
    {
        glm::vec3 m_Min;
        glm::vec3 m_Max;
    };

    struct Mesh
    {
        void Free(lvk::VulkanAPI& vk);
        // vertex data, vec of floats keeps it generic
        Vector<float>       m_VertexData;
        // mesh indices
        Vector<uint32_t>    m_IndexData;
        // vertex layout for vulkan
        VertexLayoutData    m_VertexLayout;
        // gpu submitted mesh
        lvk::Mesh           m_LvkMesh;
        // axis aligned bounding box for simple culling
        AABB                m_AABB;

    };
}