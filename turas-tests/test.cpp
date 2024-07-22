iMgUI#include "test.h"
#include <filesystem>
BEGIN_TESTS()
TEST("Engine Init & Shutdown", {
  turas::Engine e;
  e.Init();
  e.Shutdown();
})

TEST("sync wait asset", {
  turas::Engine e;
  e.Init();
  auto path = "Sponza/Sponza.gltf";
  auto handle = e.m_AssetManager.LoadAsset(path, turas::AssetType::Model);
  e.m_AssetManager.WaitAllAssets();
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loaded);
  e.Shutdown();
})

TEST("Asset unloads sync", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  e.m_AssetManager.WaitAllAssets();
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loaded);
  e.m_AssetManager.UnloadAsset(handle);
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Unloading);
  while (e.m_AssetManager.AnyAssetsUnloading()) {
    e.m_AssetManager.OnUpdate();
  }
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::NotLoaded);
  e.Shutdown();
})

TEST("Asset unloads async", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  while (e.m_AssetManager.AnyAssetsLoading()) {
    e.m_AssetManager.OnUpdate();
  }
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loaded);
  e.m_AssetManager.UnloadAsset(handle);

  while (e.m_AssetManager.AnyAssetsUnloading()) {
    e.m_AssetManager.OnUpdate();
  }

  e.Shutdown();
})

TEST("Asset loads async", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  while (e.m_AssetManager.AnyAssetsLoading()) {
    e.PrepFrame();
    e.m_AssetManager.OnUpdate();
    e.SystemsUpdate();
    turas::log::info(" ---- TICK ---- ");
    if (ImGui::Begin("Loading Assets :) ")) {
    }
    ImGui::End();
    e.SubmitFrame();
  }
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loaded);
  auto *asset = e.m_AssetManager.GetAsset(handle);
  auto *model = reinterpret_cast<turas::ModelAsset *>(asset);
  assert(model);

  for (auto &entry : model->m_Entries) {
    for (auto &[type, handle] : entry.m_AssociatedTextures) {
      assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
             turas::AssetLoadProgress::Loaded);
    }
  }

  e.m_AssetManager.UnloadAllAssets();
  e.Shutdown();
})

TEST("Check GPU submitted objects for Model", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  while (e.m_AssetManager.AnyAssetsLoading()) {
    e.m_AssetManager.OnUpdate();
  }
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loaded);
  auto *asset = e.m_AssetManager.GetAsset(handle);
  auto *model = reinterpret_cast<turas::ModelAsset *>(asset);

  for (auto &entry : model->m_Entries) {
    assert(entry.m_Mesh);
    assert(entry.m_Mesh->m_LvkMesh.m_VertexBuffer != VK_NULL_HANDLE);
    assert(entry.m_Mesh->m_LvkMesh.m_IndexBuffer != VK_NULL_HANDLE);
  }

  e.m_AssetManager.UnloadAllAssets();
  e.Shutdown();
})

TEST("Model Asset is valid", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  e.m_AssetManager.WaitAllAssets();

  auto *asset = e.m_AssetManager.GetAsset(handle);
  assert(asset);
  e.m_AssetManager.UnloadAllAssets();
  e.Shutdown();
})

TEST("Texture asset is valid", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("crate.jpg", turas::AssetType::Texture);
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loading);

  e.m_AssetManager.UnloadAllAssets();
  e.Shutdown();
})

TEST("Texture submitted to GPU async", {
  turas::Engine e;
  e.Init();
  auto handle =
      e.m_AssetManager.LoadAsset("crate.jpg", turas::AssetType::Texture);
  assert(e.m_AssetManager.GetAssetLoadProgress(handle) ==
         turas::AssetLoadProgress::Loading);
  assert(e.m_AssetManager.AnyAssetsLoading());

  while (e.m_AssetManager.AnyAssetsLoading()) {
    e.m_AssetManager.OnUpdate();
  }

  auto *asset = e.m_AssetManager.GetAsset(handle);
  auto *tex = reinterpret_cast<turas::TextureAsset *>(asset);

  assert(tex->m_Texture->m_Image != VK_NULL_HANDLE);
  e.m_AssetManager.UnloadAllAssets();
  e.Shutdown();
})

