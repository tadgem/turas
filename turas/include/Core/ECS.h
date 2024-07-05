//
// Created by liam_ on 7/3/2024.
//

#pragma once
#include "STL/Memory.h"
#include "STL/HashMap.h"
#include "entt/entt.hpp"
#include "Core/Serialization.h"
#include "Core/Engine.h"

namespace turas
{
    struct Entity
    {
        entt::entity    m_Handle;

        inline operator uint32_t() {return static_cast<uint32_t>(m_Handle);}
        inline operator entt::entity&() {return m_Handle;}
        inline operator entt::entity() {return m_Handle;}

        TURAS_IMPL_ALLOC(Entity)
    };

    class Scene
    {
    public:
        Scene();

        Entity  CreateEntity();
        void    DestroyEntity(Entity& e);

        uint32_t NumEntities();

        template<typename _Ty, typename ... Args>
        _Ty&    AddComponent(Entity& entity, Args &&... args)
        {
            return p_Registry.emplace<_Ty>(entity.m_Handle, std::forward<Args>(args)...);
        }

        template<typename _Ty>
        bool    RemoveComponent(Entity& entity)
        {
            if(HasComponent<_Ty>(entity))
            {
                p_Registry.remove<_Ty>(entity.m_Handle);
                return true;
            }
            return false;
        }

        template<typename _Ty>
        bool    HasComponent(Entity& entity)
        {
            return p_Registry.any_of<_Ty>(entity.m_Handle);
        }

        template<class Archive>
        void serialize(Archive & archive)
        {
            for(auto& sys : Engine::INSTANCE->m_EngineSubSystems)
            {

            }
        }


        TURAS_IMPL_ALLOC(Scene)
    protected:
        entt::registry  p_Registry;
        uint32_t        p_EntityCount = 0;
    };
}