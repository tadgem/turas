#include "Rendering/Viewport.h"
void turas::PresentToImageViewport::Update (turas::Scene* scene, turas::View* view, turas::Pipeline* view_pipeline)
{
}
void turas::PresentToImageViewport::RecordViewportCommands (lvk::VulkanAPI& vk ,VkCommandBuffer& cmd, uint32_t frame_index, turas::Scene* scene, turas::View* view, turas::Pipeline* view_pipeline)
{
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_PresentRenderPass;
	renderPassInfo.framebuffer = m_PresentFramebuffers[frame_index];
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = VkExtent2D {m_Width, m_Height};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	// vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, lightingPassPipeline);
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.x = 0.0f;
	viewport.width = static_cast<float>(m_Width);
	viewport.height = static_cast<float>(m_Height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = VkExtent2D{m_Width ,m_Height};
	vkCmdSetViewport(cmd, 0, 1, &viewport);
	vkCmdSetScissor(cmd, 0, 1, &scissor);
	VkDeviceSize sizes[] = { 0 };

	vkCmdBindVertexBuffers(cmd, 0, 1, &view_pipeline->m_PresentQuad.m_VertexBuffer, sizes);
	vkCmdBindIndexBuffer(cmd, view_pipeline->m_PresentQuad.m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	//vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, lightingPassPipelineLayout, 0, 1, &lightingPassDescriptorSets[frameIndex], 0, nullptr);
	vkCmdDrawIndexed(cmd, view_pipeline->m_PresentQuad.m_IndexCount, 1, 0, 0, 0);
	vkCmdEndRenderPass(cmd);

}
turas::PresentToImageViewport::PresentToImageViewport (VkRenderPass renderPass, turas::Array<VkFramebuffer, 2> framebuffers) :
m_PresentRenderPass(renderPass), m_PresentFramebuffers(framebuffers)
{
}
