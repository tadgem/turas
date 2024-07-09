//
// Created by liam_ on 7/4/2024.
//

#ifndef TURAS_ALL_MESH_H
#define TURAS_ALL_MESH_H
#include "STL/Memory.h"
#include "Assets/Asset.h"
#include "Core/System.h"
#include "Core/ECS.h"

namespace turas
{
    struct MeshComponent
    {
        MeshComponent() = default;

        MeshComponent(const AssetHandle& handle, u32 index);

        AssetHandle m_Handle;
        u32         m_EntryIndex;
        lvk::Mesh*  m_LvkMesh;

        template<typename Archive>
        void serialize(Archive& ar)
        {
            ZoneScoped;
            ar(m_Handle, m_EntryIndex);
        }

        TURAS_IMPL_ALLOC(MeshComponent)
    };

    class MeshSystem : public System
    {
    public:
        MeshSystem();

        void                    OnEngineReady() override;
        void                    OnSceneLoaded(Scene *scene) override;
        void                    OnSceneClosed(Scene *scene) override;
        void                    OnUpdate(Scene *scene) override;
        void                    OnShutdown() override;
        void                    SerializeSceneBinary(Scene* scene, BinaryOutputArchive& output) const override;
        void                    DeserializeSceneBinary(Scene* scene, BinaryInputArchive& input) override;
        Vector<AssetHandle>     GetRequiredAssets(Scene* scene) override;

        void OnMeshComponentAdded(entt::registry & reg, entt::entity e);

        template<typename Archive>
        void save(Archive& ar) const {
            ZoneScoped;
            auto meshView = GetSceneRegistry(s_CurrentSerializingScene).view<MeshComponent>();
            HashMap<u32, MeshComponent> meshes {};
            for(const auto& [ent, mesh] : meshView.each())
            {
                meshes.emplace(static_cast<u32>(ent), mesh);
            }
            ar(meshes);
        }

        template<typename Archive>
        void load(Archive& ar) {
            ZoneScoped;
            HashMap<u32, MeshComponent> meshes {};
            ar(meshes);
            auto& reg = GetSceneRegistry(s_CurrentSerializingScene);
            for(auto& [handle, mesh] : meshes)
            {
                auto ent = entt::entity(handle);
                if(!reg.valid(ent)) {
                    ent = reg.create(ent);
                }
                Entity e {ent};
                s_CurrentSerializingScene->AddComponent<MeshComponent>(e, mesh);
            }
        }

        TURAS_IMPL_ALLOC(MeshSystem)
    };
}
CEREAL_REGISTER_TYPE(turas::MeshSystem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, turas::MeshSystem)
#endif //TURAS_ALL_MESH_H
