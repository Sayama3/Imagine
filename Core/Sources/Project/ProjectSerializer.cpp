//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Project/ProjectSerializer.hpp"
#include "Imagine/Project/Project.hpp"

namespace Imagine::Core {

	ProjectSerializer::ProjectSerializer(Ref<Project>) {
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path &filepath) {
		return false;
	}
	bool ProjectSerializer::Deserialize(const std::filesystem::path &filepath) {
		return false;
	}
} // namespace Imagine::Core
