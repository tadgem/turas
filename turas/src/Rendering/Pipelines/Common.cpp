#include "Rendering/Pipelines/Common.h"
#include "Core/ECS.h"
#include "Rendering/VertexLayouts.h"
#include "STL/Array.h"
#include "Systems/Material.h"
#include "Systems/Mesh.h"
#include "Systems/Transform.h"
lvk::VkPipelineData turas::Rendering::CreateStaticMeshPipeline(lvk::VulkanAPI& vk, lvk::ShaderProgram& prog, lvk::Framebuffer* fb,
															   VkPolygonMode poly_mode, VkCullModeFlags cull_mode,
															   VkCompareOp depth_compare_op, bool enable_msaa)
{
	VkPipelineLayout pipeline_layout;
	auto			 binding_descriptions	= Vector<VkVertexInputBindingDescription>{lvk::VertexDataPosNormalUv::GetBindingDescription()};
	auto			 attribute_descriptions = lvk::VertexDataPosNormalUv::GetAttributeDescriptions();
	VkPipeline		 pipeline = vk.CreateRasterizationGraphicsPipeline(prog, binding_descriptions, attribute_descriptions, fb->m_RenderPass,
																	   vk.m_SwapChainImageExtent.width, vk.m_SwapChainImageExtent.height,
																	   poly_mode, cull_mode, enable_msaa, depth_compare_op, pipeline_layout,
																	   static_cast<u32>(fb->m_ColourAttachments.size()));
	return {pipeline, pipeline_layout};
}
turas::Rendering::BuiltInGBufferCommandDispatcher::BuiltInGBufferCommandDispatcher(u64 shader_hash, lvk::Framebuffer* framebuffer,
																				   lvk::VkPipelineData pipeline_data)
	: m_GBuffer(framebuffer), m_PipelineData(pipeline_data), m_ShaderHash(shader_hash)
{
}
void turas::Rendering::BuiltInGBufferCommandDispatcher::RecordCommands(VkCommandBuffer commandBuffer, turas::u32 frame_index, View* view,
																	   turas::Scene* scene)
{
	Array<VkClearValue, 4> clear_values{};
	clear_values[0].color		 = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clear_values[1].color		 = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clear_values[2].color		 = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clear_values[3].depthStencil = {1.0f, 0};
	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType			   = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass		   = m_GBuffer->m_RenderPass;
	render_pass_info.framebuffer	   = m_GBuffer->m_SwapchainFramebuffers[frame_index];
	render_pass_info.renderArea.offset = {0, 0};
	render_pass_info.renderArea.extent = m_GBuffer->m_Resolution;
	render_pass_info.clearValueCount   = static_cast<uint32_t>(clear_values.size());
	render_pass_info.pClearValues	   = clear_values.data();
	vkCmdBeginRenderPass(commandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineData.m_Pipeline);
	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.x		  = 0.0f;
	viewport.width	  = static_cast<float>(m_GBuffer->m_Resolution.width);
	viewport.height	  = static_cast<float>(m_GBuffer->m_Resolution.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent =
			VkExtent2D{static_cast<uint32_t>(m_GBuffer->m_Resolution.width), static_cast<uint32_t>(m_GBuffer->m_Resolution.height)};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	// despatch scene render
	DispatchStaticMeshDrawCommands(commandBuffer, frame_index, view, m_ShaderHash, m_PipelineData, scene);
	vkCmdEndRenderPass(commandBuffer);
}
void turas::Rendering::DispatchStaticMeshDrawCommands(VkCommandBuffer cmd, uint32_t frame_index, View* view, turas::u64 shader_hash,
													  lvk::VkPipelineData pipeline_data, turas::Scene* scene)
{
	auto scene_view = scene->GetRegistry().view<TransformComponent, MeshComponent, MaterialComponent>();
	// dispatch scene render
	for (const auto& [e, transform, mesh, material] : scene_view.each()) {
		// TODO: should probably think of a better way to do this
		// so we dont need to iterate over the entire list of drawables
		if (material.m_ShaderHash != shader_hash || material.m_Material == nullptr)
			continue;
		VkBuffer		 vertex_buffers[]{mesh.m_MeshAsset->m_LvkMesh.m_VertexBuffer};
		VkDeviceSize	 sizes[] = {0};
		MVPPushConstData data{transform.m_ModelMatrix, view->GetViewMatrix(), view->GetProjectionMatrix()};
		vkCmdBindVertexBuffers(cmd, 0, 1, vertex_buffers, sizes);
		vkCmdBindIndexBuffer(cmd, mesh.m_MeshAsset->m_LvkMesh.m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdPushConstants(cmd, pipeline_data.m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MVPPushConstData), &data);
		// TODO: Look at support for multiple descriptor sets in a single shader
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_data.m_PipelineLayout, 0, 1,
								&material.m_Material->m_DescriptorSets.front().m_Sets[frame_index], 0, nullptr);
		vkCmdDrawIndexed(cmd, mesh.m_MeshAsset->m_LvkMesh.m_IndexCount, 1, 0, 0, 0);
	}
}
