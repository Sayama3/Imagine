//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Project/Project.hpp"

#include "Imagine/Core/Profiling.hpp"
#include "Imagine/Scene/Scene.hpp"

namespace Imagine::Core {
std::unordered_map<AssetHandle, void_func_ptr>* Project::s_OnLoad = nullptr;
	Ref<Project> Project::s_ActiveProject = nullptr;

	// void TryLoadGizmoMaterial()
	// {
	// 	if(!Project::ProjectIsLoaded()) return;
	// 	const ProjectConfig& config = Project::GetConfig();
	// 	if(!config.gizmoShaderId.has_value()) return;
	// 	ShaderField gizmoMat {config.gizmoShaderId.value()};
	// 	if(!gizmoMat.IsValid()) return;
	// 	MGN_CORE_INFO("Assigning a new Gizmo Material to the PhysicsDebugRenderer.");
	// 	PhysicsDebugRenderer::s_Shader = gizmoMat.GetAsset();
	// }

	std::unordered_map<AssetHandle, void_func_ptr>& Project::GetOnLoad()
	{
		MGN_PROFILE_FUNCTION();
		if(Project::s_OnLoad == nullptr) {
			s_OnLoad = new std::unordered_map<AssetHandle, void_func_ptr>();
			// s_OnLoad->insert({AssetHandle{}, &TryLoadGizmoMaterial});
		}
		return *Project::s_OnLoad;
	}

	AssetHandle Project::AddOnLoad(const void_func_ptr& func)
	{
		MGN_PROFILE_FUNCTION();
		AssetHandle id;
		GetOnLoad()[id] = func;
		return id;
	}

	void Project::RemoveOnLoad(AssetHandle id)
	{
		MGN_PROFILE_FUNCTION();
		const auto it = GetOnLoad().find(id);
		if (it != GetOnLoad().end())
		{
			GetOnLoad().erase(it);
		}
	}

	Project::Project()
	{
		if(m_ProjectPath.is_relative()) {
			m_ProjectPath = std::filesystem::current_path() / m_ProjectPath;
		}
	}

	Project::Project(ProjectConfig parameters) : m_Config(std::move(parameters))
	{
		if(m_ProjectPath.is_relative()) {
			m_ProjectPath = std::filesystem::current_path() / m_ProjectPath;
		}
	}

	Project::~Project() = default;

	Ref<Project> Project::New()
	{
		MGN_PROFILE_FUNCTION();
		s_ActiveProject = CreateRef<Project>();
		s_ActiveProject->CallOnLoad();
		return s_ActiveProject;
	}