TEST("Scene is destroyed", {
  turas::Engine e;
  e.Init();
  turas::Scene *s = e.CreateScene("Test");
  assert(s);
  e.CloseScene(s);
  assert(e.m_ActiveScenes.empty());
  e.Shutdown();
})

TEST("Correct number of scenes closed", {
  turas::Engine e;
  e.Init();
  turas::Scene *s1 = e.CreateScene("Test");
  turas::Scene *s2 = e.CreateScene("Test2");
  e.CloseScene(s1);
  assert(e.m_ActiveScenes.size() == 1);
  e.CloseScene(s2);
  assert(e.m_ActiveScenes.empty());
  e.Shutdown();
})

TEST("Scene basic entity tests", {
  turas::Engine e;
  e.Init();
  turas::Scene *s1 = e.CreateScene("Test");
  auto e1 = s1->CreateEntity();
  auto e2 = s1->CreateEntity();
  assert(s1->NumEntities() == 2);
  s1->DestroyEntity(e1);
  assert(s1->NumEntities() == 1);
  s1->DestroyEntity(e2);
  assert(s1->NumEntities() == 0);
  e.Shutdown();
})

TEST("Project object / file created",
     {
         {
             turas::Engine e;
             e.Init();
             e.CreateProject("12345", "./");
             assert(e.m_Project.get() != nullptr);
             assert(std::filesystem::exists("12345.turasproj"));
             e.Shutdown();
}
// while(!std::filesystem::remove("12345.turasproj"));
}
)



TEST("Scene Serialization To Disk", {
  turas::Engine e;
  auto *transformSystem = e.AddSystem<turas::TransformSystem>();
  e.Init();
  turas::Scene *s1 = e.CreateScene("Test Scene 1");
  assert(s1);
  e.SaveScene(s1);
  assert(std::filesystem::exists("scenes/Test Scene 1.tbs"));
  e.Shutdown();

  std::filesystem::remove("scenes/Test Scene 1.tbs");
})

TEST("Scene Saved in Project", {
  turas::Engine e;
  e.Init();
  e.CreateProject("TestProjScenes", "TestProjScenes");
  turas::Scene *s1 = e.CreateScene("ts1");
  e.SaveScene(s1);

  assert(e.m_Project->m_SerializedScenes.find("ts1") !=
         e.m_Project->m_SerializedScenes.end());
  e.Shutdown();
  assert(std::filesystem::exists("../TestProjScenes/scenes/ts1.tbs"));
})

TEST("Load Scene from Name", {
  {
    turas::Engine e;
    e.Init();
    e.CreateProject("TestSceneFromPath", "TestSceneFromPath");
    turas::Scene *s1 = e.CreateScene("ts1");
    e.SaveScene(s1);
    e.SaveProject();
    e.Shutdown();
  }
  assert(std::filesystem::exists(
      "../TestSceneFromPath/TestSceneFromPath.turasproj"));
  {
    turas::Engine e;
    e.Init();
    e.LoadProject("TestSceneFromPath/TestSceneFromPath.turasproj");
    auto *s1 = e.LoadSceneFromName("ts1");
    assert(s1 != nullptr);
    assert(e.m_Project->m_Name == "TestSceneFromPath");
    e.Shutdown();
  }
})

TEST("Load Scene from Path", {
  {
    turas::Engine e;
    e.Init();
    e.CreateProject("TestSceneFromPath", "TestSceneFromPath");
    turas::Scene *s1 = e.CreateScene("ts1");
    e.SaveScene(s1);
    e.SaveProject();
    e.Shutdown();
  }
  assert(std::filesystem::exists(
      "../TestSceneFromPath/TestSceneFromPath.turasproj"));
  {
    turas::Engine e;
    e.Init();
    e.LoadProject("TestSceneFromPath/TestSceneFromPath.turasproj");
    auto *s1 = e.LoadSceneFromPath("scenes/ts1.tbs");
    assert(s1 != nullptr);
    assert(e.m_Project->m_SerializedScenes.find("ts1") !=
           e.m_Project->m_SerializedScenes.end());
    e.Shutdown();
  }
})



TEST("Transform system successfully added",
    {
  turas::Engine e;
  auto *transformSystem = e.AddSystem<turas::TransformSystem>();
  assert(transformSystem);
  e.Init();
  assert(!e.m_EngineSubSystems.empty());
  e.Shutdown();
    }
)

