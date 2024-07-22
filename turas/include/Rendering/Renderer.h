//
// Created by liam_ on 7/5/2024.
//

#pragma once

#include "STL/HashMap.h"
#include "STL/Functional.h"
#include "VulkanAPI_SDL.h"
#include "Rendering/Pipeline.h"
#include "Rendering/View.h"
#include "Rendering/Shader.h"
#include "Im3D/im3d_lvk.h"

namespace turas
{
    class Renderer;

    using CreatePipelineCallback = Function<Pipeline *(Renderer*)>;

    class Renderer
    {
    public:
        struct ViewData
        {
            UPtr<View> m_View;
            UPtr<Pipeline> m_Pipeline;

            void Free(lvk::VulkanAPI& vk);
        };

        Renderer(bool enableDebugValidation);

        void Start();

        void Shutdown();

        void PreFrame();

        void PostFrame();

        bool AddPipelineTemplate(u64 hash, const CreatePipelineCallback& pipelineTemplateFunction);

        bool RemovePipelineTemplate(u64 hash);

        View* CreateView(const String& name, u64 pipelineHash);

        bool DestroyView(const String& name);

        View* GetView(const String& name);

        View* GetView(u64 nameHash);

        Pipeline* GetViewPipeline(const String& name);

        Pipeline* GetViewPipeline(u64 nameHash);

        Shader* CreateShaderVF(const String& vertName, const String& fragName, const String& shaderName);

        bool DestroyShader(const String& shaderName);

        Shader* GetShader(const String& shaderName);

        TURAS_IMPL_ALLOC(Renderer)

        // interface to GPU (vulkan)
        lvk::VulkanAPI_SDL m_VK;
        lvk::LvkIm3dState m_Im3dState;

    protected:
        // callbacks to construct named pipelines
        HashMap<u64, CreatePipelineCallback> p_CreatePipelineCallbacks;
        // active view data
        HashMap<u64, ViewData> p_ViewData;
        // all loaded shader stage binaries
        HashMap<String, lvk::ShaderStage> p_ShaderStages;
        // all 'linked' shader programs
        HashMap<String, UPtr<Shader>> p_ShaderPrograms;

        friend class Engine;

        void OnImGui();

        void LoadShaderBinaries();
    };
}
