//
// Created by liam_ on 7/3/2024.
//
#include "Core/ECS.h"
#include "Core/Log.h"
turas::Scene::Scene() {
    ZoneScoped;
    p_Registry = entt::registry ();
}

turas::Entity turas::Scene::CreateEntity() {
    ZoneScoped;
    entt::entity handle = p_Registry.create();
    p_EntityCount++;
    return Entity {handle};
}

void turas::Scene::DestroyEntity(turas::Entity &e) {
    ZoneScoped;
    if(!p_Registry.destroy(e.m_Handle))
    {
        log::error("Scene : failed to destroy entity {}", e.operator u32());
        return;
    }
    p_EntityCount--;
}

turas::u32 turas::Scene::NumEntities() {
    ZoneScoped;
    return p_EntityCount;
}

turas::HashMap<turas::u64, turas::String> turas::Scene::SaveBinary() {
    ZoneScoped;
    HashMap<u64, String> serialized {};
    for(auto& sys : Engine::INSTANCE->m_EngineSubSystems)
    {
        std::stringstream       stream;
        BinaryOutputArchive     archive(stream);
        sys->SerializeSceneBinary((Scene *) this, archive);

        serialized.emplace(sys->m_Hash, stream.str());
    }
    return serialized;
}

void turas::Scene::LoadBinary(const turas::HashMap<u64, turas::String>& sceneData) {
    ZoneScoped;
    for(auto& sys : Engine::INSTANCE->m_EngineSubSystems) {
        for(auto& [hash, serialized_stream] : sceneData)
        {
            if(sys->m_Hash != hash) continue;
            std::stringstream stream {};
            stream << serialized_stream;
            BinaryInputArchive input(stream);
            sys->DeserializeSceneBinary((Scene *) this, input);
        }

    }
}

