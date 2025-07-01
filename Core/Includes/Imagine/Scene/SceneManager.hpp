//
// Created by Iannis on 17/06/2025.
//

#pragma once

#include "Scene.hpp"

namespace Imagine::Core {
	class SceneManager {
	public:
		using SceneRef = Scene::Ref;
	public:
		static SceneRef GetMainScene();
		static SceneRef GetScene(UUID id);
		static std::vector<SceneRef> GetLoadedScenes();
		static SceneRef CreateScene(float priority = 1.0f, bool isMainScene = true);
		static SceneRef LoadScene(const Scene& scene,float priority = 1.0f,  bool isMainScene = true);
		static void LoadScene(SceneRef scene,float priority = 1.0f,  bool isMainScene = true);

		static void UnloadScene(SceneRef scene);
		static SceneRef UnloadScene(UUID id);

		static void Shutdown();

	private:
		static inline SceneRef s_MainScene = std::make_shared<Scene>();
		// Lowest order number is first
		static inline std::multimap<float, SceneRef> s_LoadedScenes{{0.0f, s_MainScene}};
	};
} // Imagine::Core
