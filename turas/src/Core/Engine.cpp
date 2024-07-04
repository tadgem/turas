#include "STL/Memory.h"
#include "Core/Engine.h"
#include "spdlog/spdlog.h"

turas::Engine::Engine() {
}

void turas::Engine::Init() {
    spdlog::info("Initialising Turas");

    m_ActiveScenes.push_back(std::move(CreateUnique<Scene>()));
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
