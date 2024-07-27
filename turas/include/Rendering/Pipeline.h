//
// Created by liam_ on 7/10/2024.
//
#pragma once
#include "Core/Types.h"
#include "Rendering/Material.h"
#include "Rendering/View.h"
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "lvk/Pipeline.h"
namespace turas
{
	class Scene;
	// pure virtual class, implement per pipeline or use abstractions
	// pass in to pipeline to record commands for associated pipeline state
	// e.g. pass in dependencies to derived type constructor
	// Renderer will contain a map of name : pipeline creation function
	// function will add objects to pipeline and configure command dispatcher to record necessary commands
	class PipelineCommandDispatcher
	{
	public:
		virtual void RecordCommands(VkCommandBuffer buffer, u32 frameIndex, View* view, Scene* scene) = 0;
	};
	// use this to update uniform or any other kind of per frame buffers
	class PipelineStateUpdater
	{
	public:
		virtual void OnUpdateState(Scene* scene) = 0;
	};
	class Pipeline
	{
	public:
		Vector<UPtr<PipelineCommandDispatcher>> m_Renderers;
		Vector<UPtr<PipelineStateUpdater>>		m_StateUpdaters;
		Vector<MaterialCandidate>				m_MaterialCandidates;
		lvk::Pipeline							m_LvkPipeline;
		lvk::Mesh								m_PresentQuad;
		void									Free(lvk::VulkanAPI& vk);
		TURAS_IMPL_ALLOC(Pipeline)
	};
} // namespace turas
