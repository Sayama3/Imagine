//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUTexture2D.hpp"

namespace Imagine {
namespace Core {
		CPUTexture2D::CPUTexture2D(const Image<uint8_t> &i) : image(i) {
		}
		CPUTexture2D::CPUTexture2D(Image<uint8_t> &&i) : image(std::move(i)) {
		}
	} // Core
} // Imagine