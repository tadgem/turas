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

static glm::vec3 AssimpToGLM(aiVector3D aiVec) {
    ZoneScoped;
    return glm::vec3(aiVec.x, aiVec.y, aiVec.z);
}

static glm::vec2 AssimpToGLM(aiVector2D aiVec) {
    ZoneScoped;
    return glm::vec2(aiVec.x, aiVec.y);
}

static turas::String AssimpToSTD(aiString str) {
    ZoneScoped;
    return turas::String(str.C_Str());
}

void
ProcessMesh(const turas::String &assetDir, const aiScene *scene, aiMesh *mesh, aiNode *node, turas::ModelAsset *model,
            turas::Vector<turas::AssetLoadInfo> &newAssetsToLoad) {
    ZoneScoped;
    using namespace lvk;
    bool hasPositions = mesh->HasPositions();
    bool hasUVs = mesh->HasTextureCoords(0);
    bool hasNormals = mesh->HasNormals();
    bool hasIndices = mesh->HasFaces();

    assert(hasIndices);
    turas::VertexLayoutDataBuilder builder;
    builder.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3));
    builder.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3));
    builder.AddAttribute(VK_FORMAT_R32G32_SFLOAT, sizeof(glm::vec2));

    auto *m = new turas::Mesh();
    m->m_VertexLayout = builder.Build();

    if (hasPositions && hasUVs && hasNormals) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            auto Position = AssimpToGLM(mesh->mVertices[i]);
            auto UV = glm::vec2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y);
            auto Normal = AssimpToGLM(mesh->mNormals[i]);
            m->m_VertexData.push_back(Position.x);
            m->m_VertexData.push_back(Position.y);
            m->m_VertexData.push_back(Position.z);
            m->m_VertexData.push_back(Normal.z);
            m->m_VertexData.push_back(Normal.x);
            m->m_VertexData.push_back(Normal.y);
            m->m_VertexData.push_back(UV.x);
            m->m_VertexData.push_back(UV.y);
        }
    }

    if (hasIndices) {
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace currentFace = mesh->mFaces[i];
            if (currentFace.mNumIndices != 3) {
                spdlog::error("Attempting to import a mesh with non triangular face structure! cannot load this mesh.");
                return;
            }
            for (unsigned int index = 0; index < mesh->mFaces[i].mNumIndices; index++) {
                m->m_IndexData.push_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[index]));
            }
        }
    }

    turas::AABB aabb = {{mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
                        {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}};
    m->m_AABB = aabb;
    turas::HashMap<turas::Texture::MapType, turas::AssetHandle> maps;

    aiMaterial *meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
    for (unsigned int i = 0; i < meshMaterial->mNumProperties; i++) {
        aiMaterialProperty *prop = meshMaterial->mProperties[i];
        if (prop->mSemantic == aiTextureType_NONE || prop->mType != aiPTI_String) {
            continue;
        }
        aiString assimpString;
        aiGetMaterialTexture(meshMaterial, (aiTextureType) prop->mSemantic, 0, &assimpString);

        turas::String localFilePath = AssimpToSTD(assimpString);
        turas::String filePath = assetDir + "/" + localFilePath;
        assert(!filePath.empty());

        turas::AssetHandle handle(turas::Utils::Hash(filePath), turas::AssetType::Texture);
        auto mapType = static_cast<turas::Texture::MapType>(prop->mSemantic);
        maps.emplace(mapType, handle);
        turas::AssetLoadInfo loadInfo{filePath, turas::AssetType::Texture};

        if (std::find(newAssetsToLoad.begin(), newAssetsToLoad.end(), loadInfo) != newAssetsToLoad.end()) {
            continue;
        }
        newAssetsToLoad.push_back(loadInfo);
    }

    model->m_Entries.push_back({turas::UPtr<turas::Mesh>(m), maps});
}

void ProcessNode(const turas::String &assetDirectory, const aiScene *scene, aiNode *node, turas::ModelAsset *model,
                 turas::Vector<turas::AssetLoadInfo> &newAssetsToLoad) {
    ZoneScoped;
    if (node->mNumMeshes > 0) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            unsigned int sceneIndex = node->mMeshes[i];
            aiMesh *mesh = scene->mMeshes[sceneIndex];
            ProcessMesh(assetDirectory, scene, mesh, node, model, newAssetsToLoad);
        }
    }

    if (node->mNumChildren == 0) {
        return;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(assetDirectory, scene, node, model, newAssetsToLoad);
    }
}

