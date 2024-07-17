//
// Created by liam_ on 7/3/2024.
//

#pragma once

#include "STL/Memory.h"
#include "STL/HashMap.h"
#include "entt/entt.hpp"
#include "Core/Serialization.h"

namespace turas {
    class Engine;

    struct Entity {
        entt::entity m_Handle;

        inline operator u32() { return static_cast<u32>(m_Handle); }

        inline operator entt::entity &() { return m_Handle; }

        inline operator entt::entity() { return m_Handle; }

        TURAS_IMPL_ALLOC(Entity)
    };

    class Scene {
    public:
        Scene(const String &name);

        String m_Name;

        Entity CreateEntity();

        void DestroyEntity(Entity &e);

        u32 NumEntities();

        template<typename _Ty, typename ... Args>
        _Ty &AddComponent(Entity &entity, Args &&... args) {
            ZoneScoped;
            return p_Registry.emplace<_Ty>(entity.m_Handle, std::forward<Args>(args)...);
        }

        template<typename _Ty>
        bool RemoveComponent(Entity &entity) {
            ZoneScoped;
            if (HasComponent<_Ty>(entity)) {
                p_Registry.remove<_Ty>(entity.m_Handle);
                return true;
            }
            return false;
        }

        template<typename _Ty>
        bool HasComponent(Entity &entity) {
            ZoneScoped;
            return p_Registry.any_of<_Ty>(entity.m_Handle);
        }

        template<typename _Ty>
        _Ty &GetComponent(Entity &entity) {
            ZoneScoped;
            return p_Registry.get<_Ty>(entity.m_Handle);
        }

        HashMap<u64, String> SaveBinary();

        TURAS_IMPL_ALLOC(Scene)

    protected:
        friend class System;

        friend class Engine;

        void LoadBinary(HashMap<u64, String> &sceneData);

        void LoadBinaryFromArchive(BinaryInputArchive &sceneData);

        const String p_AssetsHashName = "Assets";
        const String p_SceneHashName = "SceneName";

        entt::registry p_Registry;
        u32 p_EntityCount = 0;
    };
}