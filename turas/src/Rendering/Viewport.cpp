#include "Rendering/Viewport.h"
#include "Core/Engine.h"
#include "Core/Log.h"
#include "ImGui/lvk_extensions.h"
#include "Rendering/Pipelines/Common.h"

turas::PresentToImageViewport::PresentToImageViewport (VkRenderPass render_pass, turas::Array<VkFramebuffer, 2> framebuffers) :
m_PresentRenderPass(render_pass), m_PresentFramebuffers(framebuffers)
{
	auto& vk = Engine::INSTANCE->m_Renderer.m_VK;
	m_PresentMaterial = lvk::Material::Create(vk, GetPresentShader()->m_ShaderProgram);
	auto binding_descriptions = Vector<VkVertexInputBindingDescription> {lvk::VertexDataPosUv::GetBindingDescription()};
	auto attribute_descriptions = lvk::VertexDataPosUv::GetAttributeDescriptions();
	m_PresentPipelineData.m_Pipeline = vk.CreateRasterizationGraphicsPipeline (GetPresentShader()->m_ShaderProgram,
		binding_descriptions,
		attribute_descriptions,
		render_pass,
		vk.m_SwapChainImageExtent.width, vk.m_SwapChainImageExtent.height,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		false,
		VK_COMPARE_OP_LESS,
		m_PresentPipelineData.m_PipelineLayout
	);
}

void turas::PresentToImageViewport::Update ( turas::View* view, turas::Pipeline* view_pipeline)
{
	lvk::DrawIm3dTextListsImGui (Im3d::GetTextDrawLists(), Im3d::GetTextDrawListCount(),
			view->m_ViewWidth, view->m_ViewHeight, view->GetProjectionMatrix() * view->GetViewMatrix());
}

void turas::PresentToImageViewport::RecordViewportCommands (lvk::VulkanAPI& vk, VkCommandBuffer& cmd, uint32_t frame_index, turas::View* view, turas::Pipeline* view_pipeline)
{
	std::array<VkClearValue, 2> clearValues {};
	clearValues[0].color		= { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo {};
	renderPassInfo.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass		 = m_PresentRenderPass;
	renderPassInfo.framebuffer		 = m_PresentFramebuffers[frame_index];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = VkExtent2D { m_Width, m_Height };

	renderPassInfo.clearValueCount = static_cast<uint32_t> (clearValues.size());
	renderPassInfo.pClearValues	   = clearValues.data();

	vkCmdBeginRenderPass (cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PresentPipelineData.m_Pipeline);
	VkViewport viewport {};
	viewport.x		  = 0.0f;
	viewport.x		  = 0.0f;
	viewport.width	  = static_cast<float> (m_Width);
	viewport.height	  = static_cast<float> (m_Height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor {};
	scissor.offset = { 0, 0 };
	scissor.extent = VkExtent2D { m_Width, m_Height };
	vkCmdSetViewport (cmd, 0, 1, &viewport);
	vkCmdSetScissor (cmd, 0, 1, &scissor);
	VkDeviceSize sizes[] = { 0 };

	vkCmdBindVertexBuffers (cmd, 0, 1, &view_pipeline->m_PresentQuad.m_VertexBuffer, sizes);
	vkCmdBindIndexBuffer (cmd, view_pipeline->m_PresentQuad.m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PresentPipelineData.m_PipelineLayout, 0, 1, &m_PresentMaterial.m_DescriptorSets[0].m_Sets[frame_index], 0, nullptr);
	vkCmdDrawIndexed (cmd, view_pipeline->m_PresentQuad.m_IndexCount, 1, 0, 0, 0);
	vkCmdEndRenderPass (cmd);
}
turas::Shader* turas::PresentToImageViewport::GetPresentShader()
{
	Shader* present_shader = Engine::INSTANCE->m_Renderer.GetShader ("Present");
	if(present_shader == nullptr) {
		present_shader = Engine::INSTANCE->m_Renderer.CreateShaderVF ("Present", "screen-space-uv.vert", "screen-space-image.frag");
	}
	if(present_shader == nullptr) {
		log::error("PresentToImageViewport : Failed to prepare present shader.");
	}

	return present_shader;
}

void turas::TurasImGuiViewport::Update ( turas::View* view, turas::Pipeline* view_pipeline)
{
	if(ImGui::Begin(view->m_Name.c_str())) {
		auto extent = ImGui::GetContentRegionAvail();
		auto max = Engine::INSTANCE->m_Renderer.m_VK.GetMaxFramebufferExtent();
		ImVec2 uv1 = { extent.x / max.width, extent.y / max.height };
		auto& image = view_pipeline->m_LvkPipeline.GetOutputFramebuffer()->m_ColourAttachments[0].m_AttachmentSwapchainImages[Engine::INSTANCE->m_Renderer.m_VK.GetFrameIndex()];

		ImGuiX::Image (image, extent, { 0, 0 }, uv1);
		lvk::DrawIm3dTextListsImGuiAsChild (Im3d::GetTextDrawLists(), Im3d::GetTextDrawListCount(),
			view->m_ViewWidth, view->m_ViewHeight, view->GetProjectionMatrix() * view->GetViewMatrix());
		view->m_ViewWidth	= static_cast<u32>(extent.x);
		view->m_ViewHeight	= static_cast<u32>(extent.y);
	}
	ImGui::End();
}
void turas::TurasImGuiViewport::RecordViewportCommands (lvk::VulkanAPI& vk, VkCommandBuffer& cmd, uint32_t frame_index, turas::View* view, turas::Pipeline* view_pipeline)
{
}
