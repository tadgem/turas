#include "Systems/EntityData.h"
#include "Core/Utils.h"

turas::EntityDataSystem::EntityDataSystem() : System(GetTypeHash<EntityDataSystem>())
{
}

void turas::EntityDataSystem::OnEngineReady()
{
}

void turas::EntityDataSystem::OnSceneLoaded(Scene* scene)
{
}

void turas::EntityDataSystem::OnSceneClosed(Scene* scene)
{
}

void turas::EntityDataSystem::OnUpdate(Scene* scene)
{
}

void turas::EntityDataSystem::OnShutdown()
{
}

void turas::EntityDataSystem::SerializeSceneBinary(Scene* scene, BinaryOutputArchive& output) const
{
    s_CurrentSerializingScene = scene;
    output(*this);
    s_CurrentSerializingScene = nullptr;
}


void turas::EntityDataSystem::DeserializeSceneBinary(Scene* scene, BinaryInputArchive& input)
{
    s_CurrentSerializingScene = scene;
    input(*this);
    s_CurrentSerializingScene = nullptr;
}

turas::Vector<turas::AssetHandle> turas::EntityDataSystem::GetRequiredAssets(Scene*)
{
    return turas::Vector<AssetHandle>();
}
