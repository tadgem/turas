#include "STL/Memory.h"
#include "Core/Engine.h"
#include "Core/ECS.h"
#include "spdlog/spdlog.h"
#include "Debug/StatsWindow.h"

turas::Engine::Engine() {
    ZoneScoped;
    INSTANCE = this;
}

void turas::Engine::Init() {
    ZoneScoped;
    spdlog::info("Initialising Turas");
    m_VK.Start("Turas", 1280, 720);
    m_Im3dState = lvk::LoadIm3D(m_VK);
    for(auto& sys : m_EngineSubSystems)
    {
        sys->OnEngineReady();
    }
}

void turas::Engine::Shutdown() {
    ZoneScoped;
    CloseAllScenes();

    for(auto& scene : m_ActiveScenes)
    {
        scene.reset();
    }
    m_ActiveScenes.clear();
    for(auto& sys : m_EngineSubSystems)
    {
        sys->OnShutdown();
        sys.reset();
    }
    m_EngineSubSystems.clear();
    m_AssetManager.Shutdown();

    lvk::FreeIm3d(m_VK, m_Im3dState);
    m_VK.Cleanup();

    INSTANCE = nullptr;
}

void turas::Engine::Run()
{
    while(m_VK.ShouldRun())
    {
        FrameMark;
        ZoneScopedN("Frame");
        PrepFrame();
        m_AssetManager.OnUpdate();
        PendingScenes();
        SystemsUpdate();
        StatsWindow::OnImGuiStatsWindow(m_VK);
        SubmitFrame();
    }
}

turas::Scene *turas::Engine::CreateScene(const String& name) {
    ZoneScoped;
    Scene* scene =  m_ActiveScenes.emplace_back(std::move(CreateUnique<Scene>(name))).get();
    for(auto& sys : m_EngineSubSystems)
    {
        sys->OnSceneLoaded(scene);
    }
    return scene;
}

void turas::Engine::CloseScene(turas::Scene *scene) {
    ZoneScoped;
    int index = -1;
    for (int i = 0; i < m_ActiveScenes.size(); i++)
    {
        if(m_ActiveScenes[i].get() == scene)
        {
            index = i;
        }
    }

    if(index >= 0)
    {
        for(auto& sys : m_EngineSubSystems)
        {
            sys->OnSceneClosed(m_ActiveScenes[index].get());
        }
        m_ActiveScenes[index].reset();
        m_ActiveScenes.erase(m_ActiveScenes.begin() + index);
    }

}

void turas::Engine::CloseAllScenes() {
    ZoneScoped;
    for(auto& scene : m_ActiveScenes)
    {
        // sys scene cleanup logic
        for(auto& sys : m_EngineSubSystems)
        {
            sys->OnSceneClosed(scene.get());
        }
        scene.reset();
    }

    m_ActiveScenes.clear();

}

void turas::Engine::PrepFrame() {
    ZoneScoped;
    m_VK.PreFrame();
    Im3d::NewFrame();
}

void turas::Engine::SubmitFrame() {
    ZoneScoped;
    Im3d::EndFrame();
    m_VK.PostFrame();
}

void turas::Engine::SystemsUpdate() {
    ZoneScoped;
    for(auto& sys : m_EngineSubSystems)
    {
        for(auto& scene : m_ActiveScenes)
        {
            sys->OnUpdate(scene.get());
        }
    }
}

void turas::Engine::PendingScenes() {

}
