#include "Rendering/Pipelines/Common.h"
#include "Rendering/VertexLayouts.h"
#include "STL/Array.h"
#include "Core/ECS.h"
#include "Systems/Transform.h"
#include "Systems/Mesh.h"
#include "Systems/Material.h"

lvk::VkPipelineData turas::Common::CreateStaticPipeline(lvk::VulkanAPI& vk,
        lvk::ShaderProgram& prog, lvk::Framebuffer* fb,
        VkPolygonMode polyMode, VkCullModeFlags cullMode, VkCompareOp depthCompareOp, bool enableMSAA)
{
    VkPipelineLayout pipelineLayout;
    auto bindingDescriptions = Vector<VkVertexInputBindingDescription>{lvk::VertexDataPosNormalUv::GetBindingDescription() };
    auto attributeDescriptions = lvk::VertexDataPosNormalUv::GetAttributeDescriptions();
    VkPipeline pipeline = vk.CreateRasterizationGraphicsPipeline(
        prog, bindingDescriptions, attributeDescriptions,
        fb->m_RenderPass, vk.m_SwapChainImageExtent.width, vk.m_SwapChainImageExtent.height, polyMode, cullMode,
        enableMSAA, depthCompareOp, pipelineLayout, static_cast<u32>(fb->m_ColourAttachments.size())
        );

    return {pipeline, pipelineLayout};
}


turas::Common::BuiltInGBufferCommandDispatcher::BuiltInGBufferCommandDispatcher(lvk::Framebuffer *framebuffer, lvk::VkPipelineData pipelineData) :
m_GBuffer(framebuffer), m_PipelineData(pipelineData)
{

}

void turas::Common::BuiltInGBufferCommandDispatcher::RecordCommands(VkCommandBuffer commandBuffer, turas::u32 frameIndex,
                                                                    turas::Scene *scene) {

        Array<VkClearValue, 4> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[2].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[3].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_GBuffer->m_RenderPass;
        renderPassInfo.framebuffer = m_GBuffer->m_SwapchainFramebuffers[frameIndex];
        renderPassInfo.renderArea.offset = { 0,0 };
        renderPassInfo.renderArea.extent = m_GBuffer->m_Resolution;

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineData.m_Pipeline);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.x = 0.0f;
        viewport.width = static_cast<float>(m_GBuffer->m_Resolution.width);
        viewport.height = static_cast<float>(m_GBuffer->m_Resolution.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0,0 };
        scissor.extent = VkExtent2D{
                static_cast<uint32_t>(m_GBuffer->m_Resolution.width) ,
                static_cast<uint32_t>(m_GBuffer->m_Resolution.height)
        };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        // set push consts
        // vkCmdPushConstants(commandBuffer, m_PipelineData.m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PCViewData), &pcData);
        auto view = scene->GetRegistry().view<TransformComponent, MeshComponent, MaterialComponent>();
        // despatch scene render
        for(auto [e, transform, mesh, material] : view.each())
        {
        }
//        for (auto& renderable : renderables)
//        {
//            renderable.RecordGraphicsCommands(commandBuffer);
//        }
        vkCmdEndRenderPass(commandBuffer);

}

void turas::Common::DispatchStaticMeshDrawCommands(View* view, turas::u64 shaderHash, lvk::VkPipelineData pipelineData, turas::Scene* scene)
{
    auto scene_view = scene->GetRegistry().view<TransformComponent, MeshComponent, MaterialComponent>();
    // dispatch scene render
    for(auto [e, transform, mesh, material] : scene_view.each())
    {
        // we should probably think of a better way to do this, so we dont need to iterate over the entire list of draws
        if(material.m_ShaderHash != shaderHash) continue;
    }
}