#pragma once
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "Core/System.h"
#include "Core/ECS.h"
#include "Assets/AssetManager.h"
#include "VulkanAPI_SDL.h"
#include "Im3D/im3d_lvk.h"
#include <type_traits>

namespace turas
{
    class Engine
    {
    public:
        Engine();

        void Init();
        void Shutdown();
        void Run();

        Scene*  CreateScene();
        void    CloseScene(Scene* scene);
        void    CloseAllScenes();

        TURAS_IMPL_ALLOC(Engine)

        // Subsystems of the engine, used to have ProgramComponents and Systems, roll them into a system
        Vector<UPtr<System>>    m_EngineSubSystems;

        // Collection of all running scenes, each ECS in this collection will be processed + rendered each frame
        Vector<UPtr<Scene>>     m_ActiveScenes;

        // main service for retrieving data from disk
        AssetManager            m_AssetManager;

        // interface to GPU (vulkan)
        lvk::VulkanAPI_SDL      m_VK;

        // backend for IM3D
        lvk::LvkIm3dState       m_Im3dState;

        template<typename _Ty, typename ... Args>
        _Ty* AddSystem(Args &&... args)
        {
            static_assert(std::is_base_of<System, _Ty>());
            return static_cast<_Ty*>(m_EngineSubSystems.emplace_back(std::move(CreateUnique<_Ty>(std::forward<Args>(args)...))).get());
        }

    protected:
#ifdef TURAS_ENABLE_MEMORY_TRACKING
        DebugMemoryTracker p_DebugMemoryTracker;
#endif
    };
}