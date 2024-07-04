#pragma once
#include "Core/ECS.h"
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

        // outside of class scope add EXPAND(TURAS_SYSTEM_SERIALIZATION_IMPL(NameOfYourSystem))
    };

}
