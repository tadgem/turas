#pragma once
#include "Core/Serialization.h"
#include "entt/entt.hpp"
#include "Core/Types.h"

#define TURAS_SYSTEM_SERIALIZATION_IMPL(X) DEFER(CEREAL_REGISTER_TYPE(X)); \
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, X)

namespace turas
{
    class Scene;
    class System
    {
    public:
        const u64  m_Hash;

        System(u64 hash) : m_Hash(hash) {};

        virtual void OnEngineReady() = 0;
        virtual void OnSceneLoaded(Scene* scene) = 0;
        virtual void OnSceneClosed(Scene* scene) = 0;
        virtual void OnUpdate(Scene* scene) = 0;
        virtual void OnShutdown() = 0;
        virtual void SerializeBinary(Scene* scene, BinaryOutputArchive& output) const = 0;
        virtual void DeserializeBinary(Scene* scene, BinaryInputArchive& output) = 0;

        static entt::registry& GetSceneRegistry(Scene* scene);

    protected:
        inline static Scene*  s_CurrentSerializingScene = nullptr;
        // outside of class scope add EXPAND(TURAS_SYSTEM_SERIALIZATION_IMPL(NameOfYourSystem))
    };

}
