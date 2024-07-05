//
// Created by liam_ on 7/3/2024.
//
#include "Core/ECS.h"
#include "Core/Log.h"
turas::Scene::Scene() {
    p_Registry = entt::registry ();

}

turas::Entity turas::Scene::CreateEntity() {
    entt::entity handle = p_Registry.create();
    p_EntityCount++;
    return Entity {handle};
}

void turas::Scene::DestroyEntity(turas::Entity &e) {
    if(!p_Registry.destroy(e.m_Handle))
    {
        log::error("Scene : failed to destroy entity {}", e.operator uint32_t());
        return;
    }
    p_EntityCount--;
}

uint32_t turas::Scene::NumEntities() {
    return p_EntityCount;
}

turas::HashMap<uint64_t, turas::String> turas::Scene::SaveBinary() {
    HashMap<uint64_t, String> serialized {};
    for(auto& sys : Engine::INSTANCE->m_EngineSubSystems)
    {
        std::stringstream       stream;
        BinaryOutputArchive     archive(stream);
        sys->SerializeBinary((Scene*) this, archive);

        serialized.emplace(sys->m_Hash, stream.str());
    }
    return serialized;
}

void turas::Scene::LoadBinary(turas::HashMap<uint64_t, turas::String> sceneData)
{
    for(auto& sys : Engine::INSTANCE->m_EngineSubSystems) {

    }
}

