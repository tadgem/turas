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
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loaded);
        e.m_AssetManager.UnloadAsset(handle);
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Unloading);
        while(e.m_AssetManager.AnyAssetsUnloading())
        {
            e.m_AssetManager.OnUpdate();
        }
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::NotLoaded);
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
        while(e.m_AssetManager.AnyAssetsLoading())
        {
            e.m_AssetManager.OnUpdate();
        }
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loaded);
        e.m_AssetManager.UnloadAsset(handle);

        while(e.m_AssetManager.AnyAssetsUnloading())
        {
            e.m_AssetManager.OnUpdate();
        }

        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
        while(e.m_AssetManager.AnyAssetsLoading())
        {
            e.PrepFrame();
            e.m_AssetManager.OnUpdate();
            e.SystemsUpdate();
            turas::log::info(" ---- TICK ---- ");
            if(ImGui::Begin("Loading Assets :) "))
            {

            }
            ImGui::End();
            e.SubmitFrame();
        }
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loaded);
        auto* asset = e.m_AssetManager.GetAsset(handle);
        auto* model = reinterpret_cast<turas::ModelAsset*>(asset);
        assert(model);

        for(auto& entry : model->m_Entries)
        {
            for(auto& [ type, handle] : entry.m_AssociatedTextures)
            {
                assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loaded);
            }
        }

        e.m_AssetManager.UnloadAllAssets();
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
        while(e.m_AssetManager.AnyAssetsLoading())
        {
            e.m_AssetManager.OnUpdate();
        }
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loaded);
        auto* asset = e.m_AssetManager.GetAsset(handle);
        auto* model = reinterpret_cast<turas::ModelAsset*>(asset);

        for(auto& entry : model->m_Entries)
        {
            assert(entry.m_Mesh.m_LvkMesh);
            assert(entry.m_Mesh.m_LvkMesh->m_VertexBuffer != VK_NULL_HANDLE);
            assert(entry.m_Mesh.m_LvkMesh->m_IndexBuffer != VK_NULL_HANDLE);
        }

        e.m_AssetManager.UnloadAllAssets();
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
        e.m_AssetManager.WaitAllAssets();

        auto* asset = e.m_AssetManager.GetAsset(handle);
        assert(asset);
        e.m_AssetManager.UnloadAllAssets();
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../crate.jpg", turas::AssetType::Texture);
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loading);

        e.m_AssetManager.UnloadAllAssets();
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        auto handle = e.m_AssetManager.LoadAsset("../crate.jpg", turas::AssetType::Texture);
        assert(e.m_AssetManager.GetAssetLoadProgress(handle) == turas::AssetLoadProgress::Loading);
        assert(e.m_AssetManager.AnyAssetsLoading());

        while(e.m_AssetManager.AnyAssetsLoading())
        {
            e.m_AssetManager.OnUpdate();
        }

        auto* asset = e.m_AssetManager.GetAsset(handle);
        auto* tex = reinterpret_cast<turas::TextureAsset*>(asset);

        assert(tex->m_Texture->m_Image != VK_NULL_HANDLE);
        e.m_AssetManager.UnloadAllAssets();
        e.Shutdown();
    }
)

TEST(
    {
        turas::Engine e;
        e.Init();
        turas::Scene* s = e.CreateScene("Test");
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
        turas::Scene* s1 = e.CreateScene("Test");
        turas::Scene* s2 = e.CreateScene("Test2");
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
        turas::Scene* s1 = e.CreateScene("Test");
        turas::Scene* s2 = e.CreateScene("Test2 ");
        e.CloseAllScenes();
        assert(e.m_ActiveScenes.empty());
        e.Shutdown();
    }
)
TEST(
    {
        turas::Engine e;
        e.Init();
        turas::Scene* s1 = e.CreateScene("Test");
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
        auto* s = e.CreateScene("Test");
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
        auto* s = e.CreateScene("Test");
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
    auto &s = *e.CreateScene("Test");
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
    auto* s = e.CreateScene("Test");
    auto ent = s->CreateEntity();
    auto &trans = s->AddComponent<turas::TransformComponent>(ent);
    trans.m_Position.y = 420.0f;
    auto data = s->SaveBinary();
    std::stringstream dataStream {};

    {
        turas::BinaryOutputArchive outputArchive(dataStream);
        outputArchive(data);
    }

    turas::BinaryInputArchive serialized_data(dataStream);

    e.CloseScene(s);

    auto* s2 = e.LoadScene(serialized_data);

    while(e.GetSceneLoadProgress(s2) != turas::AssetLoadProgress::Loaded)
    {
        e.m_AssetManager.OnUpdate();
        e.PendingScenes();
    }

    assert(s2->m_Name == "Test");
    assert(s2->HasComponent<turas::TransformComponent>(ent));
    assert(abs(s2->GetComponent<turas::TransformComponent>(ent).m_Position.y - 420.0f) <= 0.0001f);
    e.Shutdown();
});

