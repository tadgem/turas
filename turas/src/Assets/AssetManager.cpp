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
    return glm::vec3(aiVec.x, aiVec.y, aiVec.z);
}

static glm::vec2 AssimpToGLM(aiVector2D aiVec) {
    return glm::vec2(aiVec.x, aiVec.y);
}

static turas::String AssimpToSTD(aiString str) {
    return turas::String(str.C_Str());
}


void ProcessMesh(const aiScene* scene, aiMesh* mesh, aiNode* node, turas::ModelAsset* model,
                 turas::Vector<turas::AssetLoadInfo>& newAssetsToLoad) {
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

    turas::Mesh m{};
    m.m_VertexLayout = builder.Build();

    if (hasPositions && hasUVs && hasNormals) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            auto Position = AssimpToGLM(mesh->mVertices[i]);
            auto UV = glm::vec2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y);
            auto Normal = AssimpToGLM(mesh->mNormals[i]);
            m.m_VertexData.push_back(Position.x);
            m.m_VertexData.push_back(Position.y);
            m.m_VertexData.push_back(Position.z);
            m.m_VertexData.push_back(Normal.z);
            m.m_VertexData.push_back(Normal.x);
            m.m_VertexData.push_back(Normal.y);
            m.m_VertexData.push_back(UV.x);
            m.m_VertexData.push_back(UV.y);
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
                m.m_IndexData.push_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[index]));
            }
        }
    }
    turas::AABB aabb = { {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
                  {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z} };
    m.m_AABB = aabb;
    turas::HashMap<turas::Texture::MapType, turas::AssetHandle> maps;

    aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
    for (int i = 0; i < meshMaterial->mNumProperties; i++)
    {
        aiMaterialProperty* prop = meshMaterial->mProperties[i];
        if(prop->mSemantic == aiTextureType_NONE || prop->mType != aiPTI_String) {
            continue;
        }
        aiString assimpString;
        aiGetMaterialTexture(meshMaterial, (aiTextureType)prop->mSemantic, 0, &assimpString);

        turas::String filePath = AssimpToSTD(assimpString);
        assert(!filePath.empty());

        turas::AssetHandle handle (turas::Utils::Hash(filePath), turas::AssetType::Texture);
        auto mapType = static_cast<turas::Texture::MapType>(prop->mSemantic);
        maps.emplace(mapType, handle);
        turas::AssetLoadInfo loadInfo {filePath, turas::AssetType::Texture};

        if(std::find(newAssetsToLoad.begin(), newAssetsToLoad.end(), loadInfo) != newAssetsToLoad.end()){
            continue;
        }
        newAssetsToLoad.push_back(loadInfo);
    }

    model->m_Entries.push_back({m, maps});
}


void ProcessNode(const aiScene* scene, aiNode* node, turas::ModelAsset* model, turas::Vector<turas::AssetLoadInfo>& newAssetsToLoad)
{
    if (node->mNumMeshes > 0) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            unsigned int sceneIndex = node->mMeshes[i];
            aiMesh* mesh = scene->mMeshes[sceneIndex];
            ProcessMesh(scene, mesh, node, model, newAssetsToLoad);
        }
    }

    if (node->mNumChildren == 0) {
        return;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node, model, newAssetsToLoad);
    }
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

    turas::Vector<turas::AssetLoadInfo> newAssetsToLoad;

    ProcessNode( scene, scene->mRootNode, model, newAssetsToLoad);

    turas::log::info("LoadModel : Finished Loading Model : {}", path);

    turas::Vector<turas::AssetLoadCallback> callbacks;
    for(int i = 0; i < model->m_Entries.size(); i++)
    {
        callbacks.emplace_back([i]( turas::Asset* asset)
        {
            auto* modelAsset = reinterpret_cast<turas::ModelAsset*>(asset);

            auto* mesh = new lvk::Mesh();
            turas::Engine::INSTANCE->m_VK.CreateVertexBuffer(modelAsset->m_Entries[i].m_Mesh.m_VertexData,
                                                             mesh->m_VertexBuffer, mesh->m_VertexBufferMemory );
            turas::Engine::INSTANCE->m_VK.CreateIndexBuffer(modelAsset->m_Entries[i].m_Mesh.m_IndexData,
                                                             mesh->m_IndexBuffer, mesh->m_IndexBufferMemory);
            mesh->m_IndexCount = modelAsset->m_Entries[i].m_Mesh.m_IndexData.size();
            modelAsset->m_Entries[i].m_Mesh.m_LvkMesh = mesh;
        });
    }

    return {model, newAssetsToLoad, {}};
}

turas::AssetLoadReturn LoadTexture(const turas::String& path)
{
    turas::log::info("LoadTexture : Loading Texture : {}", path);
    auto asset =  new turas::TextureAsset (path, turas::AssetHandle(turas::Utils::Hash(path), turas::AssetType::Texture), turas::Utils::LoadBinaryFromPath(path));
    turas::AssetLoadCallback task = []( turas::Asset* asset)
    {
        auto* textureAsset = reinterpret_cast<turas::TextureAsset*>(asset);
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
