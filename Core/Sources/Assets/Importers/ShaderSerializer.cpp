//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/ShaderSerializer.hpp"
#include "Imagine/Core/FileSystem.hpp"


namespace Imagine::Core
{
	static inline constexpr std::array<const char* const, 17> Extensions {".glsl", ".shader", ".compute", ".comp", ".cs", ".vert", ".vs", ".tessco", ".tcs", ".tessev", ".tes", ".geom", ".gs", ".frag", ".fs", ".pixel", ".ps"};

	bool ShaderSerializer::IsShaderSource(const std::filesystem::path &path)
	{
		std::string extension = path.extension().string();

		std::transform(extension.begin(), extension.end(), extension.begin(), [](char c){ return std::tolower(c);});

		return path.has_extension() && std::find(Extensions.begin(), Extensions.end(), extension) != Extensions.end();
	}

} // namespace Imagine::Core
