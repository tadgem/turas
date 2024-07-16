#include "Rendering/Renderer.h"
#include "Core/Utils.h"
#include "Debug/Profile.h"

void turas::Renderer::Start() {
    ZoneScoped;
    m_VK.Start("Turas", 1280, 720);
    m_Im3dState = lvk::LoadIm3D(m_VK);
}

void turas::Renderer::Shutdown() {
    ZoneScoped;
    lvk::FreeIm3d(m_VK, m_Im3dState);
    m_VK.Cleanup();
}

void turas::Renderer::PreFrame() {
    ZoneScoped;
    m_VK.PreFrame();
    VkCommandBuffer& cmd = m_VK.BeginGraphicsCommands(m_VK.GetFrameIndex());
}

void turas::Renderer::PostFrame() {
    ZoneScoped;
    m_VK.EndGraphicsCommands(m_VK.GetFrameIndex());
    m_VK.PostFrame();
}

bool turas::Renderer::AddPipelineTemplate(turas::u64 hash, const turas::CreatePipelineCallback& pipelineTemplateFunction) {
    ZoneScoped;
    if(p_CreatePipelineCallbacks.find(hash) != p_CreatePipelineCallbacks.end())
    {
        return false;
    }

    p_CreatePipelineCallbacks.emplace(hash, pipelineTemplateFunction);
    return true;
}

bool turas::Renderer::RemovePipelineTemplate(turas::u64 hash) {
    ZoneScoped;
    if(p_CreatePipelineCallbacks.find(hash) == p_CreatePipelineCallbacks.end())    {
        return false;
    }

    p_CreatePipelineCallbacks.erase(hash);
    return true;
}

turas::View *turas::Renderer::CreateView(const turas::String &name, turas::u64 pipelineHash) {
    ZoneScoped;
    if(p_CreatePipelineCallbacks.find(pipelineHash) == p_CreatePipelineCallbacks.end()) {
        // cant create a pipeline for this view
        return nullptr;
    }

    Pipeline* p = p_CreatePipelineCallbacks[pipelineHash]((Renderer*)this);
    u64 viewNameHash = Utils::Hash(name);

    p_ViewData.emplace(viewNameHash, ViewData {CreateUnique<View>(name), UPtr<Pipeline>(p)});
    return p_ViewData[viewNameHash].m_View.get();
}

bool turas::Renderer::DestroyView(const turas::String &name) {
    ZoneScoped;
    u64 hash = Utils::Hash(name);
    if(p_ViewData.find(hash) == p_ViewData.end())
    {
        return false;
    }

    p_ViewData[hash].Free(m_VK);
    p_ViewData.erase(hash);
    return true;
}

turas::View *turas::Renderer::GetView(const turas::String& name) {
    ZoneScoped;
    return GetView(Utils::Hash(name));
}

turas::View *turas::Renderer::GetView(turas::u64 nameHash) {
    ZoneScoped;
    if(p_ViewData.find(nameHash) == p_ViewData.end())
    {
        return nullptr;
    }

    return p_ViewData[nameHash].m_View.get();
}

turas::Pipeline *turas::Renderer::GetViewPipeline(turas::u64 nameHash) {
    ZoneScoped;
    if(p_ViewData.find(nameHash) == p_ViewData.end())
    {
        return nullptr;
    }

    return p_ViewData[nameHash].m_Pipeline.get();
}

turas::Pipeline *turas::Renderer::GetViewPipeline(const turas::String &name) {
    ZoneScoped;
    return GetViewPipeline(Utils::Hash(name));
}

turas::Shader* turas::Renderer::GetShaderProgram(const turas::String &name) {
    ZoneScoped;
    u64 hash = Utils::Hash(name);
    return GetShaderProgram(hash);
}

turas::Shader* turas::Renderer::GetShaderProgram(turas::u64 hash) {
    ZoneScoped;
    if(p_Shaders.find(hash) == p_Shaders.end())
    {
        return nullptr;
    }

    return p_Shaders[hash].get();
}

void turas::Renderer::LoadShaders() {
    // Load all files ending in .spv in the shaders directory (recursively)
    Utils::GetFilesInDirectory("shaders/");
}

void turas::Renderer::ViewData::Free(lvk::VulkanAPI &vk) {
    ZoneScoped;
}
