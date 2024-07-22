#pragma once

#include "Assets/AssetManager.h"
#include "Core/Project.h"
#include "Core/System.h"
#include "Debug/Profile.h"
#include "Im3D/im3d_lvk.h"
#include "Rendering/Renderer.h"
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "VulkanAPI_SDL.h"
#include "efsw/efsw.hpp"
#include <type_traits>

namespace turas
{
  class Scene;

  class TurasFilesystemListener : public efsw::FileWatchListener
  {
  public:
    void handleFileAction(efsw::WatchID watchid, const std::string& dir,
                          const std::string& filename, efsw::Action action,
                          std::string oldFilename) override;
  };

  class Engine
  {
  public:
    Engine(bool enableDebugUpdate = false);

    void Init();

    void Shutdown();

    void Run();

    bool LoadProject(const String& path);

    bool CreateProject(const String& name, const String& projectDir);

    bool SaveProject();

    Scene* CreateScene(const String& name);

    Scene* LoadSceneFromArchive(BinaryInputArchive& archive);

    Scene* LoadSceneFromPath(const String& path);

    Scene* LoadSceneFromName(const String& name);

    bool SaveScene(Scene* s);

    void CloseScene(Scene* scene);

    void CloseAllScenes();

    AssetLoadProgress GetSceneLoadProgress(Scene* scene);

    TURAS_IMPL_ALLOC(Engine)

    inline static Engine* INSTANCE = nullptr;

    // Subsystems of the engine, used to have ProgramComponents and Systems, roll
    // them into a system
    Vector<UPtr<System>> m_EngineSubSystems;
    // Collection of all running scenes, each ECS in this collection will be
    // processed + rendered each frame
    Vector<UPtr<Scene>> m_ActiveScenes;
    // Scenes not yet loaded due to remaining asset load tasks
    Vector<Scene*> m_PendingScenes;
    // main service for retrieving data from disk
    AssetManager m_AssetManager;
    // Renders all active views & pipelines
    Renderer m_Renderer;
    // backend for IM3D
    lvk::LvkIm3dState m_Im3dState;
    // name + serialized scene info. Working directory is project file directory
    UPtr<Project> m_Project;

    // File watchers should be p_DebugUpdateEnabled only
    UPtr<efsw::FileWatcher> m_FileWatcher;
    UPtr<TurasFilesystemListener> m_UpdateListener;
    efsw::WatchID m_GlobalProjectWatchId;

    template <typename _Ty, typename... Args>
    _Ty* AddSystem(Args&&... args)
    {
      ZoneScoped;
      static_assert(std::is_base_of<System, _Ty>());
      return static_cast<_Ty*>(m_EngineSubSystems
                               .emplace_back(std::move(CreateUnique<_Ty>(
                                 std::forward<Args>(args)...)))
                               .get());
    }

    // Internal update loop, public for test: skoosh
    void PrepFrame();

    void SubmitFrame();

    void SystemsUpdate();

    void PendingScenes();

    inline bool IsDebugEnabled() { return p_DebugUpdateEnabled; }

  protected:
    void ChangeWorkingDirectory(const String& newDirectory);

    void CopyShadersToProject();

    bool p_DebugUpdateEnabled;
    String p_OriginalWorkingDir;

    void InitImGuiStyle();

    void DebugInit();

    void DebugUpdate();

#ifdef TURAS_ENABLE_MEMORY_TRACKING
    DebugMemoryTracker p_DebugMemoryTracker;
#endif
  };
} // namespace turas
