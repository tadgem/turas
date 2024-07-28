//
// Created by liam_ on 7/23/2024.
//

#pragma once
#include "Core/System.h"
#include "Core/ECS.h"
#include "STL/Optional.h"
namespace turas {

	enum EntityFlags : u64
	{
		None = 0,
		NavMeshStatic = 1 << 0, // binary 0001
		//B = 1 << 1, // binary 0010
	};
    struct EntityDataComponent {
        String              Name;
        Optional<Entity>    Parent;
		EntityFlags			Flags;

        template<typename Archive>
        void serialize(Archive &ar) {
            ZoneScoped;
            ar(Name, Parent, Flags);
        }
    };

    class EntityDataSystem : public System {
    public:
        EntityDataSystem();

        void OnEngineReady() override;

        void OnSceneLoaded(Scene *scene) override;

        void OnSceneClosed(Scene *scene) override;

        void OnUpdate(Scene *scene) override;

        void OnShutdown() override;

        void SerializeSceneBinary(Scene *scene, BinaryOutputArchive &output) const override;

        void DeserializeSceneBinary(Scene *scene, BinaryInputArchive &input) override;

        Vector<AssetHandle> GetRequiredAssets(Scene *) override;

        template<typename Archive>
        void save(Archive &ar) const {
            ZoneScoped;
            auto entity_data_view = GetSceneRegistry(s_CurrentSerializingScene).view<EntityDataComponent>();
            HashMap<u32, EntityDataComponent> datas{};
            for (const auto &[ent, data]: entity_data_view.each()) {
                datas.emplace(static_cast<u32>(ent), data);
            }
            ar(datas);
        }

        template<typename Archive>
        void load(Archive &ar) {
            ZoneScoped;
            HashMap<u32, EntityDataComponent> datas{};
            ar(datas);
            auto &reg = GetSceneRegistry(s_CurrentSerializingScene);
            for (auto &[handle, data]: datas) {
                auto ent = entt::entity(handle);
                if (!reg.valid(ent)) {
                    ent = reg.create(ent);
                }
                Entity e{ent};
                s_CurrentSerializingScene->AddComponent<EntityDataComponent>(e, data);
            }
        }

        TURAS_IMPL_ALLOC(EntityDataSystem)
    };
}
CEREAL_REGISTER_TYPE(turas::EntityDataSystem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(turas::System, turas::EntityDataSystem)
