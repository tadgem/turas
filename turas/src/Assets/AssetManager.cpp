//
// Created by liam_ on 7/4/2024.
//
#include "Assets/AssetManager.h"
#include "Core/Utils.h"
#include <filesystem>
#include "lvk/VulkanAPI.h"
#include "Core/Log.h"


turas::Asset* LoadModel(const turas::String& path)
{
    int count = 0;
    for(int i = 0; i < INT32_MAX; i++)
    {
        turas::log::info("LoadModel: i : {}", i);
        count++;
    }

    return nullptr;
}

turas::AssetHandle turas::AssetManager::LoadAsset(const turas::String &path, const turas::AssetType &assetType) {
    AssetHandle handle (Utils::Hash(path), assetType);

    switch(assetType)
    {
        case AssetType::Mesh:
            log::error("AssetManager : Please load meshes as models : {}", path);
            break;
        case AssetType::Model:
            p_PendingLoads.emplace(handle, std::move(std::async(std::launch::async, LoadModel, path)));
            break;
        case AssetType::Texture:
            break;
        case AssetType::Audio:
            break;
        case AssetType::Text:
            break;
        case AssetType::Binary:
            break;
        default:
            break;
    }


    return handle;
}

void turas::AssetManager::UnloadAsset(const turas::AssetHandle &handle) {
    if(p_LoadedAssets.find(handle) == p_LoadedAssets.end())
    {
        return;
    }

    // do we want to defer deletion
    p_LoadedAssets[handle].reset();
    p_LoadedAssets.erase(handle);
}

turas::AssetLoadProgress turas::AssetManager::GetAssetLoadProgress(const turas::AssetHandle &handle) {
    if(p_LoadedAssets.find(handle) != p_LoadedAssets.end())
    {
        return AssetLoadProgress::Loaded;
    }

    if(p_PendingLoads.find(handle) != p_PendingLoads.end())
    {
        return AssetLoadProgress::Loading;
    }

    return AssetLoadProgress::NotLoaded;
}

void turas::AssetManager::OnUpdate() {
    if(p_PendingLoads.empty())
    {
        return;
    }

    Vector<AssetHandle> pending;
    for(auto& [handle, future] : p_PendingLoads) {
        if(IsReady(future))
        {
            pending.push_back(handle);
        }
    }

    for(auto& handle : pending) {
        Asset* asset = p_PendingLoads[handle].get();
        p_LoadedAssets.emplace(handle, std::move(UPtr<Asset>(asset)));
        p_PendingLoads.erase(handle);
    }
}

bool turas::AssetManager::AnyAssetsLoading() {
    return !p_PendingLoads.empty();
}
