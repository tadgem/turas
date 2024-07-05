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

        TURAS_IMPL_ALLOC(TransformSystem)

        template<typename Archive>
        void serialize(Archive& ar) {
            ar( cereal::base_class<System>(this));
        }
    };
}
#endif //TURAS_ALL_TRANSFORM_H
