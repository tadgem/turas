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
}

void turas::Engine::Shutdown() {
    for(auto& scene : m_ActiveScenes)
    {
        scene.reset();
    }
    m_ActiveScenes.clear();
    for(auto& sys : m_EngineSubSystems)
    {
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

        StatsWindow::OnImGuiStatsWindow(m_VK);

        Im3d::EndFrame();
        m_VK.PostFrame();
    }
    lvk::FreeIm3d(m_VK, m_Im3dState);
}
