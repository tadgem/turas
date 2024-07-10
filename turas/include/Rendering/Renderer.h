//
// Created by liam_ on 7/5/2024.
//

#pragma once
#include "STL/HashMap.h"
#include "STL/Functional.h"
#include "VulkanAPI_SDL.h"
#include "Rendering/Pipeline.h"

namespace turas {

    using CreatePipelineCallback = Function<Pipeline*(lvk::VulkanAPI& vk)>;

    class Renderer {
    public:
        Renderer() = default;

        void Start();
        void Shutdown();

        void PreFrame();
        void PostFrame();

        // interface to GPU (vulkan)
        lvk::VulkanAPI_SDL      m_VK;

        TURAS_IMPL_ALLOC(Renderer)

    protected:
        HashMap<u64, CreatePipelineCallback> p_CreatePipelineCallbacks;
    };
}