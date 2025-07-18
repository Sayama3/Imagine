//
// Created by Iannis on 17/06/2025.
//

#include "Imagine/Scene/SceneManager.hpp"

#include "Imagine/Core/SmartPointers.hpp"
namespace Imagine {
	SceneManager::SceneRef SceneManager::GetMainScene() {
		return s_MainScene;
	}

	SceneManager::SceneRef SceneManager::GetScene(UUID id) {
		auto it = std::find_if(s_LoadedScenes.begin(), s_LoadedScenes.end(), [id](const std::pair<float, SceneRef> &s) { return s.second->GetID() == id; });
		if (it != s_LoadedScenes.end()) {
			return it->second;
		}
		return nullptr;
	}

	std::vector<SceneManager::SceneRef> SceneManager::GetLoadedScenes() {
		std::vector<SceneRef> scenes{};
		scenes.reserve(s_LoadedScenes.size());
		for (const auto &[flt, pScene]: s_LoadedScenes) {
			scenes.push_back(pScene);
		}
		return scenes;
	}

	SceneManager::SceneRef SceneManager::CreateScene(float priority, bool isMainScene) {
		SceneRef s = std::make_shared<Scene>();
		LoadScene(s, priority, isMainScene);
		return s;
	}

	SceneManager::SceneRef SceneManager::LoadScene(const Scene &scene, float priority, bool isMainScene) {
		SceneRef s = std::make_shared<Scene>(scene);
		LoadScene(s, priority, isMainScene);
		return s;
	}

	void SceneManager::LoadScene(SceneRef scene, float priority, bool isMainScene) {
		s_LoadedScenes.insert({priority, scene});
		if (isMainScene) s_MainScene = scene;
	}

	void SceneManager::UnloadScene(SceneRef scene) {
		auto it = std::find_if(s_LoadedScenes.begin(), s_LoadedScenes.end(), [scene](const std::pair<float, SceneRef> &s) { return s.second.get() == scene.get(); });
		if (it != s_LoadedScenes.end()) {
			s_LoadedScenes.erase(it);
		}
	}

	SceneManager::SceneRef SceneManager::UnloadScene(UUID id) {
		SceneRef scene = nullptr;
		auto it = std::find_if(s_LoadedScenes.begin(), s_LoadedScenes.end(), [id](const std::pair<float, SceneRef> &s) { return s.second->GetID() == id; });
		if (it != s_LoadedScenes.end()) {
			scene = it->second;
			s_LoadedScenes.erase(it);
		}
		return scene;
	}
	void SceneManager::Intialize() {
		s_MainScene = CreateRef<Scene>();
		s_LoadedScenes.emplace(0.0f, s_MainScene);
	}

	void SceneManager::Shutdown() {
		s_MainScene.reset();
		s_LoadedScenes.clear();
	}
} // namespace Imagine
