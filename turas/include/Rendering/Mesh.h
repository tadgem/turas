//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "lvk/Mesh.h"
#include "Rendering/VertexLayouts.h"

namespace turas {

    struct Mesh
    {
        // vertex data, vec of floats keeps it generic
        Vector<float>       m_Data;

        // vertex layout for vulkan
        VertexLayoutData    m_VertexLayout;

        // gpu submitted mesh
        lvk::Mesh*          m_GPUMesh = nullptr;

    };
}