//
// Created by liam_ on 7/10/2024.
//
#include "Systems/Camera.h"
#include "Core/Utils.h"
turas::CameraSystem::CameraSystem() : System(GetTypeHash<CameraSystem>())
{
    ZoneScoped;
}

void turas::CameraSystem::OnEngineReady() {
    ZoneScoped;
}

void turas::CameraSystem::OnSceneLoaded(turas::Scene *scene) {
    ZoneScoped;
}

void turas::CameraSystem::OnSceneClosed(turas::Scene *scene) {
    ZoneScoped;
}

void turas::CameraSystem::OnUpdate(turas::Scene *scene) {
    ZoneScoped;
}

void turas::CameraSystem::OnShutdown() {
    ZoneScoped;
}

void turas::CameraSystem::SerializeSceneBinary(turas::Scene *scene, turas::BinaryOutputArchive &output) const {
    ZoneScoped;
    s_CurrentSerializingScene = scene;
    output(*this);
    s_CurrentSerializingScene = nullptr;
}

void turas::CameraSystem::DeserializeSceneBinary(turas::Scene *scene, turas::BinaryInputArchive &input) {
    ZoneScoped;
    s_CurrentSerializingScene = scene;
    input(*this);
    s_CurrentSerializingScene = nullptr;
}

turas::Vector<turas::AssetHandle> turas::CameraSystem::GetRequiredAssets(turas::Scene *scene) {
    ZoneScoped;
    return {};
}
