//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"
#include "STL/Future.h"
#include "Assets/Asset.h"
#include "STL/Vector.h"
#include "STL/Functional.h"


namespace turas {

    using AssetLoadCallback = Function<void(Asset*)>;
    enum class AssetLoadProgress
    {
        NotLoaded,
        Loading,
        Loaded
    };

    struct AssetLoadInfo
    {
        String      m_Path;
        AssetType   m_Type;

        bool operator==(const AssetLoadInfo &o) const {
            return m_Path == o.m_Path && m_Type == o.m_Type;
        }

        bool operator<(const AssetLoadInfo &o) const {
            return m_Path.size() < o.m_Path.size();
        }
    };

    struct AssetLoadReturn
    {
        Asset*                      m_LoadedAsset;
        Vector<AssetLoadInfo>       m_NewAssetsToLoad;
        Vector<AssetLoadCallback>   m_AssetLoadTasks;
    };

    class AssetManager {
    public:

        AssetHandle         LoadAsset(const String& path, const AssetType& assetType);
        void                UnloadAsset(const AssetHandle& handle);
        AssetLoadProgress   GetAssetLoadProgress(const AssetHandle& handle);
        bool                AnyAssetsLoading();
        Asset*              GetAsset(AssetHandle& handle);

        // used to process async loaded assets
        void                OnUpdate();

        // clear up any remaining assets
        void                Shutdown();

        // synchronous call to wait for all pending assets;
        void                WaitAllAssets();

        TURAS_IMPL_ALLOC(AssetManager)

    protected:
        // Move the Asset* into a UPtr once returned from the future
        HashMap<AssetHandle, Future<AssetLoadReturn>>   p_PendingLoads;
        HashMap<AssetHandle, UPtr<Asset>>               p_LoadedAssets;
        HashMap<AssetHandle, AssetLoadReturn>           p_PendingLoadCallbacks;
        HashMap<AssetHandle, AssetLoadCallback>         p_PendingUnloadCallbacks;

        const uint16_t                                  p_CallbackTasksPerTick = 4;
    };
}