turas::AssetLoadReturn LoadModel(const turas::String &path) {
    ZoneScoped;
    turas::log::info("LoadModel : Loading Model : {}", path);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(),
                                             aiProcess_Triangulate |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_OptimizeMeshes |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_OptimizeGraph |
                                             aiProcess_FixInfacingNormals |
                                             aiProcess_FindInvalidData |
                                             aiProcess_GenBoundingBoxes
    );

    if (scene == nullptr) {
        turas::log::error("LoadModel : Failed to open model at path : {}", path);
        return {nullptr, {}};
    }
    auto *model = new turas::ModelAsset(
            path, turas::AssetHandle(turas::Utils::Hash(path), turas::AssetType::Model));

    turas::Vector<turas::AssetLoadInfo> newAssetsToLoad;
    turas::String assetDirectory = turas::Utils::GetDirectoryFromFilename(path);
    ProcessNode(assetDirectory, scene, scene->mRootNode, model, newAssetsToLoad);

    turas::log::info("LoadModel : Finished Loading Model : {}", path);

    turas::Vector<turas::AssetLoadCallback> callbacks;
    for (int i = 0; i < model->m_Entries.size(); i++) {
        callbacks.emplace_back([i](turas::Asset *asset) {
            auto *modelAsset = reinterpret_cast<turas::ModelAsset *>(asset);

            auto mesh = lvk::Mesh();
            turas::Engine::INSTANCE->m_Renderer.m_VK.CreateVertexBuffer(modelAsset->m_Entries[i].m_Mesh->m_VertexData,
                                                                        mesh.m_VertexBuffer, mesh.m_VertexBufferMemory);
            turas::Engine::INSTANCE->m_Renderer.m_VK.CreateIndexBuffer(modelAsset->m_Entries[i].m_Mesh->m_IndexData,
                                                                       mesh.m_IndexBuffer, mesh.m_IndexBufferMemory);
            mesh.m_IndexCount = static_cast<uint32_t>(modelAsset->m_Entries[i].m_Mesh->m_IndexData.size());
            modelAsset->m_Entries[i].m_Mesh->m_LvkMesh = mesh;
        });
    }

    return {model, newAssetsToLoad, callbacks};
}

void UnloadModel(turas::Asset *asset) {
    auto *model = reinterpret_cast<turas::ModelAsset *>(asset);
    for (auto &entry: model->m_Entries) {
        entry.m_Mesh->Free(turas::Engine::INSTANCE->m_Renderer.m_VK);
    }
}

turas::AssetLoadReturn LoadTexture(const turas::String &path) {
    ZoneScoped;
    turas::log::info("LoadTexture : Loading Texture : {}", path);
    auto asset = new turas::TextureAsset(path, turas::AssetHandle(turas::Utils::Hash(path), turas::AssetType::Texture),
                                         turas::Utils::LoadBinaryFromPath(path));
    turas::AssetLoadCallback task = [](turas::Asset *asset) {
        auto *textureAsset = reinterpret_cast<turas::TextureAsset *>(asset);
        auto *t = new lvk::Texture(lvk::Texture::CreateTextureFromMemory(
                turas::Engine::INSTANCE->m_Renderer.m_VK,
                textureAsset->m_TextureData.data(),
                static_cast<uint32_t>(textureAsset->m_TextureData.size()),
                VK_FORMAT_R8G8B8A8_UNORM));
        textureAsset->m_Texture = t;
    };
    return {asset, {}, {task}};
}

void UnloadTexture(turas::Asset *asset) {
    ZoneScoped;
    auto *tex = reinterpret_cast<turas::TextureAsset *>(asset);
    if (!tex->m_Texture) return;
    tex->m_Texture->Free(turas::Engine::INSTANCE->m_Renderer.m_VK);
}

turas::AssetHandle turas::AssetManager::LoadAsset(const turas::String &path, const turas::AssetType &assetType) {
    ZoneScoped;
    AssetHandle handle(Utils::Hash(path), assetType);
    p_QueuedLoads.emplace_back(AssetLoadInfo{path, assetType});

    return handle;
}

