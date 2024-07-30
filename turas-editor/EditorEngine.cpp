#include "EditorEngine.h"

#include "EditorMenu.h"
turas::EditorEngine::EditorEngine() : turas::Engine(true)
{
}
void turas::EditorEngine::Run()
{
	InitEditorFSM();
	// KEEP IN SYNC WITH ENGINE RUN
	while (m_Renderer.m_VK.ShouldRun()) {
		FrameMark;
		ZoneScopedN ("Frame");
		m_EditorFSM.Process();
	}
}
void turas::EditorEngine::AddViewsAndViewports()
{
	auto* view = m_Renderer.CreateView<turas::DebugCameraView> ("Scene View", turas::Utils::Hash("TurasDeferredBuiltin"), "Scene View");
	m_Renderer.CreateViewport<turas::TurasImGuiViewport> ("Scene Viewport");
	m_Renderer.SetViewportView ("Scene Viewport", reinterpret_cast<turas::View*>(view));

}
void turas::EditorEngine::OnEditor()
{
	EditorMenu::OnImGuiEditorMenu ((Engine*) this);
}
void turas::EditorEngine::InitEditorFSM()
{
	m_EditorFSM.AddState (Mode::Edit, [this]() { return OnEditUpdate(); });
	m_EditorFSM.AddStateEntry (Mode::Edit, [this]() { OnEditExit(); });
	m_EditorFSM.AddStateExit (Mode::Edit, [this]() { OnEditExit(); });

	m_EditorFSM.AddState (Mode::Debug, [this]() { return OnRuntimeUpdate(); });
	m_EditorFSM.AddStateEntry (Mode::Debug, [this]() { OnRuntimeEnter(); });
	m_EditorFSM.AddStateExit (Mode::Debug, [this]() { OnRuntimeExit(); });

	m_EditorFSM.AddState (Mode::OpenProject, [this]() { return OnOpenProjectUpdate(); });
	m_EditorFSM.AddStateEntry (Mode::OpenProject, [this]() { OnOpenProjectExit(); });
	m_EditorFSM.AddStateExit (Mode::OpenProject, [this]() { OnOpenProjectEnter(); });

	m_EditorFSM.AddTrigger (Trigger::Play, Mode::Edit, Mode::Debug);
	m_EditorFSM.AddTrigger (Trigger::Stop, Mode::Debug, Mode::Edit);
	m_EditorFSM.AddTrigger (Trigger::OpenProjectFinished, Mode::OpenProject, Mode::Edit);

	m_EditorFSM.SetStartingState (Mode::OpenProject);
}

int turas::EditorEngine::OnEditUpdate()
{
	PrepFrame();
	m_AssetManager.OnUpdate();
	PendingScenes();
	SystemsUpdate();
	DebugUpdate();
	OnEditor();
	SubmitFrame();
	return FSM::NO_TRIGGER;
}
void turas::EditorEngine::OnEditEnter()
{
}
void turas::EditorEngine::OnEditExit()
{
}

int turas::EditorEngine::OnRuntimeUpdate()
{
	PrepFrame();
	m_AssetManager.OnUpdate();
	PendingScenes();
	SystemsUpdate();
	DebugUpdate();
	OnEditor();
	SubmitFrame();
	return FSM::NO_TRIGGER;
}
void turas::EditorEngine::OnRuntimeEnter()
{
}
void turas::EditorEngine::OnRuntimeExit()
{
}

int turas::EditorEngine::OnOpenProjectUpdate()
{
	PrepFrame();
	m_AssetManager.OnUpdate();
	PendingScenes();
	int trigger = OpenProjectImGui();
	SubmitFrame();
	return trigger;
}
void turas::EditorEngine::OnOpenProjectEnter()
{
}
void turas::EditorEngine::OnOpenProjectExit()
{
}

static char open_project_path_buffer[256];
static char open_project_name_buffer[256];

int turas::EditorEngine::OpenProjectImGui()
{
	bool loaded_project = false;
	bool created_project = false;

	if(ImGui::Begin("Create / Open Project")) {

		ImGui::InputText ("Project Path : ", &open_project_path_buffer[0], 256);
		ImGui::InputText ("Project Name : ", &open_project_name_buffer[0], 256);

		if(ImGui::Button("Load Project")) {
			// Load the project
			String project_path_raw = String(open_project_path_buffer);
			String project_path = Utils::TrimString (project_path_raw);

			LoadProject (project_path);
			loaded_project = true;
		}

		if(ImGui::Button("Create Project")) {
			String project_path_raw = String(open_project_path_buffer);
			String project_path = Utils::TrimString (project_path_raw);

			String project_name_raw = String(open_project_name_buffer);
			String project_name = Utils::TrimString (project_name_raw);

			CreateProject (project_name, project_path);
			created_project = true;
		}
	}
	ImGui::End();

	return (loaded_project || created_project) ? Trigger::OpenProjectFinished : FSM::NO_TRIGGER;
}