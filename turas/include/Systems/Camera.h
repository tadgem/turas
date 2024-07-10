//
// Created by liam_ on 7/10/2024.
//

#pragma once
#include "Core/System.h"
#include "Rendering/View.h"

namespace turas {

    struct CameraComponent
    {
        Camera  m_Camera;
    };

    class CameraSystem : public System {
    public:

        CameraSystem();
    };
}