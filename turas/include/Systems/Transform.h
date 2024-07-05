#ifndef TURAS_ALL_TRANSFORM_H
#define TURAS_ALL_TRANSFORM_H
#include "Core/System.h"
#include "STL/Memory.h"
#include "glm/glm.hpp"

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

        void OnEngineReady() override;
        void OnSceneLoaded(Scene *scene) override;
        void OnSceneClosed(Scene *scene) override;
        void OnUpdate(Scene *scene) override;
        void OnShutdown() override;
        void SerializeBinary(Scene* scene, BinaryOutputArchive& output) const override;

        TURAS_IMPL_ALLOC(TransformSystem)
        template<typename Archive>

        void save(Archive& ar) const {
            auto transform_view = GetSceneRegistry(s_CurrentSerializingScene).view<TransformComponent>();
            for(auto [ent, trans] : transform_view.each())
            {
                ar(static_cast<uint32_t>(ent), trans);
            }
        }

        template<typename Archive>
        void load(Archive& ar) {

            // ar( cereal::base_class<System>(this));
        }
    };
}
CEREAL_REGISTER_TYPE(turas::TransformSystem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, turas::TransformSystem)
#endif //TURAS_ALL_TRANSFORM_H
