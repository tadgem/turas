//
// Created by liam_ on 7/10/2024.
//

#pragma once
#include "lvk/Pipeline.h"
#include "Core/Types.h"
#include "STL/Memory.h"

namespace turas {

    class Scene;
    // pure virtual class, implement per pipeline or use abstractions
    // pass in to pipeline to record commands for associated pipeline state
    // e.g. pass in dependencies to derived type constructor
    // Renderer will contain a map of name : pipeline creation function
    // function will add objects to pipeline and configure command dispatcher to record necessary commands
    class PipelineCommandDispatcher
    {
    public:
        virtual void RecordCommands(VkCommandBuffer buffer, u32 frameIndex, Scene* scene) = 0;
    };

    class Pipeline {
    public:

        UPtr<PipelineCommandDispatcher> m_Renderer;
    };
}