	Ref<Project> Project::Load(const std::filesystem::path &path)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_ERROR("Not Implemented.");
		// Ref<Project> project = CreateRef<Project>();
		// auto old = s_ActiveProject;
		// s_ActiveProject = project;
		//
		// ProjectSerializer ps(project);
		// if(ps.Deserialize(path))
		// {
		// 	s_ActiveProject->CallOnLoad();
		// 	return s_ActiveProject;
		// } else {
		// 	s_ActiveProject = old;
		// }
		return nullptr;
	}

	bool Project::SaveActive()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project was not loaded.");
		MGN_CORE_ERROR("Not Implemented.");
		// ProjectSerializer ps(s_ActiveProject);
		// return ps.Serialize(s_ActiveProject->m_ProjectPath);
		return false;
	}

	bool Project::SaveActive(const std::filesystem::path &path)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project was not loaded.");
		MGN_CORE_CASSERT(!path.empty(), "When given, the path should not be empty.");
		MGN_CORE_ERROR("Not Implemented.");
		// ProjectSerializer ps(s_ActiveProject);
		// return ps.Serialize(path);
		return false;
	}

	void Project::CallOnLoad()
	{
		MGN_PROFILE_FUNCTION();
		for (auto&&[id, func] : GetOnLoad())
		{
			func();
		}
	}

	std::filesystem::path Project::GetAsset() const
	{
		MGN_PROFILE_FUNCTION();
		if(m_Config.assetDirectory.is_relative())
			return m_ProjectPath.parent_path() / m_Config.assetDirectory;
		else
			return m_Config.assetDirectory;
	}

	std::filesystem::path Project::GetCache() const
	{
		MGN_PROFILE_FUNCTION();
		if(m_Config.cacheDirectory.is_relative())
			return m_ProjectPath.parent_path() / m_Config.cacheDirectory;
		else
			return m_Config.cacheDirectory;
	}

	std::filesystem::path Project::GetScripts() const
	{
		MGN_PROFILE_FUNCTION();
		if(m_Config.scriptsDirectory.is_relative())
			return m_ProjectPath.parent_path() / m_Config.scriptsDirectory;
		else
			return m_Config.scriptsDirectory;
	}

	std::filesystem::path Project::GetAssetRegistry() const
	{
		MGN_PROFILE_FUNCTION();
		if(m_Config.assetRegistryPath.is_relative())
			return m_ProjectPath.parent_path() / m_Config.assetRegistryPath;
		else
			return m_Config.assetRegistryPath;
	}

	const std::filesystem::path& Project::GetFilePath() const
	{
		MGN_PROFILE_FUNCTION();
		return m_ProjectPath;
	}

	std::filesystem::path Project::GetAssetsDirectory()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		return s_ActiveProject->GetAsset();
	}
	std::filesystem::path Project::GetCacheDirectory()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		return s_ActiveProject->GetCache();
	}
	std::filesystem::path Project::GetScriptsDirectory()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		return s_ActiveProject->GetScripts();
	}

	std::filesystem::path Project::GetAssetRegistryPath()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		return s_ActiveProject->GetAssetRegistry();
	}

	const std::filesystem::path& Project::GetProjectFilePath()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		return s_ActiveProject->GetFilePath();
	}

	const ProjectConfig& Project::GetConfig()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		return s_ActiveProject->m_Config;
	}

	void Project::ResetMainScene()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.startSceneId = {};
	}

	void Project::SetMainScene(AssetHandle id)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.startSceneId = id;
	}

	void Project::SetMainScene(const Scene& scene)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.startSceneId = scene.Handle;
	}

	void Project::SetMainScene(const Ref<Scene>& scene)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.startSceneId = scene->Handle;
	}

	AssetHandle Project::GetMainScene()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		MGN_CORE_CASSERT(s_ActiveProject->m_Config.startSceneId.has_value(), "No Start Scene associated with the project.");
		return s_ActiveProject->m_Config.startSceneId.value_or(NULL_ASSET_HANDLE);
	}


	void Project::SetDefaultMaterial(AssetHandle id)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.defaultMaterialId = id;
	}
	AssetHandle Project::GetDefaultMaterial()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		MGN_CORE_CASSERT(s_ActiveProject->m_Config.defaultMaterialId.has_value(), "No Default Material associated with the project.");
		return s_ActiveProject->m_Config.defaultMaterialId.value_or(NULL_ASSET_HANDLE);
	}

	void Project::SetDefaultShader(AssetHandle id)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.defaultShaderId = id;
	}
	AssetHandle Project::GetDefaultShader()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		MGN_CORE_CASSERT(s_ActiveProject->m_Config.defaultShaderId.has_value(), "No Default Shader associated with the project.");
		return s_ActiveProject->m_Config.defaultShaderId.value_or(NULL_ASSET_HANDLE);
	}

	void Project::SetDeferredShader(AssetHandle id)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.deferredShaderId = id;
	}
	AssetHandle Project::GetDeferredShader()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		MGN_CORE_CASSERT(s_ActiveProject->m_Config.defaultShaderId.has_value(), "No Deferred Shader associated with the project.");
		return s_ActiveProject->m_Config.deferredShaderId.value_or(NULL_ASSET_HANDLE);
	}

	void Project::SetGizmoShader(AssetHandle id)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		s_ActiveProject->m_Config.gizmoShaderId = id;
		//TODO: Send the data to the renderer.
	}

	AssetHandle Project::GetGizmoShader()
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_CASSERT(s_ActiveProject, "The Active Project is not loaded yet.");
		MGN_CORE_CASSERT(s_ActiveProject->m_Config.gizmoShaderId.has_value(), "No Gizmo Shader associated with the project.");
		return s_ActiveProject->m_Config.gizmoShaderId.value_or(NULL_ASSET_HANDLE);
	}
} // namespace Imagine::Core
