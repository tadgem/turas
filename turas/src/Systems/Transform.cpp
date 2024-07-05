//
// Created by liam_ on 7/4/2024.
//
#include "Systems/Transform.h"
#include "Core/ECS.h"
#include "Core/Log.h"
#include "Core/Utils.h"


void turas::TransformSystem::OnEngineReady() {

}

void turas::TransformSystem::OnSceneLoaded(Scene *scene) {

}

void turas::TransformSystem::OnSceneClosed(Scene *scene) {

}

void turas::TransformSystem::OnUpdate(Scene *scene) {

}

void turas::TransformSystem::OnShutdown() {

}

turas::TransformSystem::TransformSystem() : System(GetTypeHash<TransformSystem>())
{
}

void turas::TransformSystem::SerializeBinary(turas::Scene *scene, turas::BinaryOutputArchive &output) const {
    s_CurrentSerializingScene = scene;
    output(*this);
    s_CurrentSerializingScene = nullptr;
}
