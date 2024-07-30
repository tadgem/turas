//
// Created by liam_ on 7/30/2024.
//

#pragma once
#include "Core/Engine.h"
namespace turas {
    class EditorEngine : public Engine {
    	public:
    	EditorEngine();
    	void Run() override;
    	void OnEditor();
    };
}