#pragma once
#include <type_traits>
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "Core/System.h"
#include "Assets/AssetManager.h"
#include "Debug/Profile.h"
#include "Rendering/Renderer.h"
#include "Im3D/im3d_lvk.h"
#include "VulkanAPI_SDL.h"
#include "efsw/efsw.hpp"

namespace turas
{
    class Scene;

    class TurasFilesystemListener : public efsw::FileWatchListener
    {
    public:
        void handleFileAction( efsw::WatchID watchid, const std::string& dir,
                               const std::string& filename, efsw::Action action,
                               std::string oldFilename ) override;
    };

    class Engine
    {
    public:
        Engine(bool enableDebugUpdate = true);

        void Init();
        void Shutdown();
        void Run();

        Scene*              CreateScene(const String& name);
        Scene*              LoadScene(BinaryInputArchive& archive);
        void                CloseScene(Scene* scene);
        void                CloseAllScenes();
        AssetLoadProgress   GetSceneLoadProgress(Scene* scene);

        TURAS_IMPL_ALLOC(Engine)

        inline static Engine* INSTANCE = nullptr;

        // Subsystems of the engine, used to have ProgramComponents and Systems, roll them into a system
        Vector<UPtr<System>>    m_EngineSubSystems;

        // Collection of all running scenes, each ECS in this collection will be processed + rendered each frame
        Vector<UPtr<Scene>>     m_ActiveScenes;

        // Scenes not yet loaded due to remaining asset load tasks
        Vector<Scene*>          m_PendingScenes;

        // main service for retrieving data from disk
        AssetManager            m_AssetManager;

        // Renders all active views & pipelines
        Renderer                m_Renderer;

        // backend for IM3D
        lvk::LvkIm3dState       m_Im3dState;

        UPtr<efsw::FileWatcher>         m_FileWatcher;
        UPtr<TurasFilesystemListener>   m_UpdateListener;
        efsw::WatchID                   m_GlobalProjectWatchId;

        template<typename _Ty, typename ... Args>
        _Ty* AddSystem(Args &&... args)
        {
            ZoneScoped;
            static_assert(std::is_base_of<System, _Ty>());
            return static_cast<_Ty*>(m_EngineSubSystems.emplace_back(std::move(CreateUnique<_Ty>(std::forward<Args>(args)...))).get());
        }

        // Internal update loop, public for test: skoosh
        void PrepFrame();
        void SubmitFrame();
        void SystemsUpdate();
        void PendingScenes();
    protected:

        bool p_DebugUpdateEnabled;

        void DebugUpdate();
#ifdef TURAS_ENABLE_MEMORY_TRACKING
        DebugMemoryTracker p_DebugMemoryTracker;
#endif

    };
}