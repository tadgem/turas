//
// Created by liam_ on 7/30/2024.
//

#pragma once
#include "Core/Engine.h"
#include "Core/FSM.h"

namespace turas {
    class EditorEngine : public Engine {
    	public:
    	enum Mode {
    		Edit,
			Debug,
			OpenProject
		};

    	enum Trigger {
    		Play,
			Stop,
			OpenProjectFinished,
		};

    	EditorEngine();
    	void Run() override;
    	void AddViewsAndViewports() override;
    	void OnEditor();
    	void InitEditorFSM();

    	// FSM States
		int		OnEditUpdate();
    	void	OnEditEnter();
    	void	OnEditExit();

    	int		OnRuntimeUpdate();
    	void	OnRuntimeEnter();
    	void	OnRuntimeExit();

    	int		OnOpenProjectUpdate();
    	void	OnOpenProjectEnter();
    	void	OnOpenProjectExit();
    	int    OpenProjectImGui();
    	FSM	m_EditorFSM;
    };
}