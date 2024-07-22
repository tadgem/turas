//
// Created by liam_ on 7/21/2024.
//

#pragma once
#include "Rendering/Pipeline.h"
#include "Rendering/View.h"

namespace turas {
    namespace Rendering {

        struct MVPPushConstData
        {
            glm::mat4   m_Model;
            glm::mat4   m_View;
            glm::mat4   m_Proj;
        };

        class BuiltInGBufferCommandDispatcher : public PipelineCommandDispatcher
        {
        public:
            BuiltInGBufferCommandDispatcher(u64 shaderHash, lvk::Framebuffer* framebuffer, lvk::VkPipelineData pipelineData);
            lvk::Framebuffer*       m_GBuffer;
            lvk::VkPipelineData     m_PipelineData;
            const u64 m_ShaderHash;
            void RecordCommands(VkCommandBuffer buffer, u32 frameIndex, View* view, Scene *scene) override;

        public:

        };

        // Create a render pipeline for static meshes
        static lvk::VkPipelineData CreateStaticMeshPipeline(lvk::VulkanAPI& vk,
                lvk::ShaderProgram& prog,
                lvk::Framebuffer* fb,
                VkPolygonMode polyMode = VK_POLYGON_MODE_FILL, VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
                VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS, bool enableMSAA = false);

        static void DispatchStaticMeshDrawCommands(VkCommandBuffer cmd, uint32_t frameIndex, View* view, u64 shaderHash, lvk::VkPipelineData pipelineData, Scene* scene);
    };
}