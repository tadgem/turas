//
// Created by liam_ on 7/21/2024.
//

#pragma once
#include "Rendering/Pipeline.h"
#include "Rendering/View.h"

namespace turas {
    namespace Common {

        struct MVPPushConstData
        {
            glm::mat4   m_Model;
            glm::mat4   m_View;
            glm::mat4   m_Proj;
        };

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

        static void DispatchStaticMeshDrawCommands(View* view, u64 shaderHash, lvk::VkPipelineData pipelineData, Scene* scene);
    };
}