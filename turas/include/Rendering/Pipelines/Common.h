//
// Created by liam_ on 7/21/2024.
//
#pragma once
#include "Rendering/Pipeline.h"
#include "Rendering/View.h"
namespace turas {
namespace Rendering {
	struct MVPPushConstData {
		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Proj;
	};
	class BuiltInGBufferCommandDispatcher : public PipelineCommandDispatcher {
	public:
							BuiltInGBufferCommandDispatcher (u64				 shader_hash,
															 lvk::Framebuffer*	 framebuffer,
															 lvk::VkPipelineData pipeline_data);
		lvk::Framebuffer*	m_GBuffer;
		lvk::VkPipelineData m_PipelineData;
		const u64			m_ShaderHash;
		void				RecordCommands (VkCommandBuffer buffer, u32 frame_index, View* view, Scene* scene) override;
	};
	class BuiltInLightPassCommandDispatcher : public PipelineCommandDispatcher {
	public:
							  BuiltInLightPassCommandDispatcher (u64				   shader_hash,
																 lvk::Framebuffer*	   framebuffer,
																 lvk::VkPipelineData   pipeline_data,
																 lvk::Mesh*			   screen_quad,
																 lvk::LvkIm3dViewState im3d_view_state,
																 lvk::Material*		   light_pass_material);
		lvk::Framebuffer*	  m_Framebuffer;
		lvk::VkPipelineData	  m_PipelineData;
		const u64			  m_ShaderHash;
		lvk::Mesh*			  m_ScreenQuad;
		lvk::LvkIm3dViewState m_Im3dViewState;
		lvk::Material*		  m_LightPassMaterial;
		void				  RecordCommands (VkCommandBuffer buffer, u32 frame_index, View* view, Scene* scene) override;
	};
	// Create a render pipeline for static meshes
	lvk::VkPipelineData CreateStaticMeshPipeline (lvk::VulkanAPI&	  vk,
												  lvk::ShaderProgram& prog,
												  lvk::Framebuffer*	  fb,
												  VkPolygonMode		  poly_mode		   = VK_POLYGON_MODE_FILL,
												  VkCullModeFlags	  cull_mode		   = VK_CULL_MODE_BACK_BIT,
												  VkCompareOp		  depth_compare_op = VK_COMPARE_OP_LESS,
												  bool				  enable_msaa	   = false);


	lvk::Mesh			CreateScreenQuad(lvk::VulkanAPI& vk);

	static void			DispatchStaticMeshDrawCommands (VkCommandBuffer		cmd,
														uint32_t			frame_index,
														View*				view,
														u64					shader_hash,
														lvk::VkPipelineData pipeline_data,
														Scene*				scene);
}; // namespace Rendering
} // namespace turas
