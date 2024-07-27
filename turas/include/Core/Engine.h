#pragma once
#include <type_traits>
#include "Assets/AssetManager.h"
#include "Core/Project.h"
#include "Core/System.h"
#include "Debug/Profile.h"
#include "Im3D/im3d_lvk.h"
#include "Rendering/Renderer.h"
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "VulkanAPI_SDL.h"
#include "efsw/efsw.hpp"
namespace turas
{
	class Scene;
	class TurasFilesystemListener : public efsw::FileWatchListener
	{
	public:
		void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action,
							  std::string oldFilename) override;
	};
	/**
	 * @brief Engine
	 * @author Liam Devlin
	 *
	 * Top level container for all engine functionality
	 */
	class Engine
	{
	public:
						  Engine(bool enableDebugUpdate = false);
		void			  Init();
		void			  Shutdown();
		void			  Run();
		bool			  LoadProject(const String& path);
		bool			  CreateProject(const String& name, const String& projectDir);
		bool			  SaveProject();
		Scene*			  CreateScene(const String& name);
		Scene*			  LoadSceneFromArchive(BinaryInputArchive& archive);
		Scene*			  LoadSceneFromPath(const String& path);
		Scene*			  LoadSceneFromName(const String& name);
		bool			  SaveScene(Scene* s);
		void			  CloseScene(Scene* scene);
		void			  CloseAllScenes();
		AssetLoadProgress GetSceneLoadProgress(Scene* scene);
		void			  PrepFrame();
		void			  SubmitFrame();
		void			  SystemsUpdate();
		void			  PendingScenes();
		inline bool		  IsDebugEnabled() { return p_DebugUpdateEnabled; }
		template <typename _Ty, typename... Args>
		_Ty* AddSystem(Args&&... args)
		{
			ZoneScoped;
			static_assert(std::is_base_of<System, _Ty>());
			return static_cast<_Ty*>(m_EngineSubSystems.emplace_back(std::move(CreateUnique<_Ty>(std::forward<Args>(args)...))).get());
		}
		inline static Engine*		  INSTANCE = nullptr;
		Vector<UPtr<System>>		  m_EngineSubSystems;
		Vector<UPtr<Scene>>			  m_ActiveScenes;
		Vector<Scene*>				  m_PendingScenes;
		AssetManager				  m_AssetManager;
		Renderer					  m_Renderer;
		lvk::LvkIm3dState			  m_Im3dState;
		UPtr<Project>				  m_Project;
		UPtr<efsw::FileWatcher>		  m_FileWatcher;
		UPtr<TurasFilesystemListener> m_UpdateListener;
		efsw::WatchID				  m_GlobalProjectWatchId;
		TURAS_IMPL_ALLOC(Engine)
	protected:
		void		 AddBuiltInPipelines();
		virtual void AddGamePipelines();
		void		 ChangeWorkingDirectory(const String& newDirectory);
		void		 CopyShadersToProject();
		void		 InitImGuiStyle();
		virtual void InitIm3dContexts();
		void		 DebugInit();
		void		 DebugUpdate();
		void		 PostLoadProject();
#ifdef TURAS_ENABLE_MEMORY_TRACKING
		DebugMemoryTracker p_DebugMemoryTracker;
#endif
		String p_OriginalWorkingDir;
		bool   p_DebugUpdateEnabled;
	};
} // namespace turas
