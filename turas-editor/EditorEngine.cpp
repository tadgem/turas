#include "EditorEngine.h"
turas::EditorEngine::EditorEngine() : turas::Engine(true)
{
}
void turas::EditorEngine::Run()
{
	// KEEP IN SYNC WITH ENGINE RUN
	while (m_Renderer.m_VK.ShouldRun()) {
		FrameMark;
		ZoneScopedN("Frame");
		PrepFrame();
		m_AssetManager.OnUpdate();
		PendingScenes();
		SystemsUpdate();
		DebugUpdate();
		OnEditor();
		SubmitFrame();
	}
}
void turas::EditorEngine::OnEditor()
{
}