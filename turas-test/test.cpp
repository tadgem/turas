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
        e.Init();
        turas::Scene* s1 = e.CreateScene();
        auto e1 = s1->CreateEntity();
        auto e2 = s1->CreateEntity();
        assert(s1->NumEntities() == 2);
        s1->DestroyEntity(e1);
        assert(s1->NumEntities() == 1);
        s1->DestroyEntity(e2);
        assert(s1->NumEntities() == 0);
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

TEST(
    {
        turas::Engine e;
        auto* transformSystem  = e.AddSystem<turas::TransformSystem>();
        assert(transformSystem);
        e.Init();
        auto* s = e.CreateScene();
        auto ent = s->CreateEntity();

        auto& trans = s->AddComponent<turas::TransformComponent>(ent);

        assert(s->HasComponent<turas::TransformComponent>(ent));
        e.Shutdown();
    }
)


TEST(
    {
        turas::Engine e;
        auto* transformSystem  = e.AddSystem<turas::TransformSystem>();
        assert(transformSystem);
        e.Init();
        auto* s = e.CreateScene();
        auto ent = s->CreateEntity();

        auto& trans = s->AddComponent<turas::TransformComponent>(ent);
        assert(s->HasComponent<turas::TransformComponent>(ent));
        s->RemoveComponent<turas::TransformComponent>(ent);
        assert(!s->HasComponent<turas::TransformComponent>(ent));
        e.Shutdown();
    }
)

TEST(
    {
    turas::Engine e;
    auto &transformSystem = *e.AddSystem<turas::TransformSystem>();
    e.Init();
    auto &s = *e.CreateScene();
    auto ent = s.CreateEntity();
    auto &trans = s.AddComponent<turas::TransformComponent>(ent);
    auto data = s.SaveBinary();

    assert(!data.empty());

    e.Shutdown();
});

TEST(
{
    turas::Engine e;
    auto &transformSystem = *e.AddSystem<turas::TransformSystem>();
    e.Init();
    auto* s = e.CreateScene();
    auto ent = s->CreateEntity();
    auto &trans = s->AddComponent<turas::TransformComponent>(ent);
    trans.m_Position.y = 420.0f;
    auto data = s->SaveBinary();

    e.CloseScene(s);

    auto* s2 = e.CreateScene();
    s2->LoadBinary(data);

    assert(s2->HasComponent<turas::TransformComponent>(ent));
    assert(abs(s2->GetComponent<turas::TransformComponent>(ent).m_Position.y - 420.0f) <= 0.0001f);
    e.Shutdown();
});


RUN_TESTS()

