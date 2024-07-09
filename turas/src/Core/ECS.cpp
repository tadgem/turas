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
    Vector<AssetLoadInfo> assetsToLoad {};

    for(auto& sys : Engine::INSTANCE->m_EngineSubSystems)
    {
        std::stringstream       stream;
        BinaryOutputArchive     archive(stream);
        sys->SerializeSceneBinary((Scene *) this, archive);
        serialized.emplace(sys->m_Hash, stream.str());

        Vector<AssetHandle> handles = sys->GetRequiredAssets((Scene*) this);
        for(auto& handle : handles)
        {
            Asset* a = Engine::INSTANCE->m_AssetManager.GetAsset(handle);
            AssetLoadInfo loadInfo = {a->m_Path, handle.m_Type};

            if(std::find(assetsToLoad.begin(), assetsToLoad.end(), loadInfo) == assetsToLoad.end())
            {
                assetsToLoad.push_back(loadInfo);
            }
        }
    }

    // emplace asset entry
    {
        std::stringstream       stream;
        BinaryOutputArchive     archive(stream);
        archive(assetsToLoad);
        serialized.emplace(Utils::Hash(p_AssetsHashName), stream.str());
    }

    return serialized;
}

void turas::Scene::LoadBinary(turas::HashMap<u64, turas::String>& sceneData) {
    ZoneScoped;
    
    {
        u64 assetsHash = Utils::Hash(p_AssetsHashName);
        std::stringstream loadInfoStream;
        loadInfoStream << sceneData[assetsHash];
        BinaryInputArchive input(loadInfoStream);

        Vector<AssetLoadInfo> assetsToLoad{};
        input(assetsToLoad);

        for(auto& info : assetsToLoad)
        {
            Engine::INSTANCE->m_AssetManager.LoadAsset(info.m_Path, info.m_Type);
        }
    }

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

