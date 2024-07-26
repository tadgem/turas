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
            BuiltInGBufferCommandDispatcher(u64 shader_hash, lvk::Framebuffer* framebuffer, lvk::VkPipelineData pipeline_data);
            lvk::Framebuffer*       m_GBuffer;
            lvk::VkPipelineData     m_PipelineData;
            const u64 m_ShaderHash;
            void RecordCommands(VkCommandBuffer buffer, u32 frame_index, View* view, Scene *scene) override;

        public:

        };

        // Create a render pipeline for static meshes
        lvk::VkPipelineData CreateStaticMeshPipeline(lvk::VulkanAPI& vk,
                lvk::ShaderProgram& prog,
                lvk::Framebuffer* fb,
                VkPolygonMode poly_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT,
                VkCompareOp depth_compare_op = VK_COMPARE_OP_LESS, bool enable_msaa = false);

        static void DispatchStaticMeshDrawCommands(VkCommandBuffer cmd, uint32_t frame_index, View* view, u64 shader_hash, lvk::VkPipelineData pipeline_data, Scene* scene);
    };
}