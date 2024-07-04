//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"
#include "STL/Future.h"
#include "Assets/Asset.h"
#include "STL/Vector.h"



namespace turas {

    enum class AssetLoadProgress
    {
        NotLoaded,
        Loading,
        Loaded
    };

    class AssetManager {
    public:
        AssetHandle         LoadAsset(const String& path, const AssetType& assetType);
        void                UnloadAsset(const AssetHandle& handle);
        AssetLoadProgress   GetAssetLoadProgress(const AssetHandle& handle);
        bool                AnyAssetsLoading();

        // used to process async loaded assets
        void                OnUpdate();

        TURAS_IMPL_ALLOC(AssetManager)

    protected:
        // Move the Asset* into a UPtr once returned from the future
        HashMap<AssetHandle, Future<Asset*>>    p_PendingLoads;
        HashMap<AssetHandle, UPtr<Asset>>       p_LoadedAssets;
    };
}