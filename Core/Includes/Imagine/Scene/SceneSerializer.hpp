//
// Created by ianpo on 11/07/2025.
//

#pragma once

namespace Imagine {

	class Scene;

	class SceneSerializer {
	private:
		static inline constexpr const char* const c_EntitiesFolder{"Entities"};
		static inline constexpr const char* const c_ComponentsFolder{"Components"};
	public:
		static void SerializeReadable(Scene* scene, const std::filesystem::path& folderPath);
		static Scene* DeserializeReadable(const std::filesystem::path& folderPath);
	};

} // namespace Imagine
