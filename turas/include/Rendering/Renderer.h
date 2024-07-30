//
// Created by liam_ on 7/5/2024.
//
#pragma once
#include "Core/Utils.h"
#include "Im3D/im3d_lvk.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/View.h"
#include "Rendering/Viewport.h"
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

		struct ViewportData
		{
			UPtr<Viewport>	m_Viewport;
			View*			m_View = nullptr;
		};

		Renderer(bool enable_debug_validation);
		void	  Start();
		void	  Reset();
		void	  Shutdown();
		void	  PreFrame();
		void	  PostFrame();
		bool	  AddPipelineTemplate(u64 hash, const CreatePipelineCallback& pipeline_template_function);
		bool	  RemovePipelineTemplate(u64 hash);
		bool	  DestroyView(const String& name);
		View*	  GetView(const String& name);
		View*	  GetView(u64 name_hash);
		Pipeline* GetViewPipeline(const String& name);
		Pipeline* GetViewPipeline(u64 nameHash);
		Shader*	  CreateShaderVF(const String& vert_name, const String& frag_name, const String& shader_name);
		bool	  DestroyShader(const String& shader_name);
		Shader*	  GetShader(const String& shader_name);
		bool	  SetViewportView(const String& viewport_name,  View* view);
		bool	  SetViewportView(u64 viewport_hash, View* view);
		View*	  GetViewportView(const String& viewport_name);
		View*	  GetViewportView(u64 viewport_hash);

		template <typename _ViewTy, typename... Args>
		_ViewTy* CreateView(const String& name, u64 pipeline_hash, Args&&... args)
		{
			ZoneScoped;
			static_assert(std::is_base_of<View, _ViewTy>());
			if (p_CreatePipelineCallbacks.find(pipeline_hash) == p_CreatePipelineCallbacks.end()) {
				return nullptr;
			}
			Pipeline* p			   = p_CreatePipelineCallbacks[pipeline_hash]((Renderer*)this);
			u64		  viewNameHash = Utils::Hash(name);
			p_ViewData.emplace(viewNameHash, ViewData{CreateUnique<_ViewTy>(std::forward<Args>(args)...), UPtr<Pipeline>(p)});
			return reinterpret_cast<_ViewTy*>(p_ViewData[viewNameHash].m_View.get());
		}

		template <typename _ViewportTy, typename ... Args>
		_ViewportTy*	CreateViewport(const String& name, Args&&... args)
		{
			ZoneScoped;
			static_assert(std::is_base_of<Viewport, _ViewportTy>());
			u64 hash = Utils::Hash(name);
			p_Viewports.emplace(hash, ViewportData{ CreateUnique<_ViewportTy>(std::forward<Args>(args)...), nullptr});
			return reinterpret_cast<_ViewportTy*>(p_Viewports[hash].m_Viewport.get());
		}

		TURAS_IMPL_ALLOC(Renderer)
		// interface to GPU (vulkan)
		lvk::VulkanAPI_SDL m_VK;
		lvk::LvkIm3dState  m_Im3dState;

	protected:
		// callbacks to construct named pipelines
		HashMap<u64, CreatePipelineCallback> p_CreatePipelineCallbacks;
		// active view data
		HashMap<u64, ViewData> 				p_ViewData;
		// active view ports
		HashMap<u64, ViewportData> 			p_Viewports;
		// all loaded shader stage binaries
		HashMap<String, lvk::ShaderStage> 	p_ShaderStages;
		// all 'linked' shader programs
		HashMap<String, UPtr<Shader>> 		p_ShaderPrograms;

		friend class Engine;
		void OnImGui();
		void LoadShaderBinaries();
	};
} // namespace turas
