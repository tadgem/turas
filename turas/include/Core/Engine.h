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

namespace turas
{
    class Scene;
    class Engine
    {
    public:
        Engine();

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

#ifdef TURAS_ENABLE_MEMORY_TRACKING
        DebugMemoryTracker p_DebugMemoryTracker;
#endif

    };
}