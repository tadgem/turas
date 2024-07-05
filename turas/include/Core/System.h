#pragma once
#include "Core/Serialization.h"
#include "entt/entt.hpp"

#define TURAS_SYSTEM_SERIALIZATION_IMPL(X) DEFER(CEREAL_REGISTER_TYPE(X)); \
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, X)

namespace turas
{
    class Scene;
    class System
    {
    public:
        const uint64_t  m_Hash;

        System(uint64_t hash) : m_Hash(hash) {};

        virtual void OnEngineReady() = 0;
        virtual void OnSceneLoaded(Scene* scene) = 0;
        virtual void OnSceneClosed(Scene* scene) = 0;
        virtual void OnUpdate(Scene* scene) = 0;
        virtual void OnShutdown() = 0;
        virtual void SerializeBinary(Scene* scene, BinaryOutputArchive& output) const = 0;

        static entt::registry& GetSceneRegistry(Scene* scene);

    protected:
        inline static Scene*  s_CurrentSerializingScene = nullptr;
        // outside of class scope add EXPAND(TURAS_SYSTEM_SERIALIZATION_IMPL(NameOfYourSystem))
    };

}
