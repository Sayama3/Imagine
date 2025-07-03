//
// Created by Sayama on 26/10/2023.
//

#pragma once

#include "Imagine/Assets/AssetManagerBase.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Scene/Scene.hpp"


namespace Imagine::Core
{
	using void_func_ptr = std::function<void()>;

	struct ProjectConfig {
		std::string name = "Untitled";

		std::filesystem::path assetDirectory = "Assets";
		std::filesystem::path cacheDirectory = "Cache";
		std::filesystem::path scriptsDirectory = "Scripts";

		std::filesystem::path assetRegistryPath = "AssetRegistry.mgn";

		std::optional<AssetHandle> startSceneId;
		std::optional<AssetHandle> defaultMaterialId{std::nullopt};
		std::optional<AssetHandle> defaultShaderId{std::nullopt};
		std::optional<AssetHandle> deferredShaderId{std::nullopt};
		std::optional<AssetHandle> gizmoShaderId{std::nullopt};

		//TODO: Add script path once i've got scripting (i.e. C#/Lua/...).
	};


	class Project
	{
	public:
		inline static Ref<Project> GetActive() { return s_ActiveProject; }

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

		static void SetGizmoShader(AssetHandle id);
		static AssetHandle GetGizmoShader();

		static void SetDefaultMaterial(AssetHandle id);
		static AssetHandle GetDefaultMaterial();

		static void SetDefaultShader(AssetHandle id);
		static AssetHandle GetDefaultShader();

		static void SetDeferredShader(AssetHandle id);
		static AssetHandle GetDeferredShader();

		static const ProjectConfig& GetConfig();
		inline static bool ProjectIsLoaded() { return s_ActiveProject != nullptr; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path&);
		static bool SaveActive(const std::filesystem::path& path);
		static bool SaveActive();

		static AssetHandle AddOnLoad(const void_func_ptr&);
		static void RemoveOnLoad(AssetHandle id);
	public: // all the public members
		Project();
		Project(ProjectConfig parameters);
		~Project();

		[[nodiscard]] inline Ref<AssetManagerBase> GetAssetManager() { return m_AssetManager; }

		// [[nodiscard]] inline Ref<EditorAssetManager> GetEditorAssetManager() {
		// 	MGN_PROFILE_FUNCTION();
		// 	return CastPtr<EditorAssetManager>(m_AssetManager);
		// }
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
		std::filesystem::path m_ProjectPath = "./Project.mgn";
		ProjectConfig m_Config;
		Ref<AssetManagerBase> m_AssetManager;
	private:
		static std::unordered_map<AssetHandle, void_func_ptr>* s_OnLoad;
		static Ref<Project> s_ActiveProject;
		friend class ProjectSerializer;
	};

} // namespace Imagine::Core

