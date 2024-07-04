//
// Created by liam_ on 7/3/2024.
//

#pragma once
#include "STL/Memory.h"
#include "entt/entt.hpp"

namespace turas
{
    class Scene
    {
    public:
        Scene();
        entt::registry m_Registry;

        TURAS_IMPL_ALLOC(Scene)
    };
}