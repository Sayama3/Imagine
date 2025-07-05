//
// Created by ianpo on 09/04/2024.
//

#include "Imagine/Core/InternalCore.hpp"
#include "Imagine/Math/Image.hpp"
#include "Imagine/Assets/Importers/TextureImporter.hpp"
#include <stb_image.h>

namespace Imagine::Core
{
	// cf. std_image to see the extensions supported.
	static inline constexpr std::array<const char* const, 12> Extensions2D = {".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm", ".gif"};

	bool TextureImporter::IsTexture2D(const std::filesystem::path &path)
	{
		MGN_PROFILE_FUNCTION();
		std::string extension = path.extension().string();

		std::transform(extension.begin(), extension.end(), extension.begin(), [](char c){ return std::tolower(c);});

		return path.has_extension() && std::find(Extensions2D.begin(), Extensions2D.end(), extension) != Extensions2D.end();
	}
}