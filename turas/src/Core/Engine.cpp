#include "STL/Memory.h"
#include "Core/Engine.h"
#include "Core/ECS.h"
#include "Core/Log.h"
#include "spdlog/spdlog.h"
#include "Debug/StatsWindow.h"

#ifdef WIN32

#include "windows.h"

#endif

turas::Engine::Engine(bool enableDebugUpdate) : p_DebugUpdateEnabled(enableDebugUpdate) {
    ZoneScoped;
    INSTANCE = this;
    p_OriginalWorkingDir = std::filesystem::current_path().string();
    ChangeWorkingDirectory("../");
    log::info("Turas : Working Directory : {}", std::filesystem::current_path().string());
}

void turas::Engine::Init() {
    ZoneScoped;
    spdlog::info("Initialising Turas");
    m_Renderer.Start();
    m_Im3dState = lvk::LoadIm3D(m_Renderer.m_VK);
    DebugInit();
    for (auto &sys: m_EngineSubSystems) {
        sys->OnEngineReady();
    }
}

void turas::Engine::Shutdown() {
    ZoneScoped;

    if (p_DebugUpdateEnabled) {
        m_FileWatcher->removeWatch(m_GlobalProjectWatchId);
        // remove other watch sources;
    }

    CloseAllScenes();

    for (auto &scene: m_ActiveScenes) {
        scene.reset();
    }
    m_ActiveScenes.clear();
    for (auto &sys: m_EngineSubSystems) {
        sys->OnShutdown();
        sys.reset();
    }
    m_EngineSubSystems.clear();
    m_AssetManager.Shutdown();

    lvk::FreeIm3d(m_Renderer.m_VK, m_Im3dState);
    m_Renderer.Shutdown();
    INSTANCE = nullptr;

    ChangeWorkingDirectory(p_OriginalWorkingDir);
}

void turas::Engine::Run() {
    while (m_Renderer.m_VK.ShouldRun()) {
        FrameMark;
        ZoneScopedN("Frame");
        PrepFrame();
        m_AssetManager.OnUpdate();
        PendingScenes();
        SystemsUpdate();
        DebugUpdate();
        SubmitFrame();
    }
}

turas::Scene *turas::Engine::CreateScene(const String &name) {
    ZoneScoped;
    Scene *scene = m_ActiveScenes.emplace_back(std::move(CreateUnique<Scene>(name))).get();
    for (auto &sys: m_EngineSubSystems) {
        sys->OnSceneLoaded(scene);
    }
    return scene;
}

void turas::Engine::CloseScene(turas::Scene *scene) {
    ZoneScoped;
    int index = -1;
    for (int i = 0; i < m_ActiveScenes.size(); i++) {
        if (m_ActiveScenes[i].get() == scene) {
            index = i;
        }
    }

    if (index >= 0) {
        for (auto &sys: m_EngineSubSystems) {
            sys->OnSceneClosed(m_ActiveScenes[index].get());
        }
        m_ActiveScenes[index].reset();
        m_ActiveScenes.erase(m_ActiveScenes.begin() + index);
    }

}

void turas::Engine::CloseAllScenes() {
    ZoneScoped;
    for (auto &scene: m_ActiveScenes) {
        // sys scene cleanup logic
        for (auto &sys: m_EngineSubSystems) {
            sys->OnSceneClosed(scene.get());
        }
        scene.reset();
    }

    m_ActiveScenes.clear();

}

void turas::Engine::PrepFrame() {
    ZoneScoped;
    m_Renderer.PreFrame();
    Im3d::NewFrame();
}

void turas::Engine::SubmitFrame() {
    ZoneScoped;
    Im3d::EndFrame();
    m_Renderer.PostFrame();
}

void turas::Engine::SystemsUpdate() {
    ZoneScoped;
    for (auto &sys: m_EngineSubSystems) {
        for (auto &scene: m_ActiveScenes) {
            sys->OnUpdate(scene.get());
        }
    }
}