void turas::AssetManager::UnloadAsset(const turas::AssetHandle &handle) {
    ZoneScoped;
    if (p_LoadedAssets.find(handle) == p_LoadedAssets.end()) {
        return;
    }

    switch (handle.m_Type) {
        case AssetType::Model:
            p_PendingUnloadCallbacks.emplace(handle, UnloadModel);
            break;
        case AssetType::Texture:
            p_PendingUnloadCallbacks.emplace(handle, UnloadTexture);
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
}

turas::AssetLoadProgress turas::AssetManager::GetAssetLoadProgress(const turas::AssetHandle &handle) {
    ZoneScoped;

    for (auto &queued: p_QueuedLoads) {
        if (queued.ToHandle() == handle) {
            return AssetLoadProgress::Loading;
        }
    }

    if (p_PendingLoadTasks.find(handle) != p_PendingLoadTasks.end() ||
        p_PendingLoadCallbacks.find(handle) != p_PendingLoadCallbacks.end()) {
        return AssetLoadProgress::Loading;
    }

    if (p_PendingUnloadCallbacks.find(handle) != p_PendingUnloadCallbacks.end()) {
        return AssetLoadProgress::Unloading;
    }

    if (p_LoadedAssets.find(handle) != p_LoadedAssets.end()) {
        return AssetLoadProgress::Loaded;
    }

    return AssetLoadProgress::NotLoaded;
}

void turas::AssetManager::OnUpdate() {
    ZoneScoped;
    if (!AnyAssetsLoading()) {
        return;
    }

    HandleLoadAndUnloadCallbacks();

    HandlePendingLoads();

    Vector<AssetHandle> finished;
    for (auto &[handle, future]: p_PendingLoadTasks) {
        if (IsReady(future)) {
            finished.push_back(handle);
        }
    }

    for (auto &handle: finished) {
        AssetLoadReturn asyncReturn = p_PendingLoadTasks[handle].get();
        // enqueue new loads
        for (auto &newLoad: asyncReturn.m_NewAssetsToLoad) {
            LoadAsset(newLoad.m_Path, newLoad.m_Type);
        }

        if (asyncReturn.m_AssetLoadTasks.empty()) {
            p_LoadedAssets.emplace(handle, std::move(UPtr<Asset>(asyncReturn.m_LoadedAsset)));
        } else {
            p_PendingLoadCallbacks.emplace(handle, asyncReturn);
        }

        p_PendingLoadTasks.erase(handle);
    }
}

bool turas::AssetManager::AnyAssetsLoading() {
    ZoneScoped;
    return !p_PendingLoadTasks.empty() || !p_PendingLoadCallbacks.empty() || !p_PendingUnloadCallbacks.empty() ||
           !p_QueuedLoads.empty();
}

bool turas::AssetManager::AnyAssetsUnloading() {
    ZoneScoped;
    return !p_PendingUnloadCallbacks.empty();
}

void turas::AssetManager::Shutdown() {
    ZoneScoped;
    WaitAllAssets();
    WaitAllUnloads();

    UnloadAllAssets();
}

void turas::AssetManager::WaitAllAssets() {
    ZoneScoped;
    while (AnyAssetsLoading()) {
        OnUpdate();
    }

}

turas::Asset *turas::AssetManager::GetAsset(turas::AssetHandle &handle) {
    ZoneScoped;
    if (p_LoadedAssets.find(handle) == p_LoadedAssets.end()) {
        return nullptr;
    }

    return p_LoadedAssets[handle].get();
}

void turas::AssetManager::HandleLoadAndUnloadCallbacks() {
    ZoneScoped;
    u16 processedCallbacks = 0;
    Vector<AssetHandle> clears;

    for (auto &[handle, asset]: p_PendingLoadCallbacks) {
        if (processedCallbacks == p_CallbackTasksPerTick) break;

        for (u16 i = 0; i < p_CallbackTasksPerTick - processedCallbacks; i++) {
            if (i >= asset.m_AssetLoadTasks.size()) break;
            asset.m_AssetLoadTasks.back()(asset.m_LoadedAsset);
            asset.m_AssetLoadTasks.pop_back();
            processedCallbacks++;
        }

        if (asset.m_AssetLoadTasks.empty()) {
            clears.push_back(handle);
        }
    }
    for (auto &handle: clears) {
        p_LoadedAssets.emplace(handle, std::move(UPtr<Asset>(p_PendingLoadCallbacks[handle].m_LoadedAsset)));
        p_PendingLoadCallbacks.erase(handle);
    }
    clears.clear();

    for (auto &[handle, callback]: p_PendingUnloadCallbacks) {
        if (processedCallbacks == p_CallbackTasksPerTick) break;
        callback(p_LoadedAssets[handle].get());
        clears.push_back(handle);
        processedCallbacks++;
    }

    for (auto &handle: clears) {
        p_PendingUnloadCallbacks.erase(handle);
        p_LoadedAssets.erase(handle);
    }
}

void turas::AssetManager::UnloadAllAssets() {
    ZoneScoped;
    Vector<AssetHandle> assetsRemaining{};

    for (auto &[handle, asset]: p_LoadedAssets) {
        assetsRemaining.push_back(handle);
    }

    for (auto &handle: assetsRemaining) {
        UnloadAsset(handle);
    }

    WaitAllUnloads();
}

void turas::AssetManager::WaitAllUnloads() {
    ZoneScoped;
    while (AnyAssetsUnloading()) {
        OnUpdate();
    }
}

void turas::AssetManager::DispatchAssetLoadTask(const turas::AssetHandle &handle, turas::AssetLoadInfo &info) {
    switch (info.m_Type) {
        case AssetType::Model:
            p_PendingLoadTasks.emplace(handle, std::move(std::async(std::launch::async, LoadModel, info.m_Path)));
            break;
        case AssetType::Texture:
            p_PendingLoadTasks.emplace(handle, std::move(std::async(std::launch::async, LoadTexture, info.m_Path)));
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
}

void turas::AssetManager::HandlePendingLoads() {
    while (p_PendingLoadTasks.size() < p_MaxAsyncTaskInFlight && !p_QueuedLoads.empty()) {
        auto &info = p_QueuedLoads.front();
        DispatchAssetLoadTask(info.ToHandle(), info);
        p_QueuedLoads.erase(p_QueuedLoads.begin());
    }
}
