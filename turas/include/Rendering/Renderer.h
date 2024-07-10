//
// Created by liam_ on 7/5/2024.
//

#pragma once
#include "VulkanAPI_SDL.h"

namespace turas {
    class Renderer {
    public:
        Renderer();

        void Start();
        void Shutdown();

        void PreFrame();
        void PostFrame();

        // interface to GPU (vulkan)
        lvk::VulkanAPI_SDL      m_VK;
    };
}