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
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::NotLoaded);
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        turas::Scene* s = e.CreateScene();
        assert(s);
        e.CloseScene(s);
        assert(e.m_ActiveScenes.empty());
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        turas::Scene* s1 = e.CreateScene();
        turas::Scene* s2 = e.CreateScene();
        e.CloseScene(s1);
        assert(e.m_ActiveScenes.size() == 1);
        e.CloseScene(s2);
        assert(e.m_ActiveScenes.empty());
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        turas::Scene* s1 = e.CreateScene();
        turas::Scene* s2 = e.CreateScene();
        e.CloseAllScenes();
        assert(e.m_ActiveScenes.empty());
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        auto* transformSystem  = e.AddSystem<turas::TransformSystem>();
        assert(transformSystem);
        e.Init();
        assert(!e.m_EngineSubSystems.empty());
        e.Shutdown();
    }
)

RUN_TESTS()

