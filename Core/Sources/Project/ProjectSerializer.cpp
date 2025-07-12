//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Project/ProjectSerializer.hpp"
#include "Imagine/Project/Project.hpp"

namespace Imagine {

	ProjectSerializer::ProjectSerializer(Ref<Project>) {
	}

	bool ProjectSerializer::SerializeReadable(const std::filesystem::path &filepath) {
		return false;
	}
	bool ProjectSerializer::DeserializeReadable(const std::filesystem::path &filepath) {
		return false;
	}
} // namespace Imagine
