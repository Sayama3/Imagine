//
// Created by ianpo on 29/10/2023.
//

#pragma once

#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine
{
	class Project;
	class ProjectSerializer
	{
	public:
		static bool SerializeReadable(const Project* project, const std::filesystem::path& filepath);
		static Scope<Project> DeserializeReadable(std::filesystem::path filepath);
	};

} // namespace Imagine

