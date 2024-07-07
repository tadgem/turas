//
// Created by liam_ on 7/4/2024.
//
#include "Assets/AssetManager.h"
#include "Core/Utils.h"
#include <filesystem>
#include "lvk/VulkanAPI.h"
#include "Core/Log.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/cimport.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "lvk/Texture.h"
#include "Core/Engine.h"

void ProcessNode(const aiScene* assimpScene, aiNode* currentNode, turas::ModelAsset* model)
{

}

turas::AssetLoadReturn LoadModel(const turas::String& path)
{
    turas::log::info("LoadModel : Loading Model : {}", path);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(),
                                             aiProcess_Triangulate |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_OptimizeMeshes |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_OptimizeGraph |
                                             aiProcess_FixInfacingNormals |
                                             aiProcess_FindInvalidData |
                                             aiProcess_GenBoundingBoxes
    );

    if(scene == nullptr)
    {
        turas::log::error("LoadModel : Failed to open model at path : {}", path);
        return {nullptr, {}};
    }
    auto* model = new turas::ModelAsset(
            path, turas::AssetHandle(turas::Utils::Hash(path), turas::AssetType::Model));

    ProcessNode( scene, scene->mRootNode, model);

    turas::log::info("LoadModel : Finished Loading Model : {}", path);

    return {model, {}, {}};
}

turas::AssetLoadReturn LoadTexture(const turas::String& path)
{
    turas::log::info("LoadTexture : Loading Texture : {}", path);
    auto asset =  new turas::TextureAsset (path, turas::AssetHandle(turas::Utils::Hash(path), turas::AssetType::Texture), turas::Utils::LoadBinaryFromPath(path));
    turas::AssetLoadCallback task = []( turas::Asset* asset)
    {
        auto* textureAsset = static_cast<turas::TextureAsset*>(asset);
        auto* t = new lvk::Texture(lvk::Texture::CreateTextureFromMemory(
                turas::Engine::INSTANCE->m_VK,
                textureAsset->m_TextureData.data(),
                textureAsset->m_TextureData.size(),
                VK_FORMAT_R8G8B8A8_UNORM));
        textureAsset->m_Texture = t;
    };
    return {asset, {}, {task}};
}


turas::AssetHandle turas::AssetManager::LoadAsset(const turas::String &path, const turas::AssetType &assetType) {
    AssetHandle handle (Utils::Hash(path), assetType);

    switch(assetType)
    {
        case AssetType::Model:
            p_PendingLoads.emplace(handle, std::move(std::async(std::launch::async, LoadModel, path)));
            break;
        case AssetType::Texture:
            p_PendingLoads.emplace(handle, std::move(std::async(std::launch::async, LoadTexture, path)));
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

    if( p_PendingLoads.find(handle) != p_PendingLoads.end() ||
        p_PendingCallbacks.find(handle) != p_PendingCallbacks.end())
    {
        return AssetLoadProgress::Loading;
    }

    return AssetLoadProgress::NotLoaded;
}

void turas::AssetManager::OnUpdate() {
    if(p_PendingLoads.empty() && p_PendingCallbacks.empty())
    {
        return;
    }

    u16 processedCallbacks = 0;

    Vector<AssetHandle> clears;
    for(auto& [handle, asset] : p_PendingCallbacks) {
        if (processedCallbacks == p_CallbackTasksPerTick) break;

        for (u16 i = 0; i < p_CallbackTasksPerTick - processedCallbacks; i++)
        {
            if(i >= asset.m_AssetLoadTasks.size()) break;
            asset.m_AssetLoadTasks.back()(asset.m_LoadedAsset);
            asset.m_AssetLoadTasks.pop_back();
            processedCallbacks++;
        }

        if(asset.m_AssetLoadTasks.empty())
        {
            clears.push_back(handle);
        }
    }

    for(auto& handle : clears)
    {
        p_LoadedAssets.emplace(handle, std::move(UPtr<Asset>(p_PendingCallbacks[handle].m_LoadedAsset)));
        p_PendingCallbacks.erase(handle);
    }

    Vector<AssetHandle> pending;
    for(auto& [handle, future] : p_PendingLoads) {
        if(IsReady(future))
        {
            pending.push_back(handle);
        }
    }

    for(auto& handle : pending) {
        AssetLoadReturn asyncReturn  = p_PendingLoads[handle].get();
        // add new loads
        for(auto& newLoad : asyncReturn.m_NewAssetsToLoad)
        {
            LoadAsset(newLoad.m_Path, newLoad.m_Type);
        }

        if(asyncReturn.m_AssetLoadTasks.empty()) {
            p_LoadedAssets.emplace(handle, std::move(UPtr<Asset>(asyncReturn.m_LoadedAsset)));
        }
        else
        {
            p_PendingCallbacks.emplace(handle, asyncReturn);
        }
        p_PendingLoads.erase(handle);
    }
}

bool turas::AssetManager::AnyAssetsLoading() {
    return !p_PendingLoads.empty() || !p_PendingCallbacks.empty();
}

void turas::AssetManager::Shutdown() {
    WaitAllAssets();

    for(auto&[ handle, asset] : p_LoadedAssets)
    {
        asset.reset();
    }

    p_LoadedAssets.clear();
}

void turas::AssetManager::WaitAllAssets() {

    Vector<AssetLoadInfo> newAssetsToLoad {};

    for(auto& [handle, pending] : p_PendingLoads)
    {
        pending.wait();
        auto asset = pending.get();
        p_LoadedAssets.emplace(handle, std::move(UPtr<Asset>(asset.m_LoadedAsset)));

        newAssetsToLoad.insert(newAssetsToLoad.end(),
                               asset.m_NewAssetsToLoad.begin(), asset.m_NewAssetsToLoad.end());
    }

    p_PendingLoads.clear();

    if(newAssetsToLoad.empty())
    {
        return;
    }

    for(auto& loadInfo : newAssetsToLoad)
    {
        LoadAsset(loadInfo.m_Path, loadInfo.m_Type);
    }

    WaitAllAssets();

}

turas::Asset *turas::AssetManager::GetAsset(turas::AssetHandle &handle) {
    if(p_LoadedAssets.find(handle) == p_LoadedAssets.end())
    {
        return nullptr;
    }

    return p_LoadedAssets[handle].get();
}
