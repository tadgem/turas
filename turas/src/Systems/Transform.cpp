//
// Created by liam_ on 7/4/2024.
//
#include "Systems/Transform.h"
#include "Core/ECS.h"
#include "Core/Log.h"
#include "Core/Utils.h"


void turas::TransformSystem::OnEngineReady() {
    ZoneScoped;
}

void turas::TransformSystem::OnSceneLoaded(Scene *scene) {
    ZoneScoped;

}

void turas::TransformSystem::OnSceneClosed(Scene *scene) {
    ZoneScoped;
}

void turas::TransformSystem::OnUpdate(Scene *scene) {
    ZoneScoped;
    auto view = GetSceneRegistry(scene).view<TransformComponent>();

    for(auto [ent, t] : view.each())
    {
        t.m_ModelMatrix = glm::translate(glm::mat4(1), t.m_Position);
        glm::vec3 radians = glm::radians(t.m_Rotation);
        t.m_ModelMatrix *= glm::mat4_cast(glm::quat(radians));
        t.m_ModelMatrix = glm::scale(t.m_ModelMatrix, t.m_Scale);
    }
}

void turas::TransformSystem::OnShutdown() {
    ZoneScoped;
}

turas::TransformSystem::TransformSystem() : System(GetTypeHash<TransformSystem>())
{
    ZoneScoped;
}

void turas::TransformSystem::SerializeBinary(turas::Scene *scene, turas::BinaryOutputArchive &output) const {
    ZoneScoped;
    s_CurrentSerializingScene = scene;
    output(*this);
    s_CurrentSerializingScene = nullptr;
}

void turas::TransformSystem::DeserializeBinary(turas::Scene *scene, turas::BinaryInputArchive &input)
{
    ZoneScoped;
    s_CurrentSerializingScene = scene;
    input(*this);
    s_CurrentSerializingScene = nullptr;
}