TEST("Can add transform component to entity",
    {
  turas::Engine e;
  auto *transformSystem = e.AddSystem<turas::TransformSystem>();
  assert(transformSystem);
  e.Init();
  auto *s = e.CreateScene("Test");
  auto ent = s->CreateEntity();

  auto &trans = s->AddComponent<turas::TransformComponent>(ent);

  assert(s->HasComponent<turas::TransformComponent>(ent));
  e.Shutdown();
    }
)


TEST("Can remove transform component from entity",
    {
  turas::Engine e;
  auto *transformSystem = e.AddSystem<turas::TransformSystem>();
  assert(transformSystem);
  e.Init();
  auto *s = e.CreateScene("Test");
  auto ent = s->CreateEntity();

  auto &trans = s->AddComponent<turas::TransformComponent>(ent);
  assert(s->HasComponent<turas::TransformComponent>(ent));
  s->RemoveComponent<turas::TransformComponent>(ent);
  assert(!s->HasComponent<turas::TransformComponent>(ent));
  e.Shutdown();
    }
)

TEST("Transform system serialization check 1",
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

TEST("Scene + Transform system serialization check 2", {
  turas::Engine e;
  auto &transformSystem = *e.AddSystem<turas::TransformSystem>();
  e.Init();
  auto *s = e.CreateScene("Test");
  auto ent = s->CreateEntity();
  auto &trans = s->AddComponent<turas::TransformComponent>(ent);
  trans.m_Position.y = 420.0f;
  auto data = s->SaveBinary();
  std::stringstream dataStream{};

  {
    turas::BinaryOutputArchive outputArchive(dataStream);
    outputArchive(data);
  }

  turas::BinaryInputArchive serialized_data(dataStream);

  e.CloseScene(s);

  auto *s2 = e.LoadSceneFromArchive(serialized_data);

  while (e.GetSceneLoadProgress(s2) != turas::AssetLoadProgress::Loaded) {
    e.m_AssetManager.OnUpdate();
    e.PendingScenes();
  }

  assert(s2->m_Name == "Test");
  assert(s2->HasComponent<turas::TransformComponent>(ent));
  assert(abs(s2->GetComponent<turas::TransformComponent>(ent).m_Position.y -
             420.0f) <= 0.0001f);
  e.Shutdown();
});

TEST("Camera system serialization check 1", {
  turas::Engine e;
  auto &transformSystem = *e.AddSystem<turas::CameraSystem>();
  e.Init();
  auto *s = e.CreateScene("Test");
  auto ent = s->CreateEntity();
  auto &trans = s->AddComponent<turas::CameraComponent>(ent);
  trans.m_Camera.m_Height = 12345;
  auto data = s->SaveBinary();
  std::stringstream dataStream{};

  {
    turas::BinaryOutputArchive outputArchive(dataStream);
    outputArchive(data);
  }

  turas::BinaryInputArchive serialized_data(dataStream);

  e.CloseScene(s);

  auto *s2 = e.LoadSceneFromArchive(serialized_data);

  while (e.GetSceneLoadProgress(s2) != turas::AssetLoadProgress::Loaded) {
    e.m_AssetManager.OnUpdate();
    e.PendingScenes();
  }

  assert(s2->m_Name == "Test");
  assert(s2->HasComponent<turas::CameraComponent>(ent));
  assert(s2->GetComponent<turas::CameraComponent>(ent).m_Camera.m_Height ==
         12345);
  e.Shutdown();
});

TEST("Mesh system populates component GPU handles", {
  turas::Engine e;
  auto &meshSystem = *e.AddSystem<turas::MeshSystem>();
  e.Init();
  // load model
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  while (e.m_AssetManager.AnyAssetsLoading()) {
    e.m_AssetManager.OnUpdate();
  }

  auto *s = e.CreateScene("Test");
  auto ent = s->CreateEntity();
  auto &meshComponent = s->AddComponent<turas::MeshComponent>(ent, handle, 0);
  assert(meshComponent.m_MeshAsset != nullptr);
  assert(meshComponent.m_MeshAsset->m_LvkMesh.m_VertexBuffer != VK_NULL_HANDLE);
  assert(meshComponent.m_MeshAsset->m_LvkMesh.m_IndexBuffer != VK_NULL_HANDLE);

  e.Shutdown();
});

TEST("Scene Deserialization : GPU handles populated systemically", {
  turas::Engine e;
  auto &meshSystem = *e.AddSystem<turas::MeshSystem>();

  e.Init();

  // load model
  auto handle =
      e.m_AssetManager.LoadAsset("Sponza/Sponza.gltf", turas::AssetType::Model);
  while (e.m_AssetManager.AnyAssetsLoading()) {
    e.m_AssetManager.OnUpdate();
  }

  auto *s = e.CreateScene("Test");
  auto ent = s->CreateEntity();
  auto &meshComponent = s->AddComponent<turas::MeshComponent>(ent, handle, 0);

  auto sceneBinary = s->SaveBinary();
  e.CloseScene(s);

  e.m_AssetManager.UnloadAllAssets();
  e.m_AssetManager.WaitAllUnloads();

  std::stringstream dataStream{};
  {
    turas::BinaryOutputArchive outputArchive(dataStream);
    outputArchive(sceneBinary);
  }

  turas::BinaryInputArchive serialized_data(dataStream);

  auto *s2 = e.LoadSceneFromArchive(serialized_data);

  while (e.GetSceneLoadProgress(s2) != turas::AssetLoadProgress::Loaded) {
    e.m_AssetManager.OnUpdate();
    e.PendingScenes();
  }

  assert(s2->HasComponent<turas::MeshComponent>(ent));
  assert(s2->GetComponent<turas::MeshComponent>(ent).m_MeshAsset != nullptr);

  e.Shutdown();
});

TEST("VertexLayoutDataBuilder test", {
  turas::Engine e;
  e.Init();
  turas::VertexLayoutDataBuilder builder{};
  builder.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3));
  builder.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3));
  builder.AddAttribute(VK_FORMAT_R32G32_SFLOAT, sizeof(glm::vec2));

  auto data = builder.Build();

  VkVertexInputBindingDescription inputBindingDescription =
      lvk::VertexDataPosNormalUv::GetBindingDescription();
  auto attributeDescriptions =
      lvk::VertexDataPosNormalUv::GetAttributeDescriptions();

  assert(data.m_BindingDescription.binding == inputBindingDescription.binding);
  assert(data.m_BindingDescription.stride == inputBindingDescription.stride);
  assert(data.m_BindingDescription.inputRate ==
         inputBindingDescription.inputRate);

  assert(data.m_AttributeDescriptions.size() == attributeDescriptions.size());

  for (int i = 0; i < attributeDescriptions.size(); i++) {
    assert(attributeDescriptions[i].binding ==
           data.m_AttributeDescriptions[i].binding);
    assert(attributeDescriptions[i].format ==
           data.m_AttributeDescriptions[i].format);
    assert(attributeDescriptions[i].offset ==
           data.m_AttributeDescriptions[i].offset);
    assert(attributeDescriptions[i].location ==
           data.m_AttributeDescriptions[i].location);
  }

  e.Shutdown();
})

