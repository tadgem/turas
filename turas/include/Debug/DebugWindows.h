//
// Created by liam_ on 7/4/2024.
//

#pragma once

#include "lvk/VulkanAPI.h"

namespace turas {
	class Engine;
    namespace DebugWindows {
        void OnImGuiPerformanceStatsWindow(lvk::VulkanAPI &vk);
    	void OnImGuiProjectStatsWindow(turas::Engine* engine);
    };
}