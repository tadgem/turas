#include "Rendering/Pipelines/BuiltIn.h"

#include "Core/Engine.h"
#include "Rendering/Pipelines/Common.h"
turas::Pipeline* turas::Rendering::BuiltIn::CreateBuiltInDeferredPipeline (turas::Renderer* renderer)
{
	using namespace turas;
	auto& vk	  = renderer->m_VK;
	auto* p		  = new Pipeline();
	p->m_PresentQuad = Rendering::CreateScreenQuad(renderer->m_VK);

	auto* gbuffer = p->m_LvkPipeline.AddFramebuffer (renderer->m_VK);
	// Position
	gbuffer->AddColourAttachment (vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
								  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								  VK_IMAGE_ASPECT_COLOR_BIT);
	// Normal
	gbuffer->AddColourAttachment (vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
								  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								  VK_IMAGE_ASPECT_COLOR_BIT);
	// Diffuse
	gbuffer->AddColourAttachment (vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
								  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								  VK_IMAGE_ASPECT_COLOR_BIT);
	// Depth
	gbuffer->AddDepthAttachment (vk, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT,
								 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	gbuffer->Build (vk);
	Shader* gbufferShader = renderer->CreateShaderVF ("gbuffer-standard.vert", "gbuffer-standard.frag", "gbuffer-standard");

	auto* lightPassImage = p->m_LvkPipeline.AddFramebuffer (vk);
	lightPassImage->AddColourAttachment (renderer->m_VK, lvk::ResolutionScale::Full, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT,
										 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
										 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	lightPassImage->Build (vk);

	Shader* lightPassShader = renderer->CreateShaderVF ("lightpass-standard.vert", "lightpass-standard.frag", "lightpass");
	auto*	lightPassMat	= p->m_LvkPipeline.AddMaterial (vk, lightPassShader->m_ShaderProgram);
	lightPassMat->SetColourAttachment (vk, "u_PositionBufferSampler", *gbuffer, 0);
	lightPassMat->SetColourAttachment (vk, "u_NormalBufferSampler", *gbuffer, 1);
	lightPassMat->SetColourAttachment (vk, "u_ColourBufferSampler", *gbuffer, 2);
	p->m_StateUpdaters.push_back (CreateUnique<BuiltInLightPassStateUpdater> (lightPassMat));


	auto* im3dViewState = p->m_LvkPipeline.AddIm3d (renderer->m_VK, Engine::INSTANCE->m_Im3dState);

	p->m_LvkPipeline.SetOutputFramebuffer (lightPassImage);

	lvk::VkPipelineData gbufferPipelineData	  = Rendering::CreateStaticMeshPipeline (vk, gbufferShader->m_ShaderProgram, gbuffer);
	lvk::VkPipelineData lightPassPipelineData = Rendering::CreateStaticMeshPipeline (vk, lightPassShader->m_ShaderProgram, lightPassImage,
																					 VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE);
	p->m_CommandDispatchers.push_back (
		CreateUnique<Rendering::BuiltInGBufferCommandDispatcher> (gbufferShader->m_ShaderHash, gbuffer, gbufferPipelineData));

	p->m_CommandDispatchers.push_back (CreateUnique<Rendering::BuiltInLightPassCommandDispatcher> (
		lightPassShader->m_ShaderHash,
		lightPassImage,
		lightPassPipelineData,
		&p->m_PresentQuad,
		*im3dViewState,
		lightPassMat));


	return p;
}
