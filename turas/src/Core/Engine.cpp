#include "STL/Memory.h"
#include "Core/Engine.h"
#include "spdlog/spdlog.h"
#include "Debug/StatsWindow.h"

turas::Engine::Engine() {

}

void turas::Engine::Init() {
    spdlog::info("Initialising Turas");
    m_VK.Start("Turas", 1280, 720);
    m_Im3dState = lvk::LoadIm3D(m_VK);
    for(auto& sys : m_EngineSubSystems)
    {
        sys->OnEngineReady();
    }
}

void turas::Engine::Shutdown() {

    CloseAllScenes();
    lvk::FreeIm3d(m_VK, m_Im3dState);
    m_VK.Quit();

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
}

void turas::Engine::Run()
{
    while(m_VK.ShouldRun())
    {
        m_VK.PreFrame();
        Im3d::NewFrame();

        for(auto& sys : m_EngineSubSystems)
        {
            for(auto& scene : m_ActiveScenes)
            {
                sys->OnUpdate(scene.get());
            }
        }

        StatsWindow::OnImGuiStatsWindow(m_VK);

        Im3d::EndFrame();
        m_VK.PostFrame();
    }
}

turas::Scene *turas::Engine::CreateScene() {
    Scene* scene =  m_ActiveScenes.emplace_back(std::move(CreateUnique<Scene>())).get();
    for(auto& sys : m_EngineSubSystems)
    {
        sys->OnSceneLoaded(scene);
    }
    return scene;
}

void turas::Engine::CloseScene(turas::Scene *scene) {
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
