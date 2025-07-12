//
// Created by ianpo on 29/10/2023.
//

#pragma once

#include "Imagine/Project/Project.hpp"


namespace Imagine
{
	class ProjectSerializer
	{
	private:
		Ref<Project> m_ProjectHandle;
	public:
		ProjectSerializer(Ref<Project>);

		bool SerializeReadable(const std::filesystem::path& filepath);
		bool DeserializeReadable(const std::filesystem::path& filepath);
	};

} // namespace Imagine

