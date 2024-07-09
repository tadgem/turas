//
// Created by liam_ on 7/4/2024.
//
#include "Systems/Mesh.h"
#include "Core/Utils.h"
#include "Core/ECS.h"

void turas::MeshSystem::OnEngineReady() {
    ZoneScoped;
}

void turas::MeshSystem::OnSceneLoaded(Scene *scene) {
    ZoneScoped;
    auto& reg = GetSceneRegistry(scene);
    reg.on_construct<MeshComponent>().connect<&MeshSystem::OnMeshComponentAdded>(this);
}

void turas::MeshSystem::OnSceneClosed(Scene *scene) {
    ZoneScoped;
}

void turas::MeshSystem::OnUpdate(Scene *scene) {
    ZoneScoped;
    auto view = GetSceneRegistry(scene).view<MeshComponent>();

    for(auto [ent, t] : view.each())
    {
    }
}

void turas::MeshSystem::OnShutdown() {
    ZoneScoped;
}

turas::MeshSystem::MeshSystem() : System(GetTypeHash<MeshSystem>())
{
    ZoneScoped;
}

void turas::MeshSystem::SerializeSceneBinary(Scene *scene, BinaryOutputArchive &output) const {
    ZoneScoped;
    s_CurrentSerializingScene = scene;
    output(*this);
    s_CurrentSerializingScene = nullptr;
}

void turas::MeshSystem::DeserializeSceneBinary(Scene *scene, BinaryInputArchive &input)
{
    ZoneScoped;
    s_CurrentSerializingScene = scene;
    input(*this);
    s_CurrentSerializingScene = nullptr;
}

turas::Vector<turas::AssetHandle> turas::MeshSystem::GetRequiredAssets(Scene* scene) {

    Vector<AssetHandle> handles{};

    auto meshView = GetSceneRegistry(scene).view<MeshComponent>();

    for(const auto& [e, mesh] : meshView.each())
    {
        handles.push_back(mesh.m_Handle);
    }

    return handles;
}

void turas::MeshSystem::OnMeshComponentAdded(entt::registry &reg, entt::entity e) {

    MeshComponent& mesh = reg.get<MeshComponent>(e);
    if(mesh.m_LvkMesh == nullptr)
    {
        auto* asset = Engine::INSTANCE->m_AssetManager.GetAsset(mesh.m_Handle);
        auto* meshAsset = reinterpret_cast<ModelAsset*>(asset);

        if(mesh.m_EntryIndex >= meshAsset->m_Entries.size())
        {
            return;
        }

        mesh.m_LvkMesh = meshAsset->m_Entries[mesh.m_EntryIndex].m_Mesh.m_LvkMesh;
    }

}

turas::MeshComponent::MeshComponent(const turas::AssetHandle &handle, turas::u32 index) : m_Handle(handle), m_EntryIndex(index)
{

}
