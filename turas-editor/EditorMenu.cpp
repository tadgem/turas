#include "EditorMenu.h"
#include "Core/Engine.h"

static char create_scene_name_buf[256];
static bool show_new_scene_dialog = false;
void turas::EditorMenu::OnImGuiEditorMenu (Engine* engine)
{
	if(ImGui::BeginMainMenuBar()) {
		if(ImGui::BeginMenu ("File")) {
			if(ImGui::MenuItem("New Scene")) {
				show_new_scene_dialog = true;
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if(show_new_scene_dialog) {
		ImGui::Begin("New Scene");
		ImGui::InputText ("Scene Name", &create_scene_name_buf[0], 256);
		if(ImGui::Button ("Create")) {
			String scene_name_raw = String(create_scene_name_buf);
			String scene_name = Utils::TrimString (scene_name_raw);
			engine->CreateScene (scene_name);
			show_new_scene_dialog = false;
		}
		ImGui::End();
	}
}