#include "Rendering/Renderer.h"
#include "Core/Utils.h"

void turas::Renderer::Start() {
    m_VK.Start("Turas", 1280, 720);
    m_Im3dState = lvk::LoadIm3D(m_VK);
}

void turas::Renderer::Shutdown() {
    lvk::FreeIm3d(m_VK, m_Im3dState);
    m_VK.Cleanup();
}

void turas::Renderer::PreFrame() {
    m_VK.PreFrame();
    VkCommandBuffer& cmd = m_VK.BeginGraphicsCommands(m_VK.GetFrameIndex());
}

void turas::Renderer::PostFrame() {
    m_VK.EndGraphicsCommands(m_VK.GetFrameIndex());
    m_VK.PostFrame();
}

bool turas::Renderer::AddPipelineTemplate(turas::u64 hash, const turas::CreatePipelineCallback& pipelineTemplateFunction) {
    if(p_CreatePipelineCallbacks.find(hash) != p_CreatePipelineCallbacks.end())
    {
        return false;
    }

    p_CreatePipelineCallbacks.emplace(hash, pipelineTemplateFunction);
    return true;
}

bool turas::Renderer::RemovePipelineTemplate(turas::u64 hash) {
    if(p_CreatePipelineCallbacks.find(hash) == p_CreatePipelineCallbacks.end())    {
        return false;
    }

    p_CreatePipelineCallbacks.erase(hash);
    return true;
}

turas::View *turas::Renderer::CreateView(const turas::String &name, turas::u64 pipelineHash) {
    if(p_CreatePipelineCallbacks.find(pipelineHash) == p_CreatePipelineCallbacks.end()) {
        // cant create a pipeline for this view
        return nullptr;
    }

    Pipeline* p = p_CreatePipelineCallbacks[pipelineHash]((Renderer*)this);
    u64 viewNameHash = Utils::Hash(name);

    m_ViewData.emplace(viewNameHash, ViewData {CreateUnique<View>(name), UPtr<Pipeline>(p)});
    return m_ViewData[viewNameHash].m_View.get();
}

void turas::Renderer::DestroyView(const turas::String &name) {
    u64 hash = Utils::Hash(name);
    if(m_ViewData.find(hash) == m_ViewData.end())
    {
        return;
    }
}

turas::View *turas::Renderer::GetView(const turas::String& name) {
    return GetView(Utils::Hash(name));
}

turas::View *turas::Renderer::GetView(turas::u64 nameHash) {
    if(m_ViewData.find(nameHash) == m_ViewData.end())
    {
        return nullptr;
    }

    return m_ViewData[nameHash].m_View.get();
}

turas::Pipeline *turas::Renderer::GetViewPipeline(turas::u64 nameHash) {
    if(m_ViewData.find(nameHash) == m_ViewData.end())
    {
        return nullptr;
    }

    return m_ViewData[nameHash].m_Pipeline.get();
}

turas::Pipeline *turas::Renderer::GetViewPipeline(const turas::String &name) {
    return GetViewPipeline(Utils::Hash(name));
}