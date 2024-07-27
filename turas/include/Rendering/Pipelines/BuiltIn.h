//
// Created by liam_ on 7/27/2024.
//
#pragma once
#include "Rendering/Renderer.h"
namespace turas
{
	namespace Rendering
	{
		namespace BuiltIn
		{
			turas::Pipeline* CreateBuiltInDeferredPipeline(turas::Renderer* renderer);
		};
	} // namespace Rendering
} // namespace turas
