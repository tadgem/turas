#include "Core/Engine.h"
#include "Core/ECS.h"
#include "Core/Log.h"
#include "Debug/StatsWindow.h"
#include "Debug/dm_sans.h"
#include "STL/Memory.h"
#include "spdlog/spdlog.h"

#ifdef WIN32

#include "windows.h"
#endif

turas::Engine::Engine(bool enableDebugUpdate)
  : p_DebugUpdateEnabled(enableDebugUpdate), m_Renderer(enableDebugUpdate)
{
  ZoneScoped;
  INSTANCE = this;
  p_OriginalWorkingDir = std::filesystem::current_path().string();
  if (!std::filesystem::exists("shaders"))
  {
    ChangeWorkingDirectory("../");
  }
  log::info("Turas : Working Directory : {}",
            std::filesystem::current_path().string());
}

void turas::Engine::Init()
{
  ZoneScoped;
  spdlog::info("Initialising Turas");
  m_Renderer.Start();
  m_Im3dState = lvk::LoadIm3D(m_Renderer.m_VK);
  DebugInit();
  for (auto& sys : m_EngineSubSystems)
  {
    sys->OnEngineReady();
  }
}

void turas::Engine::Shutdown()
{
  ZoneScoped;

  if (p_DebugUpdateEnabled)
  {
    m_FileWatcher->removeWatch(m_GlobalProjectWatchId);
    // remove other watch sources;
  }

  CloseAllScenes();

  for (auto& scene : m_ActiveScenes)
  {
    scene.reset();
  }
  m_ActiveScenes.clear();
  for (auto& sys : m_EngineSubSystems)
  {
    sys->OnShutdown();
    sys.reset();
  }
  m_EngineSubSystems.clear();
  m_AssetManager.Shutdown();

  lvk::FreeIm3d(m_Renderer.m_VK, m_Im3dState);
  m_Renderer.Shutdown();
  INSTANCE = nullptr;

  ChangeWorkingDirectory(p_OriginalWorkingDir);
}

void turas::Engine::Run()
{
  while (m_Renderer.m_VK.ShouldRun())
  {
    FrameMark;
    ZoneScopedN("Frame");
    PrepFrame();
    m_AssetManager.OnUpdate();
    PendingScenes();
    SystemsUpdate();
    DebugUpdate();
    SubmitFrame();
  }
}

turas::Scene* turas::Engine::CreateScene(const String& name)
{
  ZoneScoped;
  Scene* scene =
    m_ActiveScenes.emplace_back(std::move(CreateUnique<Scene>(name))).get();
  for (auto& sys : m_EngineSubSystems)
  {
    sys->OnSceneLoaded(scene);
  }

  return scene;
}

void turas::Engine::CloseScene(turas::Scene* scene)
{
  ZoneScoped;
  int index = -1;
  for (int i = 0; i < m_ActiveScenes.size(); i++)
  {
    if (m_ActiveScenes[i].get() == scene)
    {
      index = i;
    }
  }

  if (index >= 0)
  {
    for (auto& sys : m_EngineSubSystems)
    {
      sys->OnSceneClosed(m_ActiveScenes[index].get());
    }
    m_ActiveScenes[index].reset();
    m_ActiveScenes.erase(m_ActiveScenes.begin() + index);
  }
}

void turas::Engine::CloseAllScenes()
{
  ZoneScoped;
  for (auto& scene : m_ActiveScenes)
  {
    // sys scene cleanup logic
    for (auto& sys : m_EngineSubSystems)
    {
      sys->OnSceneClosed(scene.get());
    }
    scene.reset();
  }

  m_ActiveScenes.clear();
}

void turas::Engine::PrepFrame()
{
  ZoneScoped;
  m_Renderer.PreFrame();
  Im3d::NewFrame();
}

void turas::Engine::SubmitFrame()
{
  ZoneScoped;
  Im3d::EndFrame();
  m_Renderer.PostFrame();
}

void turas::Engine::SystemsUpdate()
{
  ZoneScoped;
  for (auto& sys : m_EngineSubSystems)
  {
    for (auto& scene : m_ActiveScenes)
    {
      sys->OnUpdate(scene.get());
    }
  }
}

