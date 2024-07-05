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
