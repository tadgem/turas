//
// Created by liam_ on 7/21/2024.
//

#pragma once
#include "Rendering/Pipeline.h"

namespace turas {
    namespace Common {

        class BuiltInGBufferCommandDispatcher : public PipelineCommandDispatcher
        {
        public:
            BuiltInGBufferCommandDispatcher(lvk::Framebuffer* framebuffer, lvk::VkPipelineData pipelineData);
            lvk::Framebuffer*       m_GBuffer;
            lvk::VkPipelineData     m_PipelineData;

            void RecordCommands(VkCommandBuffer buffer, u32 frameIndex, Scene *scene) override;

        public:

        };

        // Create a render pipeline for static meshes
        static lvk::VkPipelineData CreateStaticPipeline(lvk::VulkanAPI& vk,
                lvk::ShaderProgram& prog,
                lvk::Framebuffer* fb,
                VkPolygonMode polyMode = VK_POLYGON_MODE_FILL, VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
                VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS, bool enableMSAA = false);
    };
}