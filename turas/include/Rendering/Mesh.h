//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "lvk/Mesh.h"


namespace turas {
    struct CPUMesh {
    };

    struct GPUMesh
    {
        lvk::Mesh m_Mesh;
    };
}