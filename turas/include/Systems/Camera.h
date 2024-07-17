//
// Created by liam_ on 7/10/2024.
//

#pragma once

#include "Core/System.h"
#include "Rendering/View.h"
#include "Core/ECS.h"

namespace turas {

    struct CameraComponent {
        Camera m_Camera;

        template<typename Archive>
        void serialize(Archive &ar) {
            ZoneScoped;
            ar(m_Camera);
        }

        TURAS_IMPL_ALLOC(Camera)
    };

    class CameraSystem : public System {
    public:

        CameraSystem();

        void OnEngineReady() override;

        void OnSceneLoaded(Scene *scene) override;

        void OnSceneClosed(Scene *scene) override;

        void OnUpdate(Scene *scene) override;

        void OnShutdown() override;

        void SerializeSceneBinary(Scene *scene, BinaryOutputArchive &output) const override;

        void DeserializeSceneBinary(Scene *scene, BinaryInputArchive &input) override;

        Vector<AssetHandle> GetRequiredAssets(Scene *scene) override;

        template<typename Archive>
        void save(Archive &ar) const {
            ZoneScoped;
            auto camView = GetSceneRegistry(s_CurrentSerializingScene).view<CameraComponent>();
            HashMap<u32, CameraComponent> cams{};
            for (const auto &[ent, cam]: camView.each()) {
                cams.emplace(static_cast<u32>(ent), cam);
            }
            ar(cams);
        }

        template<typename Archive>
        void load(Archive &ar) {
            ZoneScoped;
            HashMap<u32, CameraComponent> cams{};
            ar(cams);
            auto &reg = GetSceneRegistry(s_CurrentSerializingScene);
            for (auto &[handle, cam]: cams) {
                auto ent = entt::entity(handle);
                if (!reg.valid(ent)) {
                    ent = reg.create(ent);
                }
                Entity e{ent};
                s_CurrentSerializingScene->AddComponent<CameraComponent>(e, cam);
            }
        }

        TURAS_IMPL_ALLOC(CameraSystem)
    };
}