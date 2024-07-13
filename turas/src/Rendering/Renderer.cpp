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

    m_ViewData.emplace(viewNameHash, ViewData {CreateUnique<View>(name), UPtr<Pipeline>(p)});
    return m_ViewData[viewNameHash].m_View.get();
}

bool turas::Renderer::DestroyView(const turas::String &name) {
    ZoneScoped;
    u64 hash = Utils::Hash(name);
    if(m_ViewData.find(hash) == m_ViewData.end())
    {
        return false;
    }

    m_ViewData[hash].Free(m_VK);
    m_ViewData.erase(hash);
    return true;
}

turas::View *turas::Renderer::GetView(const turas::String& name) {
    ZoneScoped;
    return GetView(Utils::Hash(name));
}

turas::View *turas::Renderer::GetView(turas::u64 nameHash) {
    ZoneScoped;
    if(m_ViewData.find(nameHash) == m_ViewData.end())
    {
        return nullptr;
    }

    return m_ViewData[nameHash].m_View.get();
}

turas::Pipeline *turas::Renderer::GetViewPipeline(turas::u64 nameHash) {
    ZoneScoped;
    if(m_ViewData.find(nameHash) == m_ViewData.end())
    {
        return nullptr;
    }

    return m_ViewData[nameHash].m_Pipeline.get();
}

turas::Pipeline *turas::Renderer::GetViewPipeline(const turas::String &name) {
    ZoneScoped;
    return GetViewPipeline(Utils::Hash(name));
}

bool turas::Renderer::AddShader(turas::u64 shaderNameHash, lvk::ShaderProgram* shader)
{
    ZoneScoped;
    if(m_Shaders.find(shaderNameHash) != m_Shaders.end())
    {
        return false;
    }
    m_Shaders.emplace(shaderNameHash,  UPtr<lvk::ShaderProgram>(shader));
    return true;
}


bool turas::Renderer::RemoveShader(turas::u64 shaderNameHash) {
    ZoneScoped;
    if(m_Shaders.find(shaderNameHash) == m_Shaders.end())
    {
        return false;
    }
    m_Shaders[shaderNameHash]->Free(m_VK);
    m_Shaders.erase(shaderNameHash);
    return true;
}

lvk::ShaderProgram *turas::Renderer::GetShaderProgram(const turas::String &name) {
    ZoneScoped;
    u64 hash = Utils::Hash(name);
    return GetShaderProgram(hash);
}

lvk::ShaderProgram *turas::Renderer::GetShaderProgram(turas::u64 hash) {
    ZoneScoped;
    if(m_Shaders.find(hash) == m_Shaders.end())
    {
        return nullptr;
    }

    return m_Shaders[hash].get();
}

void turas::Renderer::ViewData::Free(lvk::VulkanAPI &vk) {
    ZoneScoped;
}
