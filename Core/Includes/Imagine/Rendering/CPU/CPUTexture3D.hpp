//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/Buffer.hpp"

namespace Imagine::Core {

	class CPUTexture3D final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture3D);
	private:
		Buffer m_Buffer;
		uint32_t m_width, m_height, m_depth, m_channels, m_pixelSize;
	};
}