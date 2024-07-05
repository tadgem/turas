#pragma once
#include "Core/Serialization.h"

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

        template<typename Archive>
        void serialize(Archive& ar) {
            ar(m_Hash);
        }

        // outside of class scope add EXPAND(TURAS_SYSTEM_SERIALIZATION_IMPL(NameOfYourSystem))
    };

}
