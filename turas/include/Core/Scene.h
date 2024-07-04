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

        void *operator new(size_t size) {
            if (turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.find("Scene") ==
                turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.end()) {
                turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.emplace("Scene", AllocInfo{0, 0});
            }
            turas::DebugMemoryTracker::s_Instance->s_AllocationInfo["Scene"].count++;
            turas::DebugMemoryTracker::s_Instance->s_AllocationInfo["Scene"].size += size;
            return malloc(size);
        }

        void operator delete(void *p) {
            free(p);
            if (!turas::DebugMemoryTracker::s_Instance)return;
            if (turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.find("Scene") ==
                turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.end()) {
                return;
            }
            turas::DebugMemoryTracker::s_Instance->s_AllocationInfo["Scene"].count--;
            turas::DebugMemoryTracker::s_Instance->s_AllocationInfo["Scene"].size -= sizeof(Scene);
        }
    };
}