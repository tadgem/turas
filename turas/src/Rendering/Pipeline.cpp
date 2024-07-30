#include "Rendering/Pipeline.h"

void turas::Pipeline::Free(lvk::VulkanAPI &vk) {
	m_LvkPipeline.Free(vk);
	m_PresentQuad.Free (vk);
}