void turas::Engine::PendingScenes()
{
  if (m_AssetManager.AnyAssetsLoading())
  {
    return;
  }

  if (m_PendingScenes.empty())
  {
    return;
  }

  for (auto* pendingScene : m_PendingScenes)
  {
    // Turn raw scene ptr to a UPtr so it will be
    // freed when the scene is removed from list of active scenes
    m_ActiveScenes.push_back(UPtr<Scene>(pendingScene));
    for (auto& sys : m_EngineSubSystems)
    {
      sys->OnSceneLoaded(pendingScene);
    }
  }
  m_PendingScenes.clear();
}

turas::Scene* turas::Engine::LoadSceneFromArchive(BinaryInputArchive& archive)
{
  auto* s = new Scene("empty");
  s->LoadBinaryFromArchive(archive);
  m_PendingScenes.push_back(s);
  return s;
}

turas::AssetLoadProgress
turas::Engine::GetSceneLoadProgress(turas::Scene* scene)
{
  if (std::find(m_PendingScenes.begin(), m_PendingScenes.end(), scene) !=
    m_PendingScenes.end())
  {
    return AssetLoadProgress::Loading;
  }

  for (auto& activeScene : m_ActiveScenes)
  {
    if (activeScene.get() == scene)
    {
      return AssetLoadProgress::Loaded;
    }
  }

  return turas::AssetLoadProgress::NotLoaded;
}

void turas::Engine::DebugUpdate()
{
  if (!p_DebugUpdateEnabled)
  {
    return;
  }

  m_Renderer.OnImGui();
  StatsWindow::OnImGuiStatsWindow(m_Renderer.m_VK);
}

void turas::Engine::DebugInit()
{
  InitImGuiStyle();
  if (!p_DebugUpdateEnabled)
  {
    return;
  }
  m_FileWatcher = CreateUnique<efsw::FileWatcher>();
  m_UpdateListener = CreateUnique<TurasFilesystemListener>();
  m_GlobalProjectWatchId =
    m_FileWatcher->addWatch(".", m_UpdateListener.get(), true);
  // Perhaps other sources of watch?
  m_FileWatcher->watch();
}

bool turas::Engine::LoadProject(const turas::String& path)
{
  if (m_Project.get() != nullptr)
  {
    return false;
  }
  m_Project = CreateUnique<Project>("It doesnt matter");
  // Deserialize project string
  String projectStr = Utils::LoadStringFromPath(path);
  std::stringstream stream;
  stream << projectStr;
  {
    cereal::XMLInputArchive archive(stream);
    archive(*m_Project.get());
  }

  String dir = Utils::GetDirectoryFromFilename(path);
  // change working directory to project dir
  ChangeWorkingDirectory(dir);
  // if debug copy shaders to directory
  if (p_DebugUpdateEnabled)
  {
    CopyShadersToProject();
  }
  // update renderer to reload all shaders
  // load default scene
  return true;
}

bool turas::Engine::CreateProject(const turas::String& name,
                                  const turas::String& projectDir)
{
  if (m_Project.get() != nullptr)
  {
    return false;
  }
  m_Project = CreateUnique<Project>(name);

  if (!std::filesystem::exists(projectDir))
  {
    std::filesystem::create_directory(projectDir);
  }

  // change working directory to projectDir
  ChangeWorkingDirectory(projectDir);
  // save project to specified path
  SaveProject();
  // if debug copy shaders to directory
  if (p_DebugUpdateEnabled)
  {
    CopyShadersToProject();
  }
  // update renderer to reload all shaders
  // create default scene
  return true;
}

bool turas::Engine::SaveProject()
{
  if (m_Project.get() == nullptr)
  {
    return false;
  }

  std::stringstream stream;
  {
    cereal::XMLOutputArchive archive(stream);
    archive(*m_Project.get());
  }
  String projectFilePath = std::filesystem::current_path().string() + "/" +
    m_Project->m_Name + ".turasproj";
  Utils::SaveStringToPath(stream.str(), projectFilePath);
  return true;
}