TEST("Can load built in shader binaries", {
  turas::Engine e;
  e.Init();
  auto *shader = e.m_Renderer.CreateShaderVF(
      "gbuffer-standard.vert", "gbuffer-standard.frag", "gbuffer-standard");
  assert(shader != nullptr);
  e.Shutdown();
})

TEST("Can successfully link shader binaries", {
  turas::Engine e;
  e.Init();
  auto *shader = e.m_Renderer.CreateShaderVF(
      "gbuffer-standard.vert", "gbuffer-standard.frag", "gbuffer-standard");
  assert(shader->m_ShaderProgram.m_DescriptorSetLayout != VK_NULL_HANDLE);
  e.Shutdown();
})

TEST("Can destroy shader", {
  turas::Engine e;
  e.Init();
  auto *shader = e.m_Renderer.CreateShaderVF(
      "gbuffer-standard.vert", "gbuffer-standard.frag", "gbuffer-standard");
  assert(shader != nullptr);
  shader = nullptr;
  e.m_Renderer.DestroyShader("gbuffer-standard");
  shader = e.m_Renderer.GetShader("gbuffer-standard");
  assert(shader == nullptr);
  e.Shutdown();
})

TEST("Unloaded shaders returns null object", {
  turas::Engine e;
  e.Init();
  auto *shader = e.m_Renderer.CreateShaderVF(
      "gbuffer-standard2.vert", "gbuffer-standard2.frag", "gbuffer-standard2");
  assert(shader == nullptr);
  e.Shutdown();
})

RUN_TESTS()