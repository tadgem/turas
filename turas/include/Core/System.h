#pragma once
#include "Core/Scene.h"
#include "Core/Serialization.h"

#define TURAS_SYSTEM_SERIALIZATION_IMPL(X) DEFER(CEREAL_REGISTER_TYPE(X)); \
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, X)

namespace turas
{
    class System
    {
    public:
        virtual void OnEngineReady() = 0;
        virtual void OnSceneLoaded(Scene* scene) = 0;
        virtual void OnSceneClosed(Scene* scene) = 0;
        virtual void OnUpdate(Scene* scene) = 0;
        virtual void OnShutdown() = 0;

        // TURAS_SYSTEM_SERIALIZATION_IMPL(System)
    };

    class DummySystem : public System
    {
    public:
    };
}

EXPAND(TURAS_SYSTEM_SERIALIZATION_IMPL(turas::DummySystem))