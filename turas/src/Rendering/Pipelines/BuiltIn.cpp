#include "Rendering/Pipelines/BuiltIn.h"
#include "Rendering/Pipelines/Common.h"
turas::Pipeline* turas::Rendering::BuiltIn::CreateBuiltInDeferredPipeline(turas::Renderer* renderer)
{
	using namespace turas;
	auto& vk	  = renderer->m_VK;
	auto* p		  = new Pipeline();
	auto* gbuffer = p->m_LvkPipeline.AddFramebuffer(renderer->m_VK);
	// Position
	gbuffer->AddColourAttachment(vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
								 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								 VK_IMAGE_ASPECT_COLOR_BIT);
	// Normal
	gbuffer->AddColourAttachment(vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
								 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								 VK_IMAGE_ASPECT_COLOR_BIT);
	// Diffuse
	gbuffer->AddColourAttachment(vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
								 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								 VK_IMAGE_ASPECT_COLOR_BIT);
	// Depth
	gbuffer->AddDepthAttachment(vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT,
								VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
								VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	gbuffer->Build(vk);
	Shader* gbufferShader  = renderer->CreateShaderVF("gbuffer-standard.vert", "gbuffer-standard.frag", "gbuffer-standard");
	auto*	lightPassImage = p->m_LvkPipeline.AddFramebuffer(vk);
	lightPassImage->AddColourAttachment(renderer->m_VK, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
										VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
										VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	lightPassImage->Build(vk);
	Shader* lightPassShader = renderer->CreateShaderVF("lightpass-standard.vert", "lightpass-standard.frag", "lightpass");
	auto*	lightPassMat	= p->m_LvkPipeline.AddMaterial(vk, lightPassShader->m_ShaderProgram);
	lightPassMat->SetColourAttachment(vk, "positionBufferSampler", *gbuffer, 1);
	lightPassMat->SetColourAttachment(vk, "normalBufferSampler", *gbuffer, 2);
	lightPassMat->SetColourAttachment(vk, "colourBufferSampler", *gbuffer, 0);
	p->m_LvkPipeline.SetOutputFramebuffer(lightPassImage);
	lvk::VkPipelineData gbufferPipelineData	  = Rendering::CreateStaticMeshPipeline(vk, gbufferShader->m_ShaderProgram, gbuffer);
	lvk::VkPipelineData lightPassPipelineData = Rendering::CreateStaticMeshPipeline(vk, lightPassShader->m_ShaderProgram, lightPassImage,
																					VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE);
	p->m_Renderers.push_back(
			CreateUnique<Rendering::BuiltInGBufferCommandDispatcher>(gbufferShader->m_ShaderHash, gbuffer, gbufferPipelineData));
	return p;
}
