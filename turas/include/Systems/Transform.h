#ifndef TURAS_ALL_TRANSFORM_H
#define TURAS_ALL_TRANSFORM_H
#include "Core/System.h"
#include "Core/ECS.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glm/glm.hpp"
#include "Debug/Profile.h"

namespace turas
{
    struct TransformComponent
    {
        glm::vec3 m_Position;
        glm::vec3 m_Rotation; // euler angles
        glm::vec3 m_Scale;

        glm::mat4 m_ModelMatrix;

        template<typename Archive>
        void serialize(Archive& ar)
        {
            ZoneScoped;
            ar(
                m_Position.x, m_Position.y, m_Position.z,
                m_Rotation.x, m_Rotation.y, m_Rotation.z,
                m_Scale.x, m_Scale.y, m_Scale.z
            );
        }

        TURAS_IMPL_ALLOC(TransformComponent)
    };

    class TransformSystem : public System
    {
    public:
        TransformSystem();

        void                    OnEngineReady() override;
        void                    OnSceneLoaded(Scene *scene) override;
        void                    OnSceneClosed(Scene *scene) override;
        void                    OnUpdate(Scene *scene) override;
        void                    OnShutdown() override;
        void                    SerializeSceneBinary(Scene* scene, BinaryOutputArchive& output) const override;
        void                    DeserializeSceneBinary(Scene* scene, BinaryInputArchive& input) override;
        Vector<AssetHandle>     GetRequiredAssets() override;

        TURAS_IMPL_ALLOC(TransformSystem)

        template<typename Archive>
        void save(Archive& ar) const {
            ZoneScoped;
            auto transform_view = GetSceneRegistry(s_CurrentSerializingScene).view<TransformComponent>();
            HashMap<u32, TransformComponent> transforms {};
            for(auto [ent, trans] : transform_view.each())
            {
                transforms.emplace(static_cast<u32>(ent), trans);
            }
            ar(transforms);
        }

        template<typename Archive>
        void load(Archive& ar) {
            ZoneScoped;
            HashMap<u32, TransformComponent> transforms {};
            ar(transforms);
            auto& reg = GetSceneRegistry(s_CurrentSerializingScene);
            for(auto& [handle, trans] : transforms)
            {
                auto ent = entt::entity(handle);
                if(!reg.valid(ent)) {
                    ent = reg.create(ent);
                }
                Entity e {ent};
                s_CurrentSerializingScene->AddComponent<TransformComponent>(e, trans);
            }
        }

    };
}
CEREAL_REGISTER_TYPE(turas::TransformSystem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, turas::TransformSystem)
#endif //TURAS_ALL_TRANSFORM_H
