//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "Core/Log.h"
#include "Core/Engine.h"
#include "Core/ECS.h"
#include "Core/Utils.h"
#include "STL/Vector.h"
#include "STL/Functional.h"
#include "Systems/Transform.h"
#include "Systems/Mesh.h"
#include "Systems/EntityData.h"
#include "Systems/Camera.h"
#include "Rendering/VertexLayouts.h"
#include "Rendering/Renderer.h"

inline static turas::HashMap<turas::String, turas::Procedure> s_Tests = {};

#define BEGIN_TESTS() int main(int argc, char** argv) {
#define TEST(NAME, X) s_Tests.emplace(NAME,[]() X);

#define RUN_TESTS() begin: turas::String selection = ""; {turas::Engine e;e.Init(); while(selection.empty() && \
e.m_Renderer.m_VK.ShouldRun())\
{\
e.PrepFrame();\
auto& io = ImGui::GetIO();\
ImGui::SetNextWindowSize(ImVec2{io.DisplaySize.x, io.DisplaySize.y});\
ImGui::SetNextWindowPos(ImVec2{0.0f, 0.0f});\
if(ImGui::Begin("Select Test"))\
{                                                                                       \
if(ImGui::Button("Run All")) {selection = "RUNALL";}                                     \
ImGui::Separator();\
ImGui::BeginGroup();\
int currentWidth = 0;\
for(auto& [name, func] : s_Tests)                                                  \
{\
    auto textSize = ImGui::CalcTextSize(name.c_str());\
    currentWidth += static_cast<int>(textSize.x + 40.0f);\
    auto windowWidth = ImGui::GetWindowWidth() - 200.0f;\
    if(currentWidth > windowWidth) {currentWidth = 0;} else {ImGui::SameLine();}\
    if(ImGui::Button(name.c_str())){selection = name;}\
}}\
ImGui::EndGroup();\
ImGui::End();\
e.SubmitFrame();\
} \
bool shouldRun = e.m_Renderer.m_VK.ShouldRun();e.Shutdown();if(!shouldRun) {goto end;}        }                                                                          \
                                                                                                       \
if(selection == "RUNALL")                                                          \
{                                                                                       \
for(auto& [name, t] : s_Tests) { spdlog::info("----- END TEST -----", name);spdlog::info("----- BEGIN TEST : {} -----", name);t();}spdlog::info("ALL TESTS COMPLETED SUCCESSFULLY :)");}else {\
\
s_Tests[selection]();}                                                                                      \
goto begin;end: return 1;                                                                                      \
}