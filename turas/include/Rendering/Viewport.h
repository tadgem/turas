#pragma once
#include "STL/Array.h"
#include "Core/Types.h"
#include "Core/Input.h"
#include "Core/ECS.h"
#include "Rendering/View.h"
#include "Rendering/Pipeline.h"
namespace turas {
	struct ViewportInputState
	{
		u32 					MousePosX, MousePosY;
		Input::ButtonState		LMB, RMB;
	};

	class Viewport {
	public:
		u32 				m_Width;
		u32 				m_Height;
		ViewportInputState 	m_InputState;

		virtual void Update(Scene* scene, View* view, Pipeline* view_pipeline) = 0;
		virtual void RecordViewportCommands(lvk::VulkanAPI& vk ,VkCommandBuffer& cmd, uint32_t frame_index, Scene* scene, View* view, Pipeline* view_pipeline) = 0;
	};

	class PresentToImageViewport : public Viewport
	{
	public:
		VkRenderPass 										m_PresentRenderPass;
		Array<VkFramebuffer, lvk::MAX_FRAMES_IN_FLIGHT> 	m_PresentFramebuffers;

		PresentToImageViewport(VkRenderPass renderPass, Array<VkFramebuffer, lvk::MAX_FRAMES_IN_FLIGHT> framebuffers);

		void Update (Scene* scene, View* view, Pipeline* view_pipeline) override;
		void RecordViewportCommands (lvk::VulkanAPI& vk , VkCommandBuffer& cmd, uint32_t frame_index, Scene* scene, View* view, Pipeline* view_pipeline) override;
	};

	class TurasImGuiViewport : public Viewport
	{
	public:
		void Update (Scene* scene, View* view, Pipeline* view_pipeline) override;
		void RecordViewportCommands (lvk::VulkanAPI& vk, VkCommandBuffer& cmd, uint32_t frame_index, Scene* scene, View* view, Pipeline* view_pipeline) override;

	public:

	};
}