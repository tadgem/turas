#pragma once
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "Core/System.h"
#include "Core/Scene.h"
#include "Assets/AssetManager.h"

namespace turas
{
    class Engine
    {
    public:
        Engine();

        void Init();
        void Shutdown();

        TURAS_IMPL_ALLOC(Engine)

        // Subsystems of the engine, used to have ProgramComponents and Systems, roll them into a system
        Vector<UPtr<System>>    m_EngineSubSystems;

        // Collection of all running scenes, each Scene in this collection will be processed + rendered each frame
        Vector<UPtr<Scene>>     m_ActiveScenes;

        // main service for retrieving data from disk
        AssetManager            m_AssetManager;


    protected:
        DebugMemoryTracker p_DebugMemoryTracker;
    };
}