TEST(
{
    turas::Engine e;
    auto &meshSystem = *e.AddSystem<turas::MeshSystem>();
    e.Init();
    // load model
    auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
    while(e.m_AssetManager.AnyAssetsLoading())
    {
        e.m_AssetManager.OnUpdate();
    }

    auto* s = e.CreateScene("Test");
    auto ent = s->CreateEntity();
    auto& meshComponent = s->AddComponent<turas::MeshComponent>(ent, handle, 0);
    assert(meshComponent.m_LvkMesh != nullptr);

    e.Shutdown();
});

TEST(
{
    turas::Engine e;
    auto &meshSystem = *e.AddSystem<turas::MeshSystem>();
    e.Init();
    // load model
    auto handle = e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
    while(e.m_AssetManager.AnyAssetsLoading())
    {
        e.m_AssetManager.OnUpdate();
    }

    auto* s = e.CreateScene("Test");
    auto ent = s->CreateEntity();
    auto& meshComponent = s->AddComponent<turas::MeshComponent>(ent, handle, 0);

    auto sceneBinary = s->SaveBinary();
    e.CloseScene(s);

    e.m_AssetManager.UnloadAllAssets();
    e.m_AssetManager.WaitAllUnloads();

    std::stringstream dataStream {};

    {
        turas::BinaryOutputArchive outputArchive(dataStream);
        outputArchive(sceneBinary);
    }

    turas::BinaryInputArchive serialized_data(dataStream);

    auto* s2 = e.LoadScene(serialized_data);

    while(e.GetSceneLoadProgress(s2) != turas::AssetLoadProgress::Loaded)
    {
        e.m_AssetManager.OnUpdate();
        e.PendingScenes();
    }

    assert(s2->HasComponent<turas::MeshComponent>(ent));
    assert(s2->GetComponent<turas::MeshComponent>(ent).m_LvkMesh != nullptr);

    e.Shutdown();
});


TEST(
{
    turas::Engine e;
    e.Init();
    turas::VertexLayoutDataBuilder builder {};
    builder.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3));
    builder.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3));
    builder.AddAttribute(VK_FORMAT_R32G32_SFLOAT, sizeof(glm::vec2));

    auto data = builder.Build();

    VkVertexInputBindingDescription inputBindingDescription = lvk::VertexDataPosNormalUv::GetBindingDescription();
    auto attributeDescriptions = lvk::VertexDataPosNormalUv::GetAttributeDescriptions();

    assert(data.m_BindingDescription.binding == inputBindingDescription.binding);
    assert(data.m_BindingDescription.stride == inputBindingDescription.stride);
    assert(data.m_BindingDescription.inputRate == inputBindingDescription.inputRate);

    assert(data.m_AttributeDescriptions.size() == attributeDescriptions.size());

    for(int i = 0; i < attributeDescriptions.size(); i++)
    {
        assert(attributeDescriptions[i].binding == data.m_AttributeDescriptions[i].binding);
        assert(attributeDescriptions[i].format == data.m_AttributeDescriptions[i].format);
        assert(attributeDescriptions[i].offset == data.m_AttributeDescriptions[i].offset);
        assert(attributeDescriptions[i].location == data.m_AttributeDescriptions[i].location);
    }

    e.Shutdown();
}
)




RUN_TESTS()

