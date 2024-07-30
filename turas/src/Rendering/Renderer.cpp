#include "Rendering/Renderer.h"

#include "Core/Engine.h"
#include "Core/Log.h"
#include "Debug/Profile.h"
turas::Renderer::Renderer(bool enable_debug_validation) : m_VK(enable_debug_validation) {}
void			 turas::Renderer::Start()
{
	ZoneScoped;
	m_VK.Start ("Turas", 1920, 1080);
	m_Im3dState = lvk::LoadIm3D (m_VK);
	LoadShaderBinaries();
}
void turas::Renderer::Reset()
{
	ZoneScoped;
	p_ShaderStages.clear();
	for(auto& [name, shader] : p_ShaderPrograms) {
		shader->m_ShaderProgram.Free (m_VK);
	}
	p_ShaderPrograms.clear();

	for(auto& [hash, view] : p_ViewData) {
		view.Free (m_VK);
	}
	p_ViewData.clear();

	for(auto& [hash, viewport] : p_Viewports) {
		viewport.m_Viewport->Free (m_VK);
	}
	p_Viewports.clear();
}
void turas::Renderer::Shutdown()
{
	ZoneScoped;
	Reset();
	lvk::FreeIm3d(m_VK, m_Im3dState);
	m_VK.Cleanup();
}
void turas::Renderer::PreFrame()
{
	ZoneScoped;
	m_VK.PreFrame();
}
void turas::Renderer::PostFrame()
{
	ZoneScoped;
	// Update each viewport before rendering current frame
	for(auto& [hash, viewport_data] : p_Viewports)
	{
		if(viewport_data.m_View == nullptr) continue;

		Pipeline* view_pipeline = p_ViewData[viewport_data.m_View->m_Hash].m_Pipeline.get();
		if(view_pipeline == nullptr) continue;

		viewport_data.m_Viewport->Update(viewport_data.m_View, view_pipeline);
	}

	for (u32 i = 0; i < lvk::MAX_FRAMES_IN_FLIGHT; i++) {
		VkCommandBuffer& cmd = m_VK.BeginGraphicsCommands(i);

		for(auto& scene : Engine::INSTANCE->m_ActiveScenes) {
			for(auto& [hash, view_data] : p_ViewData) {
				for(int state_updater = 0; state_updater < view_data.m_Pipeline->m_StateUpdaters.size(); state_updater++) {
					view_data.m_Pipeline->m_StateUpdaters[state_updater]->OnUpdateState (scene.get(), i);
				}
				for(int dispatcher = 0; dispatcher < view_data.m_Pipeline->m_CommandDispatchers.size(); dispatcher++) {
					view_data.m_Pipeline->m_CommandDispatchers[dispatcher]->RecordCommands (cmd, i, view_data.m_View.get(), scene.get());
				}
			}

		}
		for(auto& [hash, viewport_data] : p_Viewports)
		{
			viewport_data.m_Viewport->RecordViewportCommands(m_VK, cmd, i, viewport_data.m_View, p_ViewData[viewport_data.m_View->m_Hash].m_Pipeline.get());
		}
		m_VK.EndGraphicsCommands(i);
	}
	m_VK.PostFrame();
}
bool turas::Renderer::AddPipelineTemplate(turas::u64 hash, const turas::CreatePipelineCallback& pipeline_template_function)
{
	ZoneScoped;
	if (p_CreatePipelineCallbacks.find(hash) != p_CreatePipelineCallbacks.end()) {
		return false;
	}
	p_CreatePipelineCallbacks.emplace(hash, pipeline_template_function);
	return true;
}
bool turas::Renderer::RemovePipelineTemplate(turas::u64 hash)
{
	ZoneScoped;
	if (p_CreatePipelineCallbacks.find(hash) == p_CreatePipelineCallbacks.end()) {
		return false;
	}
	p_CreatePipelineCallbacks.erase(hash);
	return true;
}
bool turas::Renderer::DestroyView(const turas::String& name)
{
	ZoneScoped;
	u64 hash = Utils::Hash(name);
	if (p_ViewData.find(hash) == p_ViewData.end()) {
		return false;
	}
	p_ViewData[hash].Free(m_VK);
	p_ViewData.erase(hash);
	return true;
}
turas::View* turas::Renderer::GetView(const turas::String& name)
{
	ZoneScoped;
	return GetView(Utils::Hash(name));
}
turas::View* turas::Renderer::GetView(turas::u64 name_hash)
{
	ZoneScoped;
	if (p_ViewData.find(name_hash) == p_ViewData.end()) {
		return nullptr;
	}
	return p_ViewData[name_hash].m_View.get();
}
turas::Pipeline* turas::Renderer::GetViewPipeline(turas::u64 nameHash)
{
	ZoneScoped;
	if (p_ViewData.find(nameHash) == p_ViewData.end()) {
		return nullptr;
	}
	return p_ViewData[nameHash].m_Pipeline.get();
}
turas::Pipeline* turas::Renderer::GetViewPipeline(const turas::String& name)
{
	ZoneScoped;
	return GetViewPipeline(Utils::Hash(name));
}
void turas::Renderer::LoadShaderBinaries()
{
	// Load all files ending in .spv in the shaders directory (recursively)
	Vector<String> shaders = Utils::GetFilesInDirectory("shaders/");
	for (auto& path : shaders) {
		if (path.find(".spv") < path.size()) {
			// load the shader
			Vector<u8>			   binary = Utils::LoadBinaryFromPath(path);
			lvk::ShaderStage::Type stage  = lvk::ShaderStage::Type::Unknown;
			if (path.find(".vert") < path.size())
				stage = lvk::ShaderStage::Type::Vertex;
			if (path.find(".frag") < path.size())
				stage = lvk::ShaderStage::Type::Fragment;
			if (path.find(".comp") < path.size())
				stage = lvk::ShaderStage::Type::Compute;
			if (stage == lvk::ShaderStage::Type::Unknown) {
				log::error("Renderer : Failed to load stage at path {}, unkown shader stage.", path);
				continue;
			}
			String			 shaderName	 = Utils::GetFilenameFromPath(path);
			String			 cleanName	 = shaderName.substr(0, shaderName.size() - 4); // remove .spv from filename
			lvk::ShaderStage shaderStage = lvk::ShaderStage::Create(m_VK, binary, stage);
			p_ShaderStages.emplace(cleanName, shaderStage);
		}
	}
}
void turas::Renderer::OnImGui()
{
	if (ImGui::Begin("Renderer Debug")) {
		if (ImGui::CollapsingHeader("Loaded Shaders")) {
			for (auto& [name, stage] : p_ShaderStages) {
				ImGui::TextUnformatted(name.c_str());
			}
		}
	}
	ImGui::End();
}
turas::Shader*
turas::Renderer::CreateShaderVF(const turas::String& vert_name, const turas::String& frag_name, const turas::String& shader_name)
{
	if (p_ShaderStages.find(vert_name) == p_ShaderStages.end() || p_ShaderStages.find(frag_name) == p_ShaderStages.end()) {
		return nullptr;
	}
	if (p_ShaderPrograms.find(shader_name) != p_ShaderPrograms.end()) {
		return p_ShaderPrograms[shader_name].get();
	}
	lvk::ShaderStage&  vertStage = p_ShaderStages[vert_name];
	lvk::ShaderStage&  fragStage = p_ShaderStages[frag_name];
	lvk::ShaderProgram prog		 = lvk::ShaderProgram::Create(m_VK, vertStage, fragStage);
	p_ShaderPrograms.emplace(shader_name, CreateUnique<Shader>(Shader{shader_name, Utils::Hash(shader_name), prog, {vert_name, frag_name}}));
	return p_ShaderPrograms[shader_name].get();
}
bool turas::Renderer::DestroyShader(const turas::String& shader_name)
{
	if (p_ShaderPrograms.find(shader_name) == p_ShaderPrograms.end()) {
		return false;
	}
	p_ShaderPrograms[shader_name]->m_ShaderProgram.Free(m_VK);
	p_ShaderPrograms[shader_name].reset();
	p_ShaderPrograms.erase(shader_name);
	return true;
}
turas::Shader* turas::Renderer::GetShader(const turas::String& shader_name)
{
	if (p_ShaderPrograms.find(shader_name) == p_ShaderPrograms.end()) {
		return nullptr;
	}
	return p_ShaderPrograms[shader_name].get();
}
bool turas::Renderer::SetViewportView (turas::u64 viewport_hash, turas::View* view)
{
	if(!view)
	{
		return false;
	}

	if(p_Viewports.find(viewport_hash) == p_Viewports.end())
	{
		return false;
	}

	p_Viewports[viewport_hash].m_View = view;
	return true;
}
bool turas::Renderer::SetViewportView (const turas::String& viewport_name, turas::View* view)
{
	return SetViewportView(Utils::Hash(viewport_name), view);
}
turas::View* turas::Renderer::GetViewportView (turas::u64 viewport_hash)
{
	if(p_Viewports.find(viewport_hash) == p_Viewports.end())
	{
		return nullptr;
	}

	return p_Viewports[viewport_hash].m_View;
}
turas::View* turas::Renderer::GetViewportView (const turas::String& viewport_name)
{
	return GetViewportView(Utils::Hash(viewport_name));
}
void turas::Renderer::ViewData::Free(lvk::VulkanAPI& vk)
{
	m_Pipeline->Free (vk);
	ZoneScoped;
}
