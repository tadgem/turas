//
// Created by liam_ on 7/3/2024.
//
#include "Core/ECS.h"
#include "Core/Log.h"
#include "Core/Engine.h"

turas::Scene::Scene(const String &name) : m_Name(name) {
    ZoneScoped;
    p_Registry = entt::registry();
}

turas::Entity turas::Scene::CreateEntity() {
    ZoneScoped;
    entt::entity handle = p_Registry.create();
    p_EntityCount++;
    return Entity{handle};
}

void turas::Scene::DestroyEntity(turas::Entity &e) {
    ZoneScoped;
    if (!p_Registry.destroy(e.m_Handle)) {
        log::error("Scene : failed to destroy entity {}", e.operator u32());
        return;
    }
    p_EntityCount--;
}

turas::u32 turas::Scene::NumEntities() {
    ZoneScoped;
    return p_EntityCount;
}

turas::HashMap<turas::u64, turas::String> turas::Scene::SaveBinary()
{
	ZoneScoped;
	HashMap<u64, String>  serialized{};
	Vector<AssetLoadInfo> assets_to_load{};
	for (auto& sys : Engine::INSTANCE->m_EngineSubSystems) {
		std::stringstream	stream;
		BinaryOutputArchive archive(stream);
		sys->SerializeSceneBinary((Scene*)this, archive);
		serialized.emplace(sys->m_Hash, stream.str());
		Vector<AssetHandle> handles = sys->GetRequiredAssets((Scene*)this);
		for (auto& handle : handles) {
			Asset*		  a		   = Engine::INSTANCE->m_AssetManager.GetAsset(handle);
			AssetLoadInfo loadInfo = {a->m_Path, handle.m_Type};
			if (std::find(assets_to_load.begin(), assets_to_load.end(), loadInfo) == assets_to_load.end()) {
				assets_to_load.push_back(loadInfo);
			}
		}
	}
	{
		// emplace name entry
		serialized.emplace(Utils::Hash(p_SceneHashName), m_Name);
	}
	// emplace asset entry
	{
		std::stringstream	stream;
		BinaryOutputArchive archive(stream);
		archive(assets_to_load);
		serialized.emplace(Utils::Hash(p_AssetsHashName), stream.str());
	}
	return serialized;
}

turas::Vector<turas::AssetHandle> turas::Scene::GetRequiredAssets()
{
	Vector<AssetHandle> required_assets{};
	for (auto& sys : Engine::INSTANCE->m_EngineSubSystems) {
		sys->GetRequiredAssets((Scene*)this);
		Vector<AssetHandle> handles = sys->GetRequiredAssets((Scene*)this);
		for (auto& handle : handles) {
			if (std::find(required_assets.begin(), required_assets.end(), handle) == required_assets.end()) {
				required_assets.push_back(handle);
			}
		}
	}
	return required_assets;
}

void turas::Scene::LoadBinaryFromArchive(turas::BinaryInputArchive &scene_data) {
    ZoneScoped;
    HashMap<u64, turas::String> data{};
    scene_data(data);
    LoadBinary(data);
}

void turas::Scene::LoadBinary(turas::HashMap<u64, turas::String> &scene_data) {
    ZoneScoped;

    {
        u64 assets_hash = Utils::Hash(p_AssetsHashName);
        std::stringstream load_info_stream;
        load_info_stream << scene_data[assets_hash];
        BinaryInputArchive input(load_info_stream);

        Vector<AssetLoadInfo> assets_to_load{};
        input(assets_to_load);

        for (auto &info: assets_to_load) {
            Engine::INSTANCE->m_AssetManager.LoadAsset(info.m_Path, info.m_Type);
        }
    }

    {
        u64 name_hash = Utils::Hash(p_SceneHashName);
        m_Name = scene_data[name_hash];
    }

    for (auto &sys: Engine::INSTANCE->m_EngineSubSystems) {
        for (auto &[hash, serialized_stream]: scene_data) {
            if (sys->m_Hash != hash) continue;
            std::stringstream stream{};
            stream << serialized_stream;
            BinaryInputArchive input(stream);
            sys->DeserializeSceneBinary((Scene *) this, input);
        }

    }
}

