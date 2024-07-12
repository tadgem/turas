//
// Created by liam_ on 7/5/2024.
//

#pragma once
#include "STL/HashMap.h"
#include "STL/Functional.h"
#include "VulkanAPI_SDL.h"
#include "Rendering/Pipeline.h"
#include "Rendering/View.h"
#include "Im3D/im3d_lvk.h"
namespace turas {

    class Renderer;

    using CreatePipelineCallback = Function<Pipeline*(Renderer*)>;

    class Renderer {
    public:

        struct ViewData
        {
            UPtr<View>              m_View;
            UPtr<Pipeline>          m_Pipeline;
        };

        Renderer() = default;

        void        Start();
        void        Shutdown();

        void        PreFrame();
        void        PostFrame();

        bool        AddPipelineTemplate(u64 hash, const CreatePipelineCallback& pipelineTemplateFunction);
        bool        RemovePipelineTemplate(u64 hash);

        View*       CreateView(const String& name, u64 pipelineHash);
        void        DestroyView(const String& name);

        View*       GetView(const String& name);
        View*       GetView(u64 nameHash);

        Pipeline*   GetViewPipeline(const String& name);
        Pipeline*   GetViewPipeline(u64 nameHash);

        // interface to GPU (vulkan)
        lvk::VulkanAPI_SDL      m_VK;
        lvk::LvkIm3dState       m_Im3dState;

        TURAS_IMPL_ALLOC(Renderer)

        HashMap<u64, ViewData>  m_ViewData;

    protected:
        HashMap<u64, CreatePipelineCallback> p_CreatePipelineCallbacks;
    };
}