void turas::Engine::PendingScenes() {
    if (m_AssetManager.AnyAssetsLoading()) {
        return;
    }

    if (m_PendingScenes.empty()) {
        return;
    }

    for (auto *pendingScene: m_PendingScenes) {
        // Turn raw scene ptr to a UPtr so it will be
        // freed when the scene is removed from list of active scenes
        m_ActiveScenes.push_back(UPtr<Scene>(pendingScene));
        for (auto &sys: m_EngineSubSystems) {
            sys->OnSceneLoaded(pendingScene);
        }
    }
    m_PendingScenes.clear();
}

turas::Scene *turas::Engine::LoadScene(turas::BinaryInputArchive &archive) {
    auto *s = new Scene("empty");
    s->LoadBinaryFromArchive(archive);
    m_PendingScenes.push_back(s);
    return s;
}

turas::AssetLoadProgress turas::Engine::GetSceneLoadProgress(turas::Scene *scene) {
    if (std::find(m_PendingScenes.begin(), m_PendingScenes.end(), scene) != m_PendingScenes.end()) {
        return AssetLoadProgress::Loading;
    }

    for (auto &activeScene: m_ActiveScenes) {
        if (activeScene.get() == scene) {
            return AssetLoadProgress::Loaded;
        }
    }

    return turas::AssetLoadProgress::NotLoaded;
}

void turas::Engine::DebugUpdate() {
    if (!p_DebugUpdateEnabled) {
        return;
    }

    m_Renderer.OnImGui();
    StatsWindow::OnImGuiStatsWindow(m_Renderer.m_VK);
}

void turas::Engine::DebugInit() {
    if (!p_DebugUpdateEnabled) {
        return;
    }
    m_FileWatcher = CreateUnique<efsw::FileWatcher>();
    m_UpdateListener = CreateUnique<TurasFilesystemListener>();
    m_GlobalProjectWatchId = m_FileWatcher->addWatch(".", m_UpdateListener.get(), true);
    // Perhaps other sources of watch?
    m_FileWatcher->watch();
}

bool turas::Engine::LoadProject(const turas::String &path) {
    if (m_Project.get() != nullptr) {
        return false;
    }
    // Deserialize project binary
    // change working directory to project dir
    // if debug copy shaders to directory
    if (p_DebugUpdateEnabled) {
        CopyShadersToProject();
    }
    // update renderer to reload all shaders
    // load default scene
    return true;
}

bool turas::Engine::CreateProject(const turas::String &name, const turas::String &projectDir) {
    if (m_Project.get() != nullptr) {
        return false;
    }
    m_Project = CreateUnique<Project>(name);
    // change working directory to projectDir
    ChangeWorkingDirectory(projectDir);
    // save project to specified path
    SaveProject();
    // if debug copy shaders to directory
    if (p_DebugUpdateEnabled) {
        CopyShadersToProject();
    }
    // update renderer to reload all shaders
    // create default scene
    return true;
}

bool turas::Engine::SaveProject() {
    if (m_Project.get() == nullptr) {
        return false;
    }

    return true;
}

void turas::Engine::ChangeWorkingDirectory(const turas::String &newDirectory) {
    std::filesystem::current_path(newDirectory);
}

void turas::Engine::CopyShadersToProject() {

}

void turas::TurasFilesystemListener::handleFileAction(efsw::WatchID watchid, const std::string &dir,
                                                      const std::string &filename, efsw::Action action,
                                                      std::string oldFilename) {
    switch (action) {
        case efsw::Actions::Add:
            std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Added"
                      << std::endl;
            break;
        case efsw::Actions::Delete:
            std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Delete"
                      << std::endl;
            break;
        case efsw::Actions::Modified:
            std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Modified"
                      << std::endl;
            break;
        case efsw::Actions::Moved:
            std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Moved from ("
                      << oldFilename << ")" << std::endl;
            break;
        default:
            std::cout << "Should never happen!" << std::endl;
    }
}