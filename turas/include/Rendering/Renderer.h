//
// Created by liam_ on 7/5/2024.
//
#pragma once
#include "Core/Utils.h"
#include "Im3D/im3d_lvk.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/View.h"
#include "STL/Functional.h"
#include "STL/HashMap.h"
#include "VulkanAPI_SDL.h"
namespace turas
{
	class Renderer;
	using CreatePipelineCallback = Function<Pipeline*(Renderer*)>;

	/**
	* @brief Renderer
	* @author Liam Devlin
	*
	* Responsible for rendering the scene for each active view
	* Also high level container for pipelines, views, shaders,
	* pipeline creation templates etc.
	*/
	class Renderer
	{
	public:
		struct ViewData {
			UPtr<View>	   m_View;
			UPtr<Pipeline> m_Pipeline;
			void		   Free(lvk::VulkanAPI& vk);
		};

		Renderer(bool enableDebugValidation);
		void	  Start();
		void	  Shutdown();
		void	  PreFrame();
		void	  PostFrame();
		bool	  AddPipelineTemplate(u64 hash, const CreatePipelineCallback& pipelineTemplateFunction);
		bool	  RemovePipelineTemplate(u64 hash);
		bool	  DestroyView(const String& name);
		View*	  GetView(const String& name);
		View*	  GetView(u64 nameHash);
		Pipeline* GetViewPipeline(const String& name);
		Pipeline* GetViewPipeline(u64 nameHash);
		Shader*	  CreateShaderVF(const String& vertName, const String& fragName, const String& shaderName);
		bool	  DestroyShader(const String& shaderName);
		Shader*	  GetShader(const String& shaderName);
		template <typename _ViewTy, typename... Args>
		_ViewTy* CreateView(const String& name, u64 pipelineHash, Args&&... args)
		{
			ZoneScoped;
			static_assert(std::is_base_of<View, _ViewTy>());
			if (p_CreatePipelineCallbacks.find(pipelineHash) == p_CreatePipelineCallbacks.end()) {
				return nullptr;
			}
			Pipeline* p			   = p_CreatePipelineCallbacks[pipelineHash]((Renderer*)this);
			u64		  viewNameHash = Utils::Hash(name);
			p_ViewData.emplace(viewNameHash, ViewData{CreateUnique<_ViewTy>(std::forward<Args>(args)...), UPtr<Pipeline>(p)});
			return p_ViewData[viewNameHash].m_View.get();
		}
		TURAS_IMPL_ALLOC(Renderer)
		// interface to GPU (vulkan)
		lvk::VulkanAPI_SDL m_VK;
		lvk::LvkIm3dState  m_Im3dState;

	protected:
		// callbacks to construct named pipelines
		HashMap<u64, CreatePipelineCallback> p_CreatePipelineCallbacks;
		// active view data
		HashMap<u64, ViewData> p_ViewData;
		// all loaded shader stage binaries
		HashMap<String, lvk::ShaderStage> p_ShaderStages;
		// all 'linked' shader programs
		HashMap<String, UPtr<Shader>> p_ShaderPrograms;
		friend class Engine;
		void OnImGui();
		void LoadShaderBinaries();
	};
} // namespace turas
