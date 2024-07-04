#include "test.h"
#include <filesystem>
BEGIN_TESTS()
TEST(
        {
            turas::Engine e;
            e.Init();
            e.Shutdown();
        }
)

TEST(
        {
            turas::Engine e;
            e.Init();
            auto path = "../Sponza/Sponza.gltf";
            auto handle = e.m_AssetManager.LoadAsset(path, turas::AssetType::Model);
            e.m_AssetManager.WaitAllAssets();
            assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loaded);
            e.Shutdown();
        }
)

TEST(
        {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
        e.m_AssetManager.WaitAllAssets();
        e.m_AssetManager.UnloadAsset(handle);

        e.Shutdown();
    }
)

TEST(
    {
        turas::log::info("again!");
    }
)

RUN_TESTS()

