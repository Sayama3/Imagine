//
// Created by ianpo on 09/04/2024.
//

#include "Imagine/Core/InternalCore.hpp"
#include "Imagine/Math/Image.hpp"
#include "Imagine/Assets/Importers/TextureSerializer.hpp"
#include <stb_image.h>

namespace Imagine
{
	// cf. std_image to see the extensions supported.
	static inline constexpr std::array<const char* const, 12> Extensions2D = {".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm", ".gif"};

	bool TextureSerializer::IsTexture2D(const std::filesystem::path &path) {
		return path.filename().string().ends_with(".img.mgn");
	}
} // namespace Imagine
