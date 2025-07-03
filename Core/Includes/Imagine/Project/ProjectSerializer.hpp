//
// Created by ianpo on 29/10/2023.
//

#pragma once

#include "Imagine/Project/Project.hpp"


namespace Imagine::Core
{
	class ProjectSerializer
	{
	private:
		Ref<Project> m_ProjectHandle;
	public:
		ProjectSerializer(Ref<Project>);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	};

} // namespace Imagine::Core

