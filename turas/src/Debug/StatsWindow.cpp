//
// Created by liam_ on 7/4/2024.
//
#include "Debug/StatsWindow.h"
#include "STL/String.h"
#include "STL/Memory.h"
#include "Debug/Profile.h"

void turas::StatsWindow::OnImGuiStatsWindow(lvk::VulkanAPI &vk) {
    ZoneScoped;
    // Main frame loop
    if (ImGui::Begin("Stats")) {
        ImGui::Text("FPS : %.3f", 1.0 / vk.m_DeltaTime);
        ImGui::Text("Frametime : %.6f", vk.m_DeltaTime);
        ImGui::Separator();

        VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
        vmaGetHeapBudgets(vk.m_Allocator, budgets);
        // TODO: GPU memory should always be 0, but need a better way
        // to ascertain the number of heaps
        for (int i = 0; i < 3; i++) {
            const String heapName = "GPU Heaps " + std::to_string(i);
            if (ImGui::CollapsingHeader(heapName.c_str())) {
                ImGui::Text("Allocs : %i", budgets[i].statistics.allocationCount);
                ImGui::Text("Block Count: %i", budgets[i].statistics.blockCount);
                ImGui::Text("Memory Usage: %zu MB / %zu MB", (budgets[i].usage / 1024 / 1024),
                            (budgets[i].budget / 1024 / 1024));
            }
        }
#ifdef TURAS_ENABLE_MEMORY_TRACKING
        ImGui::Separator();
        if (ImGui::CollapsingHeader("CPU Memory")) {
            ImGui::Text("Untracked : %.4f KB", (float) DebugMemoryTracker::s_UntrackedSize / 1024.0f);
            for (auto &[k, v]: DebugMemoryTracker::s_Instance->s_AllocationInfo) {
                ImGui::Text("%s : %zu KB", k.c_str(), (v.size * v.count) / 1024);
            }
        }
#endif
    }
    ImGui::End();

}
