//
// Created by Sayama on 26/10/2023.
//

#pragma once

#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Assets/AssetManagerBase.hpp"
#include "Imagine/Assets/FileAssetManager.hpp"
#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine
{
	class Scene;
	using void_func_ptr = std::function<void()>;
	class ProjectLayer;

	struct ProjectConfig {
		std::string name = "Untitled";

		std::filesystem::path assetDirectory = "Assets";
		std::filesystem::path cacheDirectory = "Cache";
		std::filesystem::path scriptsDirectory = "Scripts";

		std::filesystem::path assetRegistryPath = "AssetRegistry.mgn";

		AssetHandle startSceneId{NULL_ASSET_HANDLE};
	};


	class Project
	{
		friend ProjectLayer;
	public:
		inline static Project* GetActive() { return s_ActiveProject.get(); }
		inline static void Shutdown() {s_ActiveProject.reset();}

		static std::filesystem::path GetAssetsDirectory();
		static std::filesystem::path GetCacheDirectory();
		static std::filesystem::path GetScriptsDirectory();
		static std::filesystem::path GetAssetRegistryPath();
		static const std::filesystem::path& GetProjectFilePath();

		static void ResetMainScene();
		static void SetMainScene(AssetHandle id);
		static void SetMainScene(const Scene& scene);
		static void SetMainScene(const Ref<Scene>& scene);
		static AssetHandle GetMainScene();

		static const ProjectConfig& GetConfig();
		inline static bool ProjectIsLoaded() { return s_ActiveProject != nullptr; }

		static Project* New();
		static Project* Load(const std::filesystem::path&);
		static bool SaveActive(const std::filesystem::path& path);
		static bool SaveActive();

		static AssetHandle AddOnLoad(const void_func_ptr&);
		static void RemoveOnLoad(AssetHandle id);
	public: // all the public members
		Project();
		Project(ProjectConfig parameters);
		~Project();

		[[nodiscard]] inline AssetManagerBase* GetAssetManager() { return m_AssetManager.get(); }

		[[nodiscard]] inline FileAssetManager* GetFileAssetManager() {
			return dynamic_cast<FileAssetManager*>(m_AssetManager.get());
		}
		//
		// [[nodiscard]] inline Ref<RuntimeAssetManager> GetRuntimeAssetManager() {
		// 	MGN_PROFILE_FUNCTION();
		// 	return CastPtr<RuntimeAssetManager>(m_AssetManager);
		// }
	private: // the rest of the private things
		void CallOnLoad();
		static std::unordered_map<AssetHandle, void_func_ptr>& GetOnLoad();
	private:
		[[nodiscard]] std::filesystem::path GetAsset() const;
		[[nodiscard]] std::filesystem::path GetCache() const;
		[[nodiscard]] std::filesystem::path GetScripts() const;
		[[nodiscard]] std::filesystem::path GetAssetRegistry() const;
		[[nodiscard]] const std::filesystem::path& GetFilePath() const;
	private:
		std::filesystem::path m_ProjectPath = "Project.mgn";
		ProjectConfig m_Config;
		Scope<AssetManagerBase> m_AssetManager;
	private:
		static std::unordered_map<AssetHandle, void_func_ptr>* s_OnLoad;
		static Scope<Project> s_ActiveProject;
		friend class ProjectSerializer;
	};

} // namespace Imagine