void turas::Engine::ChangeWorkingDirectory(const turas::String& newDirectory)
{
  std::filesystem::current_path(newDirectory);
}

void turas::Engine::CopyShadersToProject()
{
}

bool turas::Engine::SaveScene(turas::Scene* s)
{
  if (!s)
    return false;

  auto data = s->SaveBinary();
  std::stringstream dataStream{};
  {
    turas::BinaryOutputArchive outputArchive(dataStream);
    outputArchive(data);
  }

  if (!std::filesystem::exists("scenes/"))
  {
    std::filesystem::create_directory("scenes");
  }

  String scenePath = "scenes/" + s->m_Name + ".tbs";
  Utils::SaveStringToPath(dataStream.str(), scenePath);

  if (m_Project)
  {
    m_Project->m_SerializedScenes.emplace(s->m_Name, scenePath);
  }

  return true;
}

turas::Scene* turas::Engine::LoadSceneFromPath(const String& path)
{
  if (!std::filesystem::exists(path))
    return nullptr;

  String s = Utils::LoadStringFromPath(path);
  std::stringstream stream;
  stream << s;
  turas::BinaryInputArchive input(stream);
  Scene* scene = LoadSceneFromArchive(input);
  if (!m_Project)
  {
    return scene;
  }

  if (m_Project->m_SerializedScenes.find(scene->m_Name) ==
    m_Project->m_SerializedScenes.end())
  {
    m_Project->m_SerializedScenes.emplace(scene->m_Name, path);
  }
  return scene;
}

turas::Scene* turas::Engine::LoadSceneFromName(const turas::String& name)
{
  if (!m_Project)
    return nullptr;
  if (m_Project->m_SerializedScenes.find(name) ==
    m_Project->m_SerializedScenes.end())
  {
    return nullptr;
  }

  return LoadSceneFromPath(m_Project->m_SerializedScenes[name]);
}

void turas::Engine::InitImGuiStyle()
{
  ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)&dm_sans_ttf_bin[0],
                                             static_cast<u32>(DM_SANS_TTF_SIZE), 18.0f);

  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button] = ImVec4(0.29f, 0.29f, 0.29f, 0.62f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.29f, 0.29f, 0.72f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.29f, 0.29f, 0.23f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
  colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding = ImVec2(8.00f, 8.00f);
  style.FramePadding = ImVec2(5.00f, 2.00f);
  style.CellPadding = ImVec2(6.00f, 6.00f);
  style.ItemSpacing = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
  style.IndentSpacing = 25;
  style.ScrollbarSize = 15;
  style.GrabMinSize = 10;
  style.WindowBorderSize = 2;
  style.ChildBorderSize = 2;
  style.PopupBorderSize = 2;
  style.FrameBorderSize = 1;
  style.TabBorderSize = 1;
  style.WindowRounding = 7;
  style.ChildRounding = 4;
  style.FrameRounding = 1;
  style.PopupRounding = 4;
  style.ScrollbarRounding = 9;
  style.GrabRounding = 3;
  style.LogSliderDeadzone = 4;
  style.TabRounding = 4;
}

void turas::TurasFilesystemListener::handleFileAction(
  efsw::WatchID watchid, const std::string& dir, const std::string& filename,
  efsw::Action action, std::string oldFilename)
{
  switch (action)
  {
  case efsw::Actions::Add:
    std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Added"
      << std::endl;
    break;
  case efsw::Actions::Delete:
    std::cout << "DIR (" << dir << ") FILE (" << filename
      << ") has event Delete" << std::endl;
    break;
  case efsw::Actions::Modified:
    std::cout << "DIR (" << dir << ") FILE (" << filename
      << ") has event Modified" << std::endl;
    break;
  case efsw::Actions::Moved:
    std::cout << "DIR (" << dir << ") FILE (" << filename
      << ") has event Moved from (" << oldFilename << ")" << std::endl;
    break;
  default:
    std::cout << "Should never happen!" << std::endl;
